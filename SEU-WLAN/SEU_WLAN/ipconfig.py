# windows 下使用
import subprocess
import re

def parse_ipconfig():
    result = subprocess.run(['ipconfig', '/all'], capture_output=True, text=True)
    adapters = {}

    # with open("ipconfig_output.txt", "w", encoding="utf-8") as f:
    #     f.write(result.stdout)

    # 以换行符开头后跟非空字符为分隔
    blocks = re.split(r'\n(?=\S)', result.stdout)

    for block in blocks:
        lines = block.strip().split('\n')
        if not lines:
            continue

        # 提取适配器名称
        adapter_name = lines[0].strip()
        if len(adapter_name) == 0:
            continue
        if adapter_name.endswith(':'):
            adapter_name = adapter_name[:-1]

        # 解析每个适配器的键值对
        info = {}
        current_key = None
        for line in lines[1:]:
            line = line.strip()
            if len(line) == 0:
                continue
            # 匹配格式：最左边的冒号最为分隔
            linesplit = line.split(':', 1)

            if len(linesplit) != 2:
                if current_key:
                    # 处理值跨行的情况
                    info_value = info[current_key]
                    if type(info_value) == list:
                        info[current_key].append(line)
                    else:
                        info[current_key] = [info_value, line]
                continue

            key = linesplit[0].replace('. ', '').strip()
            value = linesplit[1].strip()
            info[key] = value
            current_key = key

        adapters[adapter_name] = info

    return adapters

if __name__ == "__main__":
    adapters = parse_ipconfig()
    import json
    print(json.dumps(adapters, indent=4, ensure_ascii=False))