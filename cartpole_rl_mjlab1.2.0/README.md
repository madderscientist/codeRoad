# mjlab 1.2.0 cartpole
写文与2026年4月6日。此时mjlab最新的release是1.2.0，但是cartpole tutorial仅适用于nightly version。于是进行了一次适配。

坑（以下这些版本差异都已经体现在changelog中了）：
- nightly改了挺多地方，比如`XmlMotorActuatorCfg`变为了`XmlActuatorCfg`
- 老版本`limited="false"`会导致`range`为`[0,0]`，nightly已经修复为`[-inf,inf]`了。而`reset_joints_by_offset`中会用`range`对位置进行`clamp`，所以1.2.0下需要写一个没有clamp的版本。复现这个问题的代码：
    ```python
    import mjlab.tasks
    import cartpole_task
    from mjlab.tasks.registry import load_env_cfg
    from mjlab.envs import ManagerBasedRlEnv

    cfg = load_env_cfg('Cartpole-Balance', play=True)
    cfg.scene.num_envs = 1
    env = ManagerBasedRlEnv(cfg=cfg, device='cpu')
    env.reset()
    asset = env.scene['cartpole']
    print('joint_names:', list(asset.joint_names))
    print('soft_joint_pos_limits_row0:', asset.data.soft_joint_pos_limits[0].tolist())
    env.close()
    ```
- 1.2.0的`MujocoCfg`没有`disableflags`参数，于是尝试在xml里用`<flag contact="disable" />`实现。但是疑似没有效果，还是需要手动设置具体`geom`的不碰撞。下面是得到这个结论的经历：
    1. 1.2.0训练效果极差
    2. 用了nightly的版本，官方代码训练效果不催，而这个版本下我的代码还是很差（两份代码的差异仅为`disableflags`参数）
    3. 试着将我的xml的轨道删除，效果突然和官方一样
    4. 发现轨道的存在会让滑块运动受阻，似乎xml的`<flag contact="disable" />`没有作用。但是通过代码检查发现`bool(model.opt.disableflags & mujoco.mjtDisableBit.mjDSBL_CONTACT)===True`。
    5. 提了个[issue](https://github.com/mujocolab/mjlab/issues/885)，总结：不是bug。因为在 mjlab 里，任务通常是把实体模型拼到父场景里。拼接用的是 `MjSpec.attach`，会忽略子XML的场景设置。

## Usage
```bash
python train.py Cartpole-Swingup --env.scene.num-envs 4096
python play.py Cartpole-Swingup --checkpoint_file logs/rsl_rl/cartpole/2026-04-07_00-10-55/model_499.pt
```

[train.py](train.py)和[play.py](play.py)都是直接复制的，并在`main`中`import cartpole_task`以注册新任务