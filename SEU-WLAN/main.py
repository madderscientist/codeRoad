import seuwlan
from ipconfig import parse_ipconfig

def get_ipv4_address():
    """获取本机IPv4地址"""
    try:
        import socket
        s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        s.connect(("8.8.8.8", 80))
        ipv4 = s.getsockname()[0]
        s.close()
        return ipv4
    except Exception:
        return "127.0.0.1"

def get_mac_address():
    """获取本机MAC地址
    注意uuid和校园网获取到的不一样
    """
    import uuid
    return ''.join(f'{uuid.getnode():012x}'[i:i+2] for i in range(0,12,2)).upper()

IPCONFIG_SUF = "(首选)"
def get_net_info():
    ipconfig_output = parse_ipconfig()
    ipv4 = None
    mac = None
    ipv6 = ""
    for key, value in ipconfig_output.items():
        if not key.endswith("WLAN"):
            continue
        for k, v in value.items():
            if ipv4 is None and k.lower().startswith("ipv4"):
                ipv4 = v.replace(IPCONFIG_SUF, "")
            elif "物理地址" in k:
                mac = v
            elif k.lower().startswith("ipv6"):
                ipv6 = v.replace(IPCONFIG_SUF, "")
    if not ipv4:
        ipv4 = get_ipv4_address()
    if not mac:
        mac = get_mac_address()

    return ipv4, mac, ipv6

def login(account: str, password: str):
    ipv4, mac, ipv6 = get_net_info()
    return seuwlan.login(account, password, ipv4, ipv6, mac)

def logout(account: str):
    ipv4, mac, ipv6 = get_net_info()
    mac_ = seuwlan.find_mac(account, ipv4)
    if mac_ != None:
        mac = mac_
    return seuwlan.unbind_mac(mac, ipv4)


if __name__ == "__main__":
    """
    # 登录（需要 -a 和 -p）
    python main.py login -a 202411001 -p 123456

    # 登出（只需要 -a）
    python main.py logout -a 202411001

    # 查看帮助
    python main.py -h
    python main.py login -h
    python main.py logout -h
    """
    import argparse
    import sys
    parser = argparse.ArgumentParser(description='SEU校园网工具')
    subparsers = parser.add_subparsers(dest='command', help='可用命令')

    parser_login = subparsers.add_parser('login', help='登录校园网')
    parser_login.add_argument('-a', '--account', required=True, help='账号')
    parser_login.add_argument('-p', '--password', required=True, help='密码')

    parser_logout = subparsers.add_parser('logout', help='登出校园网')
    parser_logout.add_argument('-a', '--account', required=True, help='账号')
    
    args = parser.parse_args()
    if args.command == 'login':
        result = login(args.account, args.password)
        if result is not None:
            print(result)
            if result["result"] == "1":
                sys.exit(0)
    elif args.command == 'logout':
        result = logout(args.account)
        if result is not None:
            print(result)
            if result["result"] == "1":
                sys.exit(0)
    else:
        parser.print_help()
    sys.exit(1)