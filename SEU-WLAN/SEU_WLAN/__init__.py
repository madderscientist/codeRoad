import platform
import re
from . import seuwlan

def __get_ipv4_address():
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

def __get_mac_address():
    """获取本机MAC地址
    注意uuid和校园网获取到的不一样
    """
    import uuid
    return f'{uuid.getnode():012x}'.upper()

def get_net_info():
    system = platform.system()
    if system == "Windows":
        v4, mac, v6 = get_net_info_win()
    elif system == "Linux":
        v4, mac, v6 = get_net_info_linux()
    else:
        raise NotImplementedError("Unsupported operating system")

    if not v4:
        v4 = __get_ipv4_address()
    if not mac:
        mac = __get_mac_address()
    return v4, mac, v6

def get_net_info_linux():
    from .ip_a import parse_ip_a
    ip_a_output = parse_ip_a()
    ipv4 = None
    mac = None
    ipv6 = ""
    for key, value in ip_a_output.items():
        if not key.endswith("wlan0"):
            continue
        for k, v in value.items():
            if ipv4 is None and k == "ipv4" and len(v) > 0:
                ipv4 = v[0].split('/')[0]
            elif mac is None and k == "mac_address":
                mac = v.replace(':', '').upper()
            elif k == "ipv6" and len(v) > 0:
                ipv6 = v[0].split('/')[0]

    return ipv4, mac, ipv6

def get_net_info_win():
    from .ipconfig import parse_ipconfig
    ipconfig_output = parse_ipconfig()
    ipv4 = None
    mac = None
    ipv6 = ""
    brace_pattern = r'\([^)]*\)$'
    for key, value in ipconfig_output.items():
        if not key.endswith("WLAN"):
            continue
        for k, v in value.items():
            if ipv4 is None and k.lower().startswith("ipv4"):
                ipv4 = re.sub(brace_pattern, "", v)
            elif "物理地址" in k:
                mac = v.replace('-', '')
            elif k.lower().startswith("ipv6"):
                ipv6 = re.sub(brace_pattern, "", v)

    return ipv4, mac, ipv6

def login(account: str, password: str):
    ipv4, mac, ipv6 = get_net_info()
    return seuwlan.login(account, password, ipv4)

def logout(account: str):
    ipv4, mac, ipv6 = get_net_info()
    mac_ = seuwlan.find_mac(account, ipv4)
    if mac_ != None:
        mac = mac_
    return seuwlan.unbind_mac(mac, ipv4)

def check_login(account: str) -> bool:
    ipv4, mac, ipv6 = get_net_info()
    return seuwlan.find_mac(account, ipv4) is not None