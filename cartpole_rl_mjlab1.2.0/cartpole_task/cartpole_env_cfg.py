"""Cartpole balance and swingup environment configuration."""

from __future__ import annotations

import math
from pathlib import Path
from typing import TYPE_CHECKING

import mujoco
import torch

from mjlab.actuator.xml_actuator import (
    XmlMotorActuatorCfg,
)  # nightly version 移除了这个类 https://github.com/mujocolab/mjlab/commit/96fbe35320f40b689eb1b7ec5d9316fc13ac7901#diff-a22a541be1e878faf8e1e0220253df83361a9eeacc0659baa22bb0980befbb0e
from mjlab.entity import Entity, EntityArticulationInfoCfg, EntityCfg
from mjlab.envs import ManagerBasedRlEnvCfg

from mjlab.envs.mdp.actions import JointEffortActionCfg
from mjlab.managers.action_manager import ActionTermCfg
from mjlab.managers.event_manager import EventTermCfg
from mjlab.managers.observation_manager import (
    ObservationGroupCfg,
    ObservationTermCfg,
)
from mjlab.managers.reward_manager import RewardTermCfg
from mjlab.managers.scene_entity_config import SceneEntityCfg
from mjlab.managers.termination_manager import TerminationTermCfg
from mjlab.rl import (
    RslRlModelCfg,
    RslRlOnPolicyRunnerCfg,
    RslRlPpoAlgorithmCfg,
)
from mjlab.scene import SceneCfg
from mjlab.sim import MujocoCfg, SimulationCfg
from mjlab.terrains import TerrainEntityCfg
from mjlab.utils.lab_api.math import sample_uniform
from mjlab.viewer import ViewerConfig

if TYPE_CHECKING:
    from mjlab.envs import ManagerBasedRlEnv

_CARTPOLE_XML: Path = Path(__file__).parent / "cartpole.xml"
# 标签 这里的name需要和后续定义entities={"cartpole": _get_cartpole_cfg(swing_up=swing_up)}保持一致
_CART_CFG = SceneEntityCfg("cartpole", joint_names=("slider",))
_HINGE_CFG = SceneEntityCfg("cartpole", joint_names=("hinge_1",))
_CART_RESET_CFG = SceneEntityCfg("cartpole", joint_ids=[0])
_HINGE_RESET_CFG = SceneEntityCfg("cartpole", joint_ids=[1])


# Entity.
def _get_spec() -> mujoco.MjSpec:  # type: ignore
    # 将 XML 编译为 MuJoCo 的 MjSpec（后续由 scene 编译成 MjModel/MjData）
    return mujoco.MjSpec.from_file(str(_CARTPOLE_XML))  # type: ignore


# Tell mjlab to use the motor defined in the XML as is
_CARTPOLE_ARTICULATION = EntityArticulationInfoCfg(  # Articulation: 铰接系统
    # 注意这里传递的是 joint 的名字 而不是 motor 的名字
    # 比如一个joint有多个motor，这里相当于操控合力；如果单独操作每个motor需要考虑约束，太复杂
    actuators=(XmlMotorActuatorCfg(target_names_expr=("slider",)),),
)


# 关节初始条件
_SWINGUP_INIT = EntityCfg.InitialStateCfg(
    # 用 dict 指定初始状态
    joint_pos={"slider": 0.0, "hinge_1": 0},
    joint_vel={".*": 0.0},  # 传递了正则表达式 表示所有关节速度都初始化为0
)

_BALANCE_INIT = EntityCfg.InitialStateCfg(
    pos=(0.0, 0.0, 0.0),
    joint_pos={"slider": 0.0, "hinge_1": math.pi},
    joint_vel={".*": 0.0},
)


# 打包 将 MjSpec 加载器、执行器、初始状态 配置到一个 EntityCfg 中，供后续 scene 创建 Entity 时使用
def _get_cartpole_cfg(swing_up: bool = False) -> EntityCfg:
    return EntityCfg(
        spec_fn=_get_spec,
        articulation=_CARTPOLE_ARTICULATION,
        init_state=_SWINGUP_INIT if swing_up else _BALANCE_INIT,
    )


# ===== 观测 =====

from mjlab.envs.mdp import (
    joint_pos_rel,
    joint_vel_rel,
    reset_joints_by_offset,
    time_out,
)


# 旋转关节的位置用余弦和正弦表示，避免角度跳变问题
def pole_angle_cos_sin(
    env: ManagerBasedRlEnv,
    asset_cfg: SceneEntityCfg = _HINGE_CFG,
) -> torch.Tensor:
    """Cosine and sine of the pole hinge angle. Shape: [num_envs, 2]."""
    # 【疑似】框架会便利注册的函数的每一个缺省值和params（如ObservationTermCfg的参数），然后判断类型、填充缺少的属性（比如joint_ids）
    asset: Entity = env.scene[asset_cfg.name]
    angle = asset.data.joint_pos[:, asset_cfg.joint_ids]  # 获取的角度没有范围
    return torch.cat([torch.cos(angle), torch.sin(angle)], dim=-1)


# ===== 奖励 =====

# dm_control uses value_at_margin=0.1 by default.
_GAUSSIAN_SCALE = math.sqrt(-2 * math.log(0.1))
_QUADRATIC_SCALE = math.sqrt(1 - 0.1)


# 输入值和容忍范围，输出容忍度
def _gaussian_tolerance(x: torch.Tensor, margin: float) -> torch.Tensor:
    """Gaussian sigmoid tolerance: 1 at x=0, value_at_margin=0.1 at |x|=margin."""
    if margin == 0:
        return (x == 0).float()
    scaled = x / margin * _GAUSSIAN_SCALE
    return torch.exp(-0.5 * scaled**2)


def _quadratic_tolerance(x: torch.Tensor, margin: float) -> torch.Tensor:
    """Quadratic sigmoid tolerance: 1 at x=0, 0 at |x|>=margin."""
    if margin == 0:
        return (x == 0).float()
    scaled = x / margin * _QUADRATIC_SCALE
    return torch.clamp(1 - scaled**2, min=0.0)


def cartpole_smooth_reward(
    env: ManagerBasedRlEnv,
    cart_cfg: SceneEntityCfg = _CART_CFG,
    hinge_cfg: SceneEntityCfg = _HINGE_CFG,
) -> torch.Tensor:
    """dm_control smooth cartpole reward: upright * centered * small_control * small_vel.

    Args:
      env: The environment.
      cart_cfg: Entity config selecting the slider joint.
      hinge_cfg: Entity config selecting the hinge joint.
    """
    asset: Entity = env.scene[cart_cfg.name]

    # Pole angle cosine.
    hinge_angle = asset.data.joint_pos[:, hinge_cfg.joint_ids].squeeze(-1)
    pole_cos = torch.cos(hinge_angle - math.pi)
    upright = (pole_cos + 1) / 2

    # Cart position.
    cart_pos = asset.data.joint_pos[:, cart_cfg.joint_ids].squeeze(-1)
    centered = (1 + _gaussian_tolerance(cart_pos, margin=1.0)) / 2

    # Control effort (raw action from the policy).
    control = env.action_manager.action.squeeze(-1)
    small_control = (4 + _quadratic_tolerance(control, margin=1.0)) / 5

    # Pole angular velocity.
    hinge_vel = asset.data.joint_vel[:, hinge_cfg.joint_ids].squeeze(-1)
    small_velocity = (1 + _gaussian_tolerance(hinge_vel, margin=5.0)) / 2

    return upright * centered * small_control * small_velocity


def reset_joints_by_offset_no_limit(
    env: ManagerBasedRlEnv,
    env_ids: torch.Tensor | None,
    position_range: tuple[float, float],
    velocity_range: tuple[float, float],
    asset_cfg: SceneEntityCfg,
) -> None:
    if env_ids is None:
        env_ids = torch.arange(env.num_envs, device=env.device, dtype=torch.int)

    asset: Entity = env.scene[asset_cfg.name]
    default_joint_pos = asset.data.default_joint_pos
    assert default_joint_pos is not None
    default_joint_vel = asset.data.default_joint_vel
    assert default_joint_vel is not None

    joint_pos = default_joint_pos[env_ids][:, asset_cfg.joint_ids].clone()
    joint_pos += sample_uniform(*position_range, joint_pos.shape, env.device)
    # 相比于reset_joints_by_offset，这里不限制位置范围，适合摆动up时铰链的随机重置
    # 在最新版本里面已经修复了这个问题：unlimited的joint的range是inf

    joint_vel = default_joint_vel[env_ids][:, asset_cfg.joint_ids].clone()
    joint_vel += sample_uniform(*velocity_range, joint_vel.shape, env.device)

    joint_ids = asset_cfg.joint_ids
    if isinstance(joint_ids, list):
        joint_ids = torch.tensor(joint_ids, device=env.device)

    asset.write_joint_state_to_sim(
        joint_pos.view(len(env_ids), -1),
        joint_vel.view(len(env_ids), -1),
        env_ids=env_ids,
        joint_ids=joint_ids,
    )


# ===== 环境 =====
def _make_env_cfg(swing_up: bool = False) -> ManagerBasedRlEnvCfg:
    cart_cfg = SceneEntityCfg("cartpole", joint_names=("slider",))
    hinge_cfg = SceneEntityCfg("cartpole", joint_names=("hinge_1",))

    actor_terms = {
        "cart_pos": ObservationTermCfg(
            func=joint_pos_rel,
            params={"asset_cfg": cart_cfg},
        ),
        "pole_angle": ObservationTermCfg(
            func=pole_angle_cos_sin,
            params={"asset_cfg": hinge_cfg},
        ),
        "cart_vel": ObservationTermCfg(
            func=joint_vel_rel,
            params={"asset_cfg": cart_cfg},
        ),
        "pole_vel": ObservationTermCfg(
            func=joint_vel_rel,
            params={"asset_cfg": hinge_cfg},
        ),
    }

    observations = {
        "actor": ObservationGroupCfg(actor_terms, enable_corruption=True),
        "critic": ObservationGroupCfg({**actor_terms}),
    }

    actions: dict[str, ActionTermCfg] = {
        "effort": JointEffortActionCfg(
            entity_name="cartpole",
            actuator_names=("slider",),
            scale=1.0,
        ),
    }

    slider_range = (-0.1, 0.1) if not swing_up else (0.0, 0.0)
    events = {
        "reset_slider": EventTermCfg(
            func=reset_joints_by_offset,
            mode="reset",
            params={
                "position_range": slider_range,
                "velocity_range": (-0.01, 0.01),
                "asset_cfg": _CART_RESET_CFG,
            },
        ),
        "reset_hinge": EventTermCfg(
            func=reset_joints_by_offset_no_limit,
            mode="reset",
            params={
                "position_range": (-0.034, 0.034),
                "velocity_range": (-0.01, 0.01),
                "asset_cfg": _HINGE_RESET_CFG,
            },
        ),
    }

    rewards = {
        "smooth_reward": RewardTermCfg(
            func=cartpole_smooth_reward,
            weight=1.0,
            params={"cart_cfg": cart_cfg, "hinge_cfg": hinge_cfg},
        ),
    }

    terminations = {
        "time_out": TerminationTermCfg(func=time_out, time_out=True),
    }

    return ManagerBasedRlEnvCfg(
        scene=SceneCfg(
            terrain=TerrainEntityCfg(terrain_type="plane"),
            entities={"cartpole": _get_cartpole_cfg(swing_up=swing_up)},
            num_envs=1,
            env_spacing=4.0,
        ),
        observations=observations,
        actions=actions,
        events=events,
        rewards=rewards,
        terminations=terminations,
        viewer=ViewerConfig(
            origin_type=ViewerConfig.OriginType.ASSET_BODY,
            entity_name="cartpole",
            body_name="cart",
            distance=4.0,
            elevation=-15.0,
            azimuth=0.0,
        ),
        sim=SimulationCfg(
            # mujoco=MujocoCfg(timestep=0.01, disableflags=("contact",)),
            # 1.2.0 没有 disableflags
            mujoco=MujocoCfg(timestep=0.01),
        ),
        decimation=5,  # 仿真是100Hz更新，控制是其1/5
        episode_length_s=50.0,
    )


def cartpole_balance_env_cfg(
    play: bool = False,
) -> ManagerBasedRlEnvCfg:
    cfg = _make_env_cfg(swing_up=False)
    if play:
        cfg.episode_length_s = 1e10
        cfg.observations["actor"].enable_corruption = False
    return cfg


def cartpole_swingup_env_cfg(
    play: bool = False,
) -> ManagerBasedRlEnvCfg:
    cfg = _make_env_cfg(swing_up=True)
    if play:
        cfg.episode_length_s = 1e10
        cfg.observations["actor"].enable_corruption = False
    return cfg


# RL config.


def cartpole_ppo_runner_cfg() -> RslRlOnPolicyRunnerCfg:
    return RslRlOnPolicyRunnerCfg(
        actor=RslRlModelCfg(
            hidden_dims=(64, 64),
            activation="elu",
            obs_normalization=False,
            distribution_cfg={
                "class_name": "GaussianDistribution",
                "init_std": 1.0,
                "std_type": "scalar",
            },
        ),
        critic=RslRlModelCfg(
            hidden_dims=(64, 64),
            activation="elu",
            obs_normalization=False,
        ),
        algorithm=RslRlPpoAlgorithmCfg(
            value_loss_coef=1.0,
            use_clipped_value_loss=True,
            clip_param=0.2,
            entropy_coef=0.01,
            num_learning_epochs=6,
            num_mini_batches=2,
            learning_rate=1.0e-3,
            schedule="adaptive",
            optimizer="adamw",
            gamma=0.99,
            lam=0.95,
            desired_kl=0.01,
            max_grad_norm=1.0,
        ),
        experiment_name="cartpole",
        save_interval=100,
        num_steps_per_env=32,
        max_iterations=500,
    )
