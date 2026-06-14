from time import sleep
from SEU_WLAN import logout
from onboot import loop_login, log_message, ACCOUNT

if __name__ == "__main__":
    log_file = "seu_login_reset_daily.log"
    log_message("尝试登出...", log_file=log_file)
    logout(ACCOUNT)
    sleep(10)
    loop_login(log_file=log_file)