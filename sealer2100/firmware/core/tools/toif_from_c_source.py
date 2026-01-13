#!/usr/bin/env python3

import re
import ast
import argparse
import os
import sys

def list_icon_arrays(c_code: str):
    """
    从 C 文件中列出所有 static const uint8_t xxx[] 数组名
    """
    pattern = r'static\s+const\s+uint8_t\s+(\w+)\s*\[\s*\]\s*=\s*{'
    return re.findall(pattern, c_code)

def parse_c_array(c_code: str):
    """
    解析 C 语言中定义的 uint8_t 数组，返回字节列表。
    """
    match = re.search(r'{(.*?)}', c_code, re.DOTALL)
    if not match:
        raise ValueError("No array initializer found")

    array_content = match.group(1)

    # 去掉注释和多余空白
    lines = array_content.splitlines()
    cleaned_lines = []
    for line in lines:
        line = line.split('//')[0]  # 去掉行内注释
        cleaned_lines.append(line.strip())

    cleaned_data = ','.join(cleaned_lines)

    # 分割元素
    elements = [e.strip() for e in cleaned_data.split(',') if e.strip()]
    byte_array = bytearray()

    for elem in elements:
        if elem.startswith("'") and elem.endswith("'"):
            # 字符字面量
            char = ast.literal_eval(elem)  # 支持转义字符
            byte_array.append(ord(char))
        elif elem.startswith("0x") or elem.isdigit():
            byte_array.append(int(elem, 0))
        else:
            raise ValueError(f"Unrecognized element: {elem}")

    return byte_array

def extract_array_code(c_code: str, array_name: str):
    """
    提取某个数组的完整定义代码
    """
    pattern = rf'static\s+const\s+uint8_t\s+{array_name}\s*\[\s*\]\s*=\s*{{.*?}};'
    match = re.search(pattern, c_code, re.DOTALL)
    if not match:
        raise ValueError(f"Array '{array_name}' not found in file.")
    return match.group(0)

def extract_and_write_to_file(c_code: str, array_name: str):
    """
    提取并写入单个 icon
    """
    try:
        array_code = extract_array_code(c_code, array_name)
        byte_array = parse_c_array(array_code)
        output_file = f"{array_name}.toif"
        with open(output_file, 'wb') as f:
            f.write(byte_array)
        print(f"📤 Extracting '{array_name}' to '{output_file}'...")
        print(f"✅ Written {len(byte_array)} bytes to '{output_file}'")
    except Exception as e:
        print(f"❌ Failed to extract '{array_name}': {e}")

def main():
    parser = argparse.ArgumentParser(
        description="Extract uint8_t icon arrays from a C file and write them as binary files."
    )
    parser.add_argument("c_file", help="Path to the C file")
    parser.add_argument("icons", nargs="*", help="Names of icons to extract")
    parser.add_argument("--all", action="store_true", help="Extract all icons")

    args = parser.parse_args()

    if not os.path.isfile(args.c_file):
        print(f"❌ File not found: {args.c_file}")
        sys.exit(1)

    with open(args.c_file, 'r', encoding='utf-8') as f:
        c_code = f.read()

    all_icons = list_icon_arrays(c_code)

    if not args.icons and not args.all:
        # 只列出 icon 名字
        print("📦 Found icon arrays:")
        for name in all_icons:
            print(f"  - {name}")
        return

    if args.all:
        icons_to_extract = all_icons
    else:
        icons_to_extract = args.icons

    for icon in icons_to_extract:
        if icon not in all_icons:
            print(f"⚠️  Icon '{icon}' not found in file. Skipping.")
            continue
        extract_and_write_to_file(c_code, icon)

if __name__ == "__main__":
    main()
