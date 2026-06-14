from dotenv import load_dotenv
import os
from datetime import datetime
from SEU_WLAN import check_login, login

load_dotenv()

def get_env_var(name: str) -> str:
    """获取环境变量，如果不存在则抛出异常"""
    value = os.getenv(name)
    if value is None:
        raise ValueError(f"请在 .env 文件中设置 {name}")
    return value

ACCOUNT = get_env_var("ACCOUNT")
PASSWORD = get_env_var("PASSWORD")

CHECK_INTERVAL = 30  # 重试间隔 sec
# 日志文件路径，None表示不记录文件
LOG_FILE = "seu_login_onboot.log"

def log_message(msg, log_file=LOG_FILE):
    """打印并记录日志"""
    timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
    log_str = f"[{timestamp}] {msg}"
    print(log_str)
    
    if log_file:
        try:
            with open(log_file, 'a', encoding='utf-8') as f:
                f.write(log_str + '\n')
        except Exception as e:
            print(f"[WARN] 写入日志失败: {e}")

def loop_login(log_file=LOG_FILE):
    import sys
    import time
    retry_count = 0
    while True:
        if check_login(ACCOUNT):
            sys.exit(0)
        log_message(f"未连接校园网，尝试登录... (第 {retry_count + 1} 次)", log_file=log_file)
        result = login(ACCOUNT, PASSWORD)
        if result is not None and result.get("result") == "1":
            log_message("登录成功！", log_file=log_file)
            sys.exit(0)
        else:
            log_message(f"登录失败：{result}，{CHECK_INTERVAL}秒后重试", log_file=log_file)
        retry_count += 1
        time.sleep(CHECK_INTERVAL)

if __name__ == "__main__":
    loop_login()