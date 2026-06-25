#!/bin/bash

# 输入目录，默认为当前目录
input_dir=${1:-.}

# 遍历所有 PNG 文件
for file in "$input_dir"/*.png; do
    if [ -f "$file" ]; then
        # 获取文件名
        filename=$(basename "$file")

        # 调整大小并保存到输出目录
        echo "Resizing $filename ..."
        echo "Thumbnail: ../thumbnail/$filename"
        magick "$file" -resize 108x108 "../thumbnail/$filename"
        echo "Image: ../image/$filename"
        magick "$file" -resize 432x432 "../image/$filename"
        echo "Wallpaper: ../wallpaper/$filename"
        magick "$file" -resize 480x480 -gravity center -background black -extent 480x800 "../wallpaper/$filename"
    fi
done

echo "All images have been resized "

