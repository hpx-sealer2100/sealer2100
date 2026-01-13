#/usr/bin/env sh

# echo command executing

BOARDLOADER_START=0x08000000
BOOTLOADER_START=0x08020000
FIRMWARE_P1_START=0x08060000
FIRMWARE_P2_START=0x90000000
CUR_DIR=$(cd `dirname $0`; pwd)
boardloader_file="${CUR_DIR}/core/build/boardloader/boardloader.bin"
bootloader_file="${CUR_DIR}/core/build/bootloader/bootloader.bin"
firmware_file_p1="${CUR_DIR}/core/build/firmware/firmware.p1.bin"
firmware_file_p2="${CUR_DIR}/core/build/firmware/firmware.p2.bin"

# split STM32PROG_DIR variable in .env.sh, .env.sh is a local file not in version control
. ./.env.sh

EXT_LOADER=$STM32PROG_DIR/ExternalLoader/GD25Qxx-DunAn-8MB.stldr
echo "hard reset device ..."
$STM32PROG_DIR/STM32_Programmer_CLI -c port=swd -hardRst

# flash boardloader
echo "Flashing boardloader..."
$STM32PROG_DIR/STM32_Programmer_CLI -c port=swd -w $boardloader_file $BOARDLOADER_START

# flash bootloader
echo "Flashing bootloader..."
$STM32PROG_DIR/STM32_Programmer_CLI -c port=swd -w $bootloader_file $BOOTLOADER_START

# flash firmware_file_p1
echo "Flashing firmware part 1..."
# st-flash --reset write $firmware_file_p1 $FIRMWARE_P1_START
$STM32PROG_DIR/STM32_Programmer_CLI -c port=swd -w $firmware_file_p1 $FIRMWARE_P1_START

# use STM32CubeProgrammer to flash firmware_file_p2
echo "Flashing firmware part 2..."
$STM32PROG_DIR/STM32_Programmer_CLI -c port=swd -el $EXT_LOADER -w $firmware_file_p2 $FIRMWARE_P2_START

echo "hard reset device ..."
$STM32PROG_DIR/STM32_Programmer_CLI -c port=swd -hardRst
