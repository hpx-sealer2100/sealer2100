.PHONY: vendor

JOBS = 4
MAKE = make -j $(JOBS)
SCONS = scons -Q -j $(JOBS)

BUILD_DIR             = build
BOOTLOADER_BUILD_DIR  = $(BUILD_DIR)/bootloader
FIRMWARE_BUILD_DIR    = $(BUILD_DIR)/firmware
UNIX_BUILD_DIR        = $(BUILD_DIR)/unix
UPGRADER_BUILD_DIR    = $(BUILD_DIR)/upgrader
RUST_BUILD_DIR        = $(BUILD_DIR)/rust

UNAME_S := $(shell uname -s)
CURRENT_DIR := '\"$(shell pwd)\"'
UNIX_PORT_OPTS ?=
CROSS_PORT_OPTS ?=

PRODUCTION ?= 0
PYOPT      ?= 1
BITCOIN_ONLY ?= 0
TREZOR_MODEL ?= T
TREZOR_MEMPERF ?= 0
ADDRESS_SANITIZER ?= 0
UI2 ?= 0
PRODUCTION_MODEL = H
PCB_VERSION ?= PCB_VERSION_V_1_1

# OpenOCD interface default. Alternative: ftdi/olimex-arm-usb-tiny-h
OPENOCD_INTERFACE ?= stlink
# OpenOCD transport default. Alternative: jtag
OPENOCD_TRANSPORT ?= hla_swd
OPENOCD = openocd -f interface/$(OPENOCD_INTERFACE).cfg -c "transport select $(OPENOCD_TRANSPORT)" -f target/stm32f4x.cfg

BOOTLOADER_START    = 0x08000000
FIRMWARE_P1_START   = 0x08060000
FIRMWARE_P2_START   = 0x90000000

BOOTLOADER_MAXSIZE  = 393216
FIRMWARE_P1_MAXSIZE = 1572864
FIRMWARE_P2_MAXSIZE = 3145728
FIRMWARE_MAXSIZE    = 4718592

CFLAGS += -DSCM_REVISION='\"$(shell git rev-parse HEAD | sed 's:\(..\):\\x\1:g')\"'
BUILD_DATE=$(shell date "+%m%d")

BOOT_BUILD_VERSION=$(shell ../tools/version.sh ./embed/bootloader/version.h)
FIRMWARE_BUILD_VERSION=$(shell ../tools/version.sh ./embed/firmware/version.h)
UPGRADER_BUILD_VERSION=$(shell ../tools/version.sh ./embed/upgrader/version.h)
ifndef BUILD_ID
BUILD_COMMIT=$(shell git rev-parse HEAD | cut -c1-7)
FIRMWARE_BUILD_NAME=touch.$(FIRMWARE_BUILD_VERSION)-Stable-$(BUILD_DATE)-$(BUILD_COMMIT)
BOOT_BUILD_NAME=bootloader.$(BOOT_BUILD_VERSION)-Stable-$(BUILD_DATE)-$(BUILD_COMMIT)
UPGRADER_BUILD_NAME=upgrader.$(UPGRADER_BUILD_VERSION)-Stable-$(BUILD_DATE)-$(BUILD_COMMIT)
CFLAGS += -DBUILD_ID='\"$(BUILD_COMMIT)\"'
else
FIRMWARE_BUILD_NAME=touch.$(FIRMWARE_BUILD_VERSION)-Alpha-$(BUILD_DATE)-$(BUILD_ID)
BOOT_BUILD_NAME=bootloader.$(BOOT_BUILD_VERSION)-Alpha-$(BUILD_DATE)-$(BUILD_ID)
UPGRADER_BUILD_NAME=upgrader.$(UPGRADER_BUILD_VERSION)-Alpha-$(BUILD_DATE)-$(BUILD_ID)
CFLAGS += -DBUILD_ID='\"$(BUILD_ID)\"'
endif


TESTPATH = $(CURDIR)/../tests

EMU = $(CURDIR)/emu.py
EMU_LOG_FILE ?= $(TESTPATH)/trezor.log
EMU_TEST_ARGS = --disable-animation --headless --output=$(EMU_LOG_FILE) --temporary-profile
EMU_TEST = $(EMU) $(EMU_TEST_ARGS) -c

JUNIT_XML ?= $(TESTPATH)/junit.xml
PYTEST = pytest --junitxml=$(JUNIT_XML)
TREZOR_FIDO2_UDP_PORT = 21326
RUST_TARGET=$(shell rustc -vV | sed -n 's/host: //p')

## help commands:

help: ## show this help
	@awk -f ../tools/help.awk $(MAKEFILE_LIST)

## dependencies commands:

vendor: ## update git submodules
	git submodule update --init --recursive --force

## emulator commands:

run: ## run unix port
	cd src ; ../$(UNIX_BUILD_DIR)/trezor-emu-core

emu: ## run emulator
	$(EMU)

## test commands:

test: ## run unit tests
	cd tests ; ./run_tests.sh $(TESTOPTS)

test_rust: ## run rs unit tests
	cd embed/rust ; cargo test  --target=$(RUST_TARGET) --no-default-features --features model_t$(shell echo $(TREZOR_MODEL) | tr "TR" "tr"),test -- --test-threads=1

test_emu: ## run selected device tests from python-trezor
	$(EMU_TEST) $(PYTEST) $(TESTPATH)/device_tests $(TESTOPTS)

test_emu_multicore: ## run device tests using multiple cores
	$(PYTEST) -n auto $(TESTPATH)/device_tests $(TESTOPTS) --control-emulators --model=core --random-order-seed=$(shell echo $$RANDOM)

test_emu_monero: ## run selected monero device tests from monero-agent
	cd tests ; $(EMU_TEST) ./run_tests_device_emu_monero.sh $(TESTOPTS)

test_emu_u2f: ## run selected u2f device tests from u2f-tests-hid
	$(EMU_TEST) --slip0014 $(TESTPATH)/fido_tests/u2f-tests-hid/HIDTest $(TREZOR_FIDO2_UDP_PORT) $(TESTOPTS)
	$(EMU_TEST) --slip0014 $(TESTPATH)/fido_tests/u2f-tests-hid/U2FTest $(TREZOR_FIDO2_UDP_PORT) $(TESTOPTS)

test_emu_fido2: ## run fido2 device tests
	cd $(TESTPATH)/fido_tests/fido2 ; \
		$(EMU_TEST) --slip0014 $(PYTEST) --maxfail=5 --sim tests/standard/ --vendor trezor $(TESTOPTS)

test_emu_click: ## run click tests
	$(EMU_TEST) $(PYTEST) $(TESTPATH)/click_tests $(TESTOPTS)

test_emu_ui: ## run ui integration tests
	UI2="$(UI2)" $(EMU_TEST) $(PYTEST) $(TESTPATH)/device_tests --ui=test --ui-check-missing $(TESTOPTS)

test_emu_ui_multicore: ## run ui integration tests using multiple cores
	UI2="$(UI2)" $(PYTEST) -n auto $(TESTPATH)/device_tests $(TESTOPTS) --ui=test --ui-check-missing --control-emulators --model=core --random-order-seed=$(shell echo $$RANDOM)

test_emu_ui_record: ## record and hash screens for ui integration tests
	UI2="$(UI2)" $(EMU_TEST) $(PYTEST) $(TESTPATH)/device_tests --ui=record --ui-check-missing $(TESTOPTS)

test_emu_ui_record_multicore: ## record and hash screens for ui integration tests using multiple cores
	UI2="$(UI2)" $(PYTEST) -n auto $(TESTPATH)/device_tests $(TESTOPTS) --ui=record --ui-check-missing --control-emulators --model=core --random-order-seed=$(shell echo $$RANDOM)

pylint: ## run pylint on application sources and tests
	pylint -E $(shell find src tests -name *.py)

mypy: ## deprecated; use "make typecheck"
	@echo "mypy is deprecated; use 'make typecheck'"
	make typecheck

typecheck: pyright

pyright:
	python ../tools/pyright_tool.py

clippy:
	cd embed/rust ; cargo clippy --all-features --target=$(RUST_TARGET)

## code generation:

templates: ## render Mako templates (for lists of coins, tokens, etc.)
	./tools/build_templates

templates_check: ## check that Mako-rendered files match their templates
	./tools/build_templates --check

## build commands:

build: build_bootloader build_firmware ## build all

build_embed: build_bootloader build_firmware # build bootloader, firmware

build_bootloader: ## build bootloader
	$(SCONS) CFLAGS="$(CFLAGS)" PRODUCTION="$(PRODUCTION)" TREZOR_MODEL="$(TREZOR_MODEL)" PRODUCTION_MODEL="$(PRODUCTION_MODEL)" $(BOOTLOADER_BUILD_DIR)/bootloader.bin
	cp $(BOOTLOADER_BUILD_DIR)/bootloader.bin $(BOOTLOADER_BUILD_DIR)/$(BOOT_BUILD_NAME).bin

build_firmware: templates build_cross ## build firmware with frozen modules
	$(SCONS) CFLAGS="$(CFLAGS)" PRODUCTION="$(PRODUCTION)" TREZOR_MODEL="$(TREZOR_MODEL)" PRODUCTION_MODEL="$(PRODUCTION_MODEL)" PYOPT="$(PYOPT)" BITCOIN_ONLY="$(BITCOIN_ONLY)" UI2="$(UI2)" $(FIRMWARE_BUILD_DIR)/firmware.bin
	cp $(FIRMWARE_BUILD_DIR)/firmware.bin $(FIRMWARE_BUILD_DIR)/$(FIRMWARE_BUILD_NAME).bin

build_unix: templates ## build unix port
	$(SCONS) CFLAGS="$(CFLAGS)" $(UNIX_BUILD_DIR)/trezor-emu-core $(UNIX_PORT_OPTS) TREZOR_MODEL="$(TREZOR_MODEL)" BITCOIN_ONLY="$(BITCOIN_ONLY)" TREZOR_EMULATOR_ASAN="$(ADDRESS_SANITIZER)" UI2="$(UI2)" UNAME_S="$(UNAME_S)" CURRENT_DIR="$(CURRENT_DIR)"

build_unix_frozen: templates build_cross ## build unix port with frozen modules
	$(SCONS) CFLAGS="$(CFLAGS)" $(UNIX_BUILD_DIR)/trezor-emu-core $(UNIX_PORT_OPTS) TREZOR_MODEL="$(TREZOR_MODEL)" PYOPT="$(PYOPT)" BITCOIN_ONLY="$(BITCOIN_ONLY)" TREZOR_EMULATOR_ASAN="$(ADDRESS_SANITIZER)" UI2="$(UI2)" TREZOR_MEMPERF="$(TREZOR_MEMPERF)" TREZOR_EMULATOR_FROZEN=1

build_unix_debug: templates ## build unix port
	$(SCONS) --max-drift=1 CFLAGS="$(CFLAGS)" $(UNIX_BUILD_DIR)/trezor-emu-core $(UNIX_PORT_OPTS) TREZOR_MODEL="$(TREZOR_MODEL)" BITCOIN_ONLY="$(BITCOIN_ONLY)" TREZOR_EMULATOR_ASAN=1 UI2="$(UI2)" TREZOR_EMULATOR_DEBUGGABLE=1

build_upgrader: ## build upgrader
	$(SCONS) CFLAGS="$(CFLAGS)" PRODUCTION="$(PRODUCTION)" TREZOR_MODEL="$(TREZOR_MODEL)" PRODUCTION_MODEL="$(PRODUCTION_MODEL)" $(UPGRADER_BUILD_DIR)/upgrader.bin
	cp $(UPGRADER_BUILD_DIR)/upgrader.bin $(UPGRADER_BUILD_DIR)/$(UPGRADER_BUILD_NAME).bin

build_upgrader_debug: ## build debugable upgrader, can flash to device
	$(SCONS) UPGRADER_DEBUG="1" CFLAGS="$(CFLAGS)" PRODUCTION="$(PRODUCTION)" TREZOR_MODEL="$(TREZOR_MODEL)" PRODUCTION_MODEL="$(PRODUCTION_MODEL)" $(UPGRADER_BUILD_DIR)/upgrader.bin
	cp $(UPGRADER_BUILD_DIR)/upgrader.bin $(UPGRADER_BUILD_DIR)/$(UPGRADER_BUILD_NAME).bin

build_cross: ## build mpy-cross port
	$(MAKE) -C vendor/micropython/mpy-cross $(CROSS_PORT_OPTS)

## clean commands:

clean: clean_bootloader clean_firmware clean_unix clean_cross clean_rust_airgapped## clean all
	rm -f ".sconsign.dblite"

clean_bootloader: ## clean bootloader build
	rm -rf $(BOOTLOADER_BUILD_DIR)

clean_firmware: ## clean firmware build
	rm -rf $(FIRMWARE_BUILD_DIR) $(RUST_BUILD_DIR)

clean_unix: ## clean unix build
	rm -rf $(UNIX_BUILD_DIR) $(RUST_BUILD_DIR)

clean_upgrader: ## clean upgrader build
	rm -rf $(UPGRADER_BUILD_DIR)

clean_cross: ## clean mpy-cross build
	$(MAKE) -C vendor/micropython/mpy-cross clean $(CROSS_PORT_OPTS)

clean_rust_airgapped:
	cargo clean --manifest-path=vendor/keystone3-firmware/rust/Cargo.toml

## flash commands:

flash: flash_bootloader flash_firmware ## flash everything using OpenOCD

flash_bootloader: $(BOOTLOADER_BUILD_DIR)/bootloader.bin ## flash bootloader using OpenOCD
	$(OPENOCD) -c "init; reset halt; flash write_image erase $< $(BOOTLOADER_START); exit"

flash_firmware: $(FIRMWARE_BUILD_DIR)/firmware.bin ## flash firmware using OpenOCD
	$(OPENOCD) -c "init; reset halt; flash write_image erase $<.p1 $(FIRMWARE_P1_START); flash write_image erase $<.p2 $(FIRMWARE_P2_START); exit"

flash_erase: ## erase all sectors in flash bank 0
	$(OPENOCD) -c "init; reset halt; flash info 0; flash erase_sector 0 0 last; flash erase_check 0; exit"

# flash_erase_storage: ## erase storage sectors from flash
# 	$(OPENOCD) -c "init; flash erase_sector 0 4 4; flash erase_sector 0 16 16; exit"

flash_boardloader_jlink: $(BOARDLOADER_BUILD_DIR)/boardloader.bin ## flash bootloader using JLink
	JLinkExe -nogui 1 -commanderscript embed/debugger_scripts/jlink/write_boardloader.jlink

flash_bootloader_jlink: $(BOOTLOADER_BUILD_DIR)/bootloader.bin ## flash bootloader using JLink
	JLinkExe -nogui 1 -commanderscript embed/debugger_scripts/jlink/write_bootloader.jlink

# flash_bootloader_ci_jlink: $(BOOTLOADER_CI_BUILD_DIR)/bootloader.bin ## flash CI bootloader using JLink
# 	JLinkExe -nogui 1 -commanderscript embed/bootloader_ci/bootloader_flash.jlink

flash_firmware_jlink: $(FIRMWARE_BUILD_DIR)/firmware.bin ## flash firmware using JLink. file names must end in .bin for JLink
	cp -f $<.p1 $<.p1.bin
	cp -f $<.p2 $<.p2.bin
	JLinkExe -nogui 1 -commanderscript embed/firmware/firmware_flash.jlink

# flash_firmware_t1_jlink: $(FIRMWARE_BUILD_DIR)/firmware.bin ## flash T1 core port via JLink
# 	JLinkExe -nogui 1 -commanderscript embed/firmware/firmware_flash_t1.jlink

## openocd debug commands:

openocd: ## start openocd which connects to the device
	$(OPENOCD)

openocd_reset: ## cause a system reset using OpenOCD
	$(OPENOCD) -c "init; reset; exit"

GDB = arm-none-eabi-gdb --nx -ex 'set remotetimeout unlimited' -ex 'set confirm off' -ex 'target remote 127.0.0.1:3333' -ex 'monitor reset halt'

gdb_bootloader: $(BOOTLOADER_BUILD_DIR)/bootloader.elf ## start remote gdb session to openocd with bootloader symbols
	$(GDB) $<

gdb_firmware: $(FIRMWARE_BUILD_DIR)/firmware.elf ## start remote gdb session to openocd with firmware symbols
	$(GDB) $<

## misc commands:

binctl: ## print info about binary files
	./tools/headertool.py $(BOOTLOADER_BUILD_DIR)/bootloader.bin
	./tools/headertool.py $(FIRMWARE_BUILD_DIR)/firmware.bin

bloaty: ## run bloaty size profiler
	bloaty -d symbols -n 0 -s file $(FIRMWARE_BUILD_DIR)/firmware.elf | less
	bloaty -d compileunits -n 0 -s file $(FIRMWARE_BUILD_DIR)/firmware.elf | less

sizecheck: ## check sizes of binary files
	test $(BOARDLOADER_MAXSIZE) -ge $(shell wc -c < $(BOARDLOADER_BUILD_DIR)/boardloader.bin)
	test $(BOOTLOADER_MAXSIZE) -ge $(shell wc -c < $(BOOTLOADER_BUILD_DIR)/bootloader.bin)
	test $(FIRMWARE_P1_MAXSIZE) -ge $(shell wc -c < $(FIRMWARE_BUILD_DIR)/firmware.bin.p1)
	test $(FIRMWARE_P2_MAXSIZE) -ge $(shell wc -c < $(FIRMWARE_BUILD_DIR)/firmware.bin.p2)
	test $(FIRMWARE_MAXSIZE) -ge $(shell wc -c < $(FIRMWARE_BUILD_DIR)/firmware.bin)

combine: ## combine boardloader + bootloader + prodtest into one combined image
	./tools/combine_firmware \
		$(BOARDLOADER_START) $(BOARDLOADER_BUILD_DIR)/boardloader.bin \
		$(BOOTLOADER_START) $(BOOTLOADER_BUILD_DIR)/bootloader.bin \
		$(PRODTEST_START) $(PRODTEST_BUILD_DIR)/prodtest.bin \
		> $(PRODTEST_BUILD_DIR)/combined.bin

coverage:  # generate coverage report
	./tools/coverage-report
