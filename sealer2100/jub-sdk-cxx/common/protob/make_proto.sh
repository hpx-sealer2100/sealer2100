#!/bin/bash

# 当前脚本所在目录
CUR_DIR="$(cd "$(dirname "$0")" && pwd)"
# 上一级目录下的protocpp目录
OUT_DIR="${CUR_DIR}/../protocpp"

OS_TYPE=$(uname)

if [ "$OS_TYPE" = "Darwin" ]; then
    export PATH="${CUR_DIR}/../../tools/protoc-3.9.1-osx-x86_64/bin:$PATH"
elif [ "$OS_TYPE" = "Linux" ]; then
    export PATH="${CUR_DIR}/../..//tools/protoc-3.9.1-linux-x86_64/bin:$PATH"
else
    echo "未知操作系统: $OS_TYPE"
fi
protoc --version

# 创建输出目录（如果不存在）
mkdir -p "${OUT_DIR}"

# 查找当前目录下所有proto文件并生成C++代码
for proto in "${CUR_DIR}"/*.proto; do
    if [ -f "$proto" ]; then
        protoc --cpp_out="${OUT_DIR}" --proto_path="${CUR_DIR}" "$proto"
    fi
done