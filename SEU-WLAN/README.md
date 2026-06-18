# 自动连接SEU校园网
核心参看 [SEU_WLAN/README.md](SEU_WLAN/README.md)

本层文件即开即用。对于windows系统，直接用管理员权限运行 `reg_shed.ps1` 即可；对于linux系统，需要手动将任务添加到 `crontab` 中，相关说明在 [`crontab_config_linux`](./crontab_config_linux)中

机制为：
1. 开机1分钟后检查连接情况，如果没连上就每隔30重试
2. 每天凌晨4点重置登录状态（登出-登入）

要写一个 `.env` 文件！内容如下：
```sh
ACCOUNT=一卡通
PASSWORD=密码
```

额外的依赖为 `dotenv`