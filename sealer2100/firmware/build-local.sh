set -e
if [ "$1" = "clean" ]; then
    poetry run make -C core clean
fi

poetry run make -C core build_boardloader
poetry run make -C core build_bootloader
export BITCOIN_ONLY=0
export PYOPT=0
rm -f ./core/build/firmware/firmware.elf
rm -f ./core/build/firmware/firmware.bin
rm -f ./core/build/firmware/firmware.p1.bin
rm -f ./core/build/firmware/firmware.p2.bin
poetry run make -C core build_firmware

if [ "$1" = "flash" ]; then
    sh ./flash.sh
fi
