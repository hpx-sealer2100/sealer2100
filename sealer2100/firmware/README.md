# 环境搭建

1. 安装 arm-none-eabi-gcc  
``` sh 
# macOS
# step 0:
#   查看是否安装过 arm-none-eabi-xxx
#   如果安装过，需要卸载掉
brew search arm-none-eabi
brew uninstall arm-none-eabi-gcc arm-none-eabi-gdb

# step 1: 安装 arm-none-eabi-gcc
brew install --cask gcc-arm-embedded
```

2. 安装rust
``` sh
# macOS
brew install rustup-init
rustup-init
rustup default stable
rustup install nightly
```

2. 安装python & pyenv & potrey
``` sh
# macOS
brew install python
brew install pyenv
brew install poetry

# 添加环境变量到shell
cat <<'EOF' >> ~/.zprofile
eval "$(/opt/homebrew/bin/brew shellenv)"
export CPATH="$HOMEBREW_PREFIX/include:$CPATH"
export LIBRARY_PATH="$HOMEBREW_PREFIX/lib:$LIBRARY_PATH"

# 设置 pyenv 的根目录
export PYENV_ROOT="$HOME/.pyenv"

# 将 pyenv 的 bin 目录添加到 PATH
export PATH="$PYENV_ROOT/bin:$PATH"

# 初始化 pyenv（非交互式 shell）
eval "$(pyenv init --path)"
EOF
```

3. 安装 protobuf

``` sh
# macOS
brew install protobuf
```

4. 安装 [stm32cubeide](https://www.st.com/en/development-tools/stm32cubeide.html) 与 [stm32cubeprogrammer](https://www.st.com/en/development-tools/stm32cubeprog.html)  

``` sh
# 复制 `core/tools/GD25Qxx-DunAn-8MB.stldr` 到 stm32cubeprogrammer 安装目录的 ExternalLoader 文件夹中
# macOS
sudo cp core/tools/GD25Qxx-DunAn-8MB.stldr /Applications/STMicroelectronics/STM32Cube/STM32CubeProgrammer/STM32CubeProgrammer.app/Contents/Resources/bin/ExternalLoader

# 在项目根目录创建.env.sh 内容为
cat <<'EOF' >> .env.sh
STM32PROG_DIR=/Applications/STMicroelectronics/STM32Cube/STM32CubeProgrammer/STM32CubeProgrammer.app/Contents/Resources/bin
EOF
```

5. 安装git-lfs  
``` sh
# macOS
brew install git-lfs

# ubuntu
``` sh
sudo apt install git-lfs
```

# 项目环境初始化

1. 项目环境初始化
``` sh
# 首次安装依赖时，需要重启终端
# 使用pyenv 设置项目使用的 python 版本
pyenv local 3.10
pyenv install
# 设置poetry 使用的python
# rm -rf .venv # 如果之前进行过 poetry install
poetry env use $(pyenv which python)
poetry install
```

2. 安装项目依赖
``` sh
# 子模块初始化
git submodule update --init --recursivecd
# 同步资源文件
git lfs install
git lfs pull
```

# 编译项目
## 真机固件
``` sh
# 使用脚本编译
sh build-local.sh
```

## 模拟器固件
**注意：模拟器在nix-shell下会崩溃，需要在macOS原生系统上运行**


由于mac本逻辑分辨率比较低（尽管mac本的实际分辨率很高），在代码中对模拟器的大小做了缩放，导致在模拟器上看着比较模糊。如果有外接的大的显示器，可以通过修改[display-unix.c](core/embed/extmod/modtrezorui/display-unix.c#L44)代码中定义的缩放比例来调整模拟器大小。修改缩放比例之后需要重新编译模拟器。

```c
#define WINDOW_SCALE 0.6
```

``` sh
# 安装依赖
brew install sdl2 sdl2_image
# 编译模拟器
poetry run make -C core build_unix
# 运行模拟器， 只修改了python文件，不需要重新编译模拟器
poetry run core/emu.py

# 更新代码后,需要关掉模拟器重新启动，也可以运行下面命令
# poetry run core/emu.py -w # 在修改过文件之后自动重启模拟器
```

# 代码烧录

```
sh ./flash.sh
```

# 拷贝资源文件
- 第一次烧录后，硬件上电进入 boardloader，会在电脑系统上挂载两个 U 盘，把 core/src/trezor/res 文件夹下除 res/nfts 文件拷贝到 SYSTEM 盘符下。
- 后续测试时，在 poetry shell 环境下使用如下指令回到 boardloader，同样会在系统上挂载两个 U 盘，把相应的资源拷贝到相应位置,res文件夹为隐藏属性，打开文件浏览器的查看隐藏功能。
```
trezorctl device reboot-to-boardloader
```

- 真正上线时，禁止回boardloader指令，使用update-res指令进行升级资源

# 固件升级指令
``` sh
trezorctl device reboot-to-bootloader
trezorctl device emmc-dir-make -p 0:updates
trezorctl device emmc-file-write -l ./core/build/firmware/firmware.bin -r 0:updates/firmware.bin -f -cs 16384
trezorctl device firmware-update-emmc -p 0:updates/firmware.bin
trezorctl device bl-reboot
```

# 升级bootloader指令
``` sh
trezorctl device reboot-to-bootloader
trezorctl device emmc-dir-make -p 0:boot
trezorctl device emmc-file-write -l ./core/build/bootloader/bootloader.bin -r 0:boot/bootloader.bin -f -cs 16384
trezorctl device bl-reboot
```

# 蓝牙固件升级指令
``` sh
trezorctl device reboot-to-bootloader
trezorctl device emmc-dir-make -p 0:updates
trezorctl device emmc-file-write -l ota.bin -r 0:updates/ota.bin -f -cs 16384
trezorctl device firmware-update-emmc -p 0:updates/ota.bin
trezorctl device bl-reboot
```

# 生产固件签名密钥生成方法
首先公司中选择三个重要的管理人员并分配“index”，1，2，3
运行
```
poetry run python ./core/tools/gen_keys.py
```
脚本会生成一对“固件签名公私钥”和一对“蓝牙签名公私钥”  
其中“固件签名公私钥”和自己的“index”三个管理人员都需要进行安全的保存，后续发布固件时进行使用。
其中“蓝牙签名公私钥”只需要一个用来发布蓝牙固件的管理人员安全的保存存，后续发布蓝牙时进行使用。

# 生成protobuf
poetry run make protobuf

# 模拟器

```sh
```

``` sh
# 编译模拟器
poetry run make -C core build_unix
# 运行模拟器， 只修改了python文件，不需要重新编译模拟器
poetry run core/emu.py

# 更新代码后,需要关掉模拟器重新启动，也可以运行下面命令
# poetry run core/emu.py -w # 在修改过文件之后自动重启模拟器
```