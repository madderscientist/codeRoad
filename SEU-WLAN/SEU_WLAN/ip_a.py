# linux 下使用
import subprocess
import re

def parse_ip_a():
    result = subprocess.run(['ip', 'a'], capture_output=True, text=True)
    adapters = {}

    # 1. 按块分割：每个网络接口以 "数字: 接口名:" 开头
    # 正则含义：匹配行首，后跟 数字、冒号、空格、非空字符
    blocks = re.split(r'\n(?=\d+: \S)', result.stdout)

    for block in blocks:
        lines = block.strip().split('\n')
        if not lines:
            continue

        # 2. 提取适配器名称
        # 第一行格式示例: "2: eth0: <BROADCAST,MULTICAST,UP,LOWER_UP> ..."
        header = lines[0]
        match = re.match(r'\d+:\s+(\S+?):', header)
        if not match:
            continue
        adapter_name = match.group(1)

        info = {
            'header': header,
            'mac_address': None,
            'ipv4': [],
            'ipv6': []
        }

        # 3. 遍历后续行，提取 MAC 和 IP 地址
        for line in lines[1:]:
            stripped_line = line.strip()
            
            # 提取 MAC 地址 (link/ether 或 link/loopback)
            if stripped_line.startswith('link/'):
                mac_match = re.search(r'link/\S+\s+([0-9a-fA-F:]{17})', stripped_line)
                if mac_match:
                    info['mac_address'] = mac_match.group(1)

            # 提取 IPv4 地址
            elif stripped_line.startswith('inet '):
                ipv4_match = re.search(r'inet\s+(\S+)', stripped_line)
                if ipv4_match:
                    info['ipv4'].append(ipv4_match.group(1))

            # 提取 IPv6 地址
            elif stripped_line.startswith('inet6 '):
                ipv6_match = re.search(r'inet6\s+(\S+)', stripped_line)
                if ipv6_match:
                    info['ipv6'].append(ipv6_match.group(1))

        adapters[adapter_name] = info

    return adapters

if __name__ == "__main__":
    adapters = parse_ip_a()
    import json
    print(json.dumps(adapters, indent=4, ensure_ascii=False))