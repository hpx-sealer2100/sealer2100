#!/bin/bash

# 检查输入参数
if [ "$#" -ne 2 ]; then
    echo "检查两个提交之间的res目录中修改、增加的问题键"
    echo "Usage: $0 <commitA> <commitB>"
    echo "Usage: $0 <tag1> <tag2>"
    exit 1
fi

# 获取参数
commitA=$1
commitB=$2

# 判断输入是否为哈希值（40位或较短的哈希值）
is_hash() {
    # 使用 git rev-parse 验证是否是有效的哈希值
    if git rev-parse --verify --quiet "$1" >/dev/null 2>&1; then
        # 检查是否是纯哈希（40位或更短）
        if [[ "$1" =~ ^[0-9a-f]{7,40}$ ]]; then
            return 0  # 是哈希
        fi
    fi
    return 1  # 不是哈希
}
# 截取短哈希值（如果是哈希）
if is_hash "$commitA"; then
    short_commitA=$(echo $commitA | cut -c1-7)
else
    short_commitA=$commitA
fi

if is_hash "$commitB"; then
    short_commitB=$(echo $commitB | cut -c1-7)
else
    short_commitB=$commitB
fi

# 设置默认输入目录
input_directory="core/src/trezor/res"

# 设置默认输出目录
output_directory="res-${short_commitA}-${short_commitB}"

# 检查输出目录是否存在，不存在则创建
if [ -d "$output_directory" ]; then
    rm -rf "$output_directory"
fi
mkdir -p "$output_directory"

# 获取从 commitA 到 commitB 增加或修改的文件
changed_files=$(git diff --name-status $commitA $commitB -- $input_directory | grep -E '^(A|M)' | awk '{print $2}')

# 检查是否有匹配的文件
if [ -z "$changed_files" ]; then
    echo "No files were added or modified in the specified range."
    exit 0
fi

# 遍历文件并复制到输出目录
echo "Copying added/modified files to $output_directory:"
for file in $changed_files; do
    # 检查文件是否存在（防止因历史提交删除导致文件不存在）
    if [ -f "$file" ]; then
        echo "Copying $file"
        # 移除 core/src/trezor/res 前缀
        relative_path="${file#$input_directory/}"
        # 创建目标目录（如果不存在）
        mkdir -p "$output_directory/$(dirname "$relative_path")"
        # 复制文件到目标目录
        cp "$file" "$output_directory/$relative_path"
        echo "Copied $file to $output_directory/$relative_path"
    else
        echo "Warning: $file does not exist in the current working tree. Skipping."
    fi
done

echo "Done."
