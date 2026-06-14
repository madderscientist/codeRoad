import requests
import random
import re
import json

headers = {
    "accept": "*/*",
    "accept-language": "zh-CN,zh;q=0.9,en;q=0.8,en-GB;q=0.7,en-US;q=0.6",
    "sec-ch-ua": '"Microsoft Edge";v="149", "Chromium";v="149", "Not)A;Brand";v="24"',
    "sec-ch-ua-mobile": "?0",
    "sec-ch-ua-platform": '"Windows"',
    "sec-fetch-dest": "script",
    "sec-fetch-mode": "no-cors",
    "sec-fetch-site": "same-site",
    "referer": "https://w.seu.edu.cn/"
}

baseurl = "https://w.seu.edu.cn:802/eportal/"
jsVersion = "3.3.3"

#%% 登录
def login(account: str, password: str, ipv4: str, ipv6: str = "", mac: str = "000000000000", mode: int = 1):
    """登录校园网"""
    pref = {
        0: "",  # 什么前缀也不加
        1: ",0,",   # PC端无感知记录MAC	登录成功后记录MAC，下次自动登录
        2: ",1,",   # 手机端无感知记录MAC	同上，但针对手机终端
        3: ",a,",   # PC端不记录MAC	登录成功但不记住MAC
        4: ",b,",   # 手机端不记录MAC	同上，但针对手机终端
    }

    callback_name = "dr1004"
    params = {
        "c": "Portal",
        "a": "login",
        "callback": callback_name,
        "login_method": "1",
        "user_account": pref.get(mode, "") + account,
        "user_password": password,
        "wlan_user_ip": ipv4,
        "wlan_user_ipv6": ipv6,
        "wlan_user_mac": mac,
        "wlan_ac_ip": "",
        "wlan_ac_name": "",
        "jsVersion": jsVersion,
        "v": random.randint(500, 10500)
    }
    try:
        response = requests.get(
            baseurl,
            params=params,
            headers=headers,
            timeout=10,
            verify=True
        )
        print(f"[login] status code: {response.status_code}")
        # dr1004({"result":"1","msg":"\u8ba4\u8bc1\u6210\u529f"})
        if response.status_code == 200:
            match = re.search(rf'{callback_name}\((.*)\)', response.text)
            if match:
                result = json.loads(match.group(1))
                return result
        return None
    except requests.exceptions.RequestException as e:
        print(f"[login] request failed: {e}")
        return None

#%% 下面的函数在登录后使用
def find_mac(account: str, ipv4: str) -> str | None:
    """根据IPv4地址查找MAC地址 使用校园网提供的接口"""
    callback_name = "dr1003"
    params = {
        "c": "Portal",
        "a": "find_mac",
        "callback": callback_name,
        "user_account": account,
        "login_method": "1",
        "find_mac": "0",
        "wlan_user_ip": ipv4,
        "jsVersion": jsVersion,
        "v": random.randint(500, 10500)
    }
    try:
        response = requests.get(
            baseurl,
            params=params,
            headers=headers,
            timeout=10,
            verify=True
        )
        """
        dr1003({
            "result": "1",
            "msg": "\u6210\u529f",
            "list": [{
                "online_session": 18556,
                "online_time": "2026-06-13 14:00:20",
                "online_ip": "10.203.156.125",
                "online_mac": "28a44a31f3df",
                "time_long": "496",
                "uplink_bytes": "0",
                "downlink_bytes": "1222",
                "dhcp_host": "",
                "device_alias": "",
                "nas_ip": "352255754",
                "user_account": account,
                "is_owner_ip": "1"
            }]
        })
        """
        
        print(f"[find_mac] status code: {response.status_code}")

        if response.status_code == 200:
            match = re.search(rf'{callback_name}\((.*)\)', response.text)
            if match:
                result = json.loads(match.group(1))
                if result.get("result") == "1":
                    online_mac = result.get("list", [{}])[0].get("online_mac")
                    if online_mac:
                        upper_mac = online_mac.upper()
                        print(f"[find_mac] result: {upper_mac}")
                        return upper_mac
                else:
                    print(f"[find_mac] msg: {result.get('msg')}")
        return None

    except requests.exceptions.RequestException as e:
        print(f"[find_mac] request failed: {e}")
        return None

def unbind_mac(mac: str, ipv4: str) -> dict | None:
    """发送解绑 MAC 请求
    Args:
        mac: 要解绑的MAC地址，形如 "11A22A33A4BC"
        ipv4: 设备当前的IPv4地址
    """
    callback_name = "dr1004"
    
    params = {
        "c": "Portal",
        "a": "unbind_mac",
        "callback": callback_name,
        "user_account": "",
        "wlan_user_mac": mac.replace("-", "").replace(":", ""),  # 移除分隔符
        "wlan_user_ip": ipv4,
        "jsVersion": jsVersion,
        "v": random.randint(500, 10500)
    }
    
    try:
        response = requests.get(
            baseurl,
            params=params,
            headers=headers,
            timeout=10,
            verify=True
        )
        
        print(f"[unbind_mac] status code: {response.status_code}")

        # dr1004({"result":"1","msg":"\u89e3\u7ed1Mac\u6210\u529f"})
        if response.status_code == 200:
            match = re.search(rf'{callback_name}\((.*)\)', response.text)
            if match:
                result = json.loads(match.group(1))
                return result
        
        return None
        
    except requests.exceptions.RequestException as e:
        print(f"[unbind_mac] request failed: {e}")
        return None