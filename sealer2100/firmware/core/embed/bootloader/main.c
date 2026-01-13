/**
 ******************************************************************************
 * @file    JPEG/JPEG_MJPEG_VideoDecoding/CM7/Src/main.c
 * @author  MCD Application Team
 * @brief   This sample code shows how to use the HW JPEG to Decode an MJPEG video file
 *          using the STM32H7xx HAL API.
 *          This is the main program for Cortex-M7
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2019 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "bootui.h"
#include "common.h"
#include "display.h"
#include "emmc_commands.h"
#include "emmc_fs.h"
#include "ff.h"
#include "flash.h"
#include "image.h"
#include "messages.h"
#include "mipi_lcd.h"
#include "mpu.h"
#include "sdram.h"
#include "thd89/se.h"
#include "spi.h"
#include "stm32h7xx_hal.h"
#include "sys.h"
#include "usart.h"
#include "usb.h"
#include <stdint.h>
#include <stdio.h>
#include "i2c.h"

#include "battery.h"
#include "lowlevel.h"
#include "qspi_flash.h"
#include "random_delays.h"
#include "secbool.h"
#include "stm32h747xx.h"
#include "stm32h7xx_hal_gpio.h"
#include "stm32h7xx_hal_rcc.h"
#include "uart_log.h"
#include "device.h"
#include "power_manager.h"

#define FACTORY_MODE 0

#define MSG_NAME_TO_ID(x)         MessageType_MessageType_##x

#define USB_OTG_HS_DATA_FIFO_RAM  (USB_OTG_HS_PERIPH_BASE + 0x20000U)
#define USB_OTG_HS_DATA_FIFO_SIZE (4096U)

#if PRODUCTION
const uint8_t BOOTLOADER_KEY_M = 2;
const uint8_t BOOTLOADER_KEY_N = 3;
#else
const uint8_t BOOTLOADER_KEY_M = 2;
const uint8_t BOOTLOADER_KEY_N = 3;
#endif
const uint8_t* const BOOTLOADER_KEYS[] = {
#if PRODUCTION
    (const uint8_t *)"\xEE\xD4\xE5\x12\x3D\xB0\x81\x4F\x2D\xDB\x86\x6A\x68\x86\x2F\x24\xE9\xA8\x78\x34\x3E\x69\xB0\xD2\x52\x25\xB9\x51\x12\xCA\xA3\x77",
    (const uint8_t *)"\x30\x37\x00\xC4\x5C\xAD\x09\xD3\xDA\xBE\x17\x6E\x1B\xEC\x24\xF0\xA6\xA8\x5B\x80\x9F\x5E\x8E\x09\x57\x92\x57\x09\x10\x1B\xF8\xCF",
    (const uint8_t *)"\x24\xED\x84\xE2\x6B\x73\x5D\x7C\x7A\x44\xE4\xB6\xE6\xAA\xD4\x43\xA5\x5E\x6A\xD7\x50\x41\x4B\x71\x00\x6A\xF0\xD0\x16\xC7\x63\x42",
#else
    (const uint8_t *)"\xd7\x59\x79\x3b\xbc\x13\xa2\x81\x9a\x82\x7c\x76\xad\xb6\xfb\xa8\xa4\x9a\xee\x00\x7f\x49\xf2\xd0\x99\x2d\x99\xb8\x25\xad\x2c\x48",
    (const uint8_t *)"\x63\x55\x69\x1c\x17\x8a\x8f\xf9\x10\x07\xa7\x47\x8a\xfb\x95\x5e\xf7\x35\x2c\x63\xe7\xb2\x57\x03\x98\x4c\xf7\x8b\x26\xe2\x1a\x56",
    (const uint8_t *)"\x77\x29\x12\xab\x61\xd1\xdc\x4f\x91\x33\x32\x5e\x57\xe1\x46\xab\x9f\xac\x17\xa4\x57\x2c\x6f\xcd\xf3\x55\xf8\x00\x36\x10\x00\x04",
#endif
};
DMA2D_HandleTypeDef DMA2D_Handle;

FATFS SDFatFs;  /* File system object for SD card logical drive */
char SDPath[4]; /* SD card logical drive path */

static void bus_fault_enable()
{
    SCB->SHCSR |= SCB_SHCSR_BUSFAULTENA_Msk;
}
static void bus_fault_disable()
{
    SCB->SHCSR &= ~SCB_SHCSR_BUSFAULTENA_Msk;
}

void Error_Display(char* str)
{
    (void)str;
    return;
}

#define USB_IFACE_NUM   0
#define USB_PACKET_SIZE 64

static void usb_init_all(secbool usb21_landing)
{
    usb_dev_info_t dev_info = {
        .device_class = 0x00,
        .device_subclass = 0x00,
        .device_protocol = 0x00,
        .vendor_id = 0x1209,
        .product_id = 0x6869,
        .release_num = 0x0200,
        .manufacturer = "HyperPay",
        .product = "Sealer2001 Boot",
        .serial_number = "000000000000000000000000",
        .interface = "Sealer2001 Interface",
        .usb21_enabled = sectrue,
        .usb21_landing = usb21_landing,
    };

    static uint8_t rx_buffer[USB_PACKET_SIZE];

    static const usb_webusb_info_t webusb_info = {
        .iface_num = USB_IFACE_NUM,
        .ep_in = USB_EP_DIR_IN | 0x01,
        .ep_out = USB_EP_DIR_OUT | 0x01,
        .subclass = 0,
        .protocol = 0,
        .max_packet_len = sizeof(rx_buffer),
        .rx_buffer = rx_buffer,
        .polling_interval = 1,
    };

    usb_init(&dev_info);

    ensure(usb_webusb_add(&webusb_info), NULL);

    usb_start();
}

static inline bool is_usb_connect(void){
    return pm_get_power_source() == POWER_SOURCE_USB;
}

// 0: by battery, 1: by usb
int dev_pwr_source = 0;
int battery_soc = 0;
static void upate_battery_info(void) {
    battery_soc = battery_read_SOC();
    dev_pwr_source = is_usb_connect()? 1 : 0;
}

secbool bootloader_usb_loop(const vendor_header* const vhdr, const image_header* const hdr)
{ // touch click commented on development board
    // if both are NULL, we don't have a firmware installed
    // let's show a webusb landing page in this case
    usb_init_all((vhdr == NULL && hdr == NULL) ? sectrue : secfalse);

    uint8_t buf[USB_PACKET_SIZE];
    int r;

    for ( ;; )
    {
        while ( true )
        {
            ble_uart_poll();
            // check bluetooth
            if ( USB_PACKET_SIZE == spi_slave_poll(buf) )
            {
                host_channel = CHANNEL_SLAVE;
                break;
            }
            // check usb
            else if ( USB_PACKET_SIZE == usb_webusb_read_blocking(USB_IFACE_NUM, buf, USB_PACKET_SIZE, 5) )
            {
                host_channel = CHANNEL_USB;
                break;
            }

            ui_bootloader_page_switch(hdr);
            static uint32_t tickstart = 0;
            if ( (HAL_GetTick() - tickstart) >= 1000 )
            {
                upate_battery_info();
                ui_title_update();
                tickstart = HAL_GetTick();
            }
            continue;
        }

        uint16_t msg_id;
        uint32_t msg_size;
        if ( sectrue != msg_parse_header(buf, &msg_id, &msg_size) )
        {
            // invalid header -> discard
            continue;
        }

        switch ( msg_id )
        {
        case MSG_NAME_TO_ID(Initialize): // Initialize
            process_msg_Initialize(USB_IFACE_NUM, msg_size, buf, vhdr, hdr);
            break;
        case MSG_NAME_TO_ID(Ping): // Ping
            process_msg_Ping(USB_IFACE_NUM, msg_size, buf);
            break;
        case MSG_NAME_TO_ID(WipeDevice): // WipeDevice
            ui_fadeout();
            ui_wipe_confirm(hdr);
            ui_fadein();
            int response = ui_input_poll(INPUT_CONFIRM | INPUT_CANCEL, true);
            if ( INPUT_CANCEL == response )
            {
                ui_fadeout();
                // ui_bootloader_first(hdr);
                ui_fadein();
                send_user_abort(USB_IFACE_NUM, "Wipe cancelled");
                break;
            }
            ui_fadeout();
            ui_screen_wipe();
            ui_fadein();
            r = process_msg_WipeDevice(USB_IFACE_NUM, msg_size, buf);
            if ( r < 0 )
            { // error
                ui_fadeout();
                ui_screen_fail();
                ui_fadein();
                usb_stop();
                usb_deinit();
                // while (!touch_click()) {
                // }
                restart();
                return secfalse; // shutdown
            }
            else
            { // success
                ui_fadeout();
                ui_screen_done(0, sectrue);
                ui_fadein();
                usb_stop();
                usb_deinit();
                // while (!touch_click()) {
                // }
                restart();
                return secfalse; // shutdown
            }
            break;
        case MSG_NAME_TO_ID(FirmwareErase): // FirmwareErase
            process_msg_FirmwareErase(USB_IFACE_NUM, msg_size, buf);
            break;
        case MSG_NAME_TO_ID(FirmwareUpload): // FirmwareUpload
            r = process_msg_FirmwareUpload(USB_IFACE_NUM, msg_size, buf);
            if ( r < 0 && r != -4 )
            { // error, but not user abort (-4)
                ui_fadeout();
                ui_screen_fail();
                ui_fadein();
                usb_stop();
                usb_deinit();
                // while (!touch_click()) {
                // }
                restart();
                return secfalse; // shutdown
            }
            else if ( r == 0 )
            { // last chunk received
                // ui_screen_install_progress_upload(1000);
                ui_fadeout();
                ui_screen_done(4, sectrue);
                ui_fadein();
                ui_screen_done(3, secfalse);
                hal_delay(1000);
                ui_screen_done(2, secfalse);
                hal_delay(1000);
                ui_screen_done(1, secfalse);
                hal_delay(1000);
                usb_stop();
                usb_deinit();
                display_clear();
                return sectrue; // jump to firmware
            }
            break;
        case MSG_NAME_TO_ID(FirmwareErase_ex): // erase ble update buffer
            process_msg_FirmwareEraseBLE(USB_IFACE_NUM, msg_size, buf);
            break;
        case MSG_NAME_TO_ID(GetFeatures): // GetFeatures
            process_msg_GetFeatures(USB_IFACE_NUM, msg_size, buf, vhdr, hdr);
            break;
        case MSG_NAME_TO_ID(Reboot): // Reboot
            process_msg_Reboot(USB_IFACE_NUM, msg_size, buf);
            break;
        case MSG_NAME_TO_ID(FirmwareUpdateEmmc): // FirmwareUpdateEmmc
            pm_battery_long_press_reset(false);
            process_msg_FirmwareUpdateEmmc(USB_IFACE_NUM, msg_size, buf);
            pm_battery_long_press_reset(true);
            break;
        case MSG_NAME_TO_ID(EmmcFixPermission): // EmmcFixPermission
            process_msg_EmmcFixPermission(USB_IFACE_NUM, msg_size, buf);
            break;
        case MSG_NAME_TO_ID(EmmcPathInfo): // EmmcPathInfo
            process_msg_EmmcPathInfo(USB_IFACE_NUM, msg_size, buf);
            break;
        case MSG_NAME_TO_ID(EmmcFileRead): // EmmcFileRead
            process_msg_EmmcFileRead(USB_IFACE_NUM, msg_size, buf);
            break;
        case MSG_NAME_TO_ID(EmmcFileWrite): // EmmcFileWrite
            process_msg_EmmcFileWrite(USB_IFACE_NUM, msg_size, buf);
            break;
        case MSG_NAME_TO_ID(EmmcFileDelete): // EmmcFileDelete
            process_msg_EmmcFileDelete(USB_IFACE_NUM, msg_size, buf);
            break;
        case MSG_NAME_TO_ID(EmmcDirList): // EmmcDirList
            process_msg_EmmcDirList(USB_IFACE_NUM, msg_size, buf);
            break;
        case MSG_NAME_TO_ID(EmmcDirMake): // EmmcDirMake
            process_msg_EmmcDirMake(USB_IFACE_NUM, msg_size, buf);
            break;
        case MSG_NAME_TO_ID(EmmcDirRemove): // EmmcDirRemove
            process_msg_EmmcDirRemove(USB_IFACE_NUM, msg_size, buf);
            break;
        case MSG_NAME_TO_ID(ReadSEPublicKey): // ReadSEPublicKey
            process_msg_ReadSEPublicKey(USB_IFACE_NUM, msg_size, buf);
            break;
        case MSG_NAME_TO_ID(WriteSEPublicCert): // WriteSEPublicCert
            process_msg_WriteSEPublicCert(USB_IFACE_NUM, msg_size, buf);
            break;
        case MSG_NAME_TO_ID(ReadSEPublicCert): // ReadSEPublicCert
            process_msg_ReadSEPublicCert(USB_IFACE_NUM, msg_size, buf);
            break;
        case MSG_NAME_TO_ID(SESignMessage): // SESignMessage
            process_msg_SESignMessage(USB_IFACE_NUM, msg_size, buf);
            break;
#if !PRODUCTION
        case MSG_NAME_TO_ID(SEWipeUserStorage): // SEWipeUserStorage
            process_msg_SEWipeUserStorage(USB_IFACE_NUM, msg_size, buf);
            break;
#endif
        default:
            process_msg_unknown(USB_IFACE_NUM, msg_size, buf);
            break;
        }
    }
}

secbool bootloader_usb_loop_factory(const vendor_header* const vhdr, const image_header* const hdr)
{
    // if both are NULL, we don't have a firmware installed
    // let's show a webusb landing page in this case
    usb_init_all((vhdr == NULL && hdr == NULL) ? sectrue : secfalse);

    uint8_t buf[USB_PACKET_SIZE];
    int r;

    for ( ;; )
    {
        r = usb_webusb_read_blocking(USB_IFACE_NUM, buf, USB_PACKET_SIZE, USB_TIMEOUT);
        if ( r != USB_PACKET_SIZE )
        {
            continue;
        }
        host_channel = CHANNEL_USB;

        uint16_t msg_id;
        uint32_t msg_size;
        if ( sectrue != msg_parse_header(buf, &msg_id, &msg_size) )
        {
            // invalid header -> discard
            continue;
        }

        switch ( msg_id )
        {
        case MSG_NAME_TO_ID(Initialize): // Initialize
            process_msg_Initialize(USB_IFACE_NUM, msg_size, buf, vhdr, hdr);
            break;
        case MSG_NAME_TO_ID(Ping): // Ping
            process_msg_Ping(USB_IFACE_NUM, msg_size, buf);
            break;
        case MSG_NAME_TO_ID(GetFeatures): // GetFeatures
            process_msg_GetFeatures(USB_IFACE_NUM, msg_size, buf, vhdr, hdr);
            break;
        case MSG_NAME_TO_ID(DeviceInfoSettings): // DeviceInfoSettings
            process_msg_DeviceInfoSettings(USB_IFACE_NUM, msg_size, buf);
            break;
        case MSG_NAME_TO_ID(GetDeviceInfo): // GetDeviceInfo
            process_msg_GetDeviceInfo(USB_IFACE_NUM, msg_size, buf);
            break;
        case MSG_NAME_TO_ID(ReadSEPublicKey): // ReadSEPublicKey
            process_msg_ReadSEPublicKey(USB_IFACE_NUM, msg_size, buf);
            break;
        case MSG_NAME_TO_ID(WriteSEPublicCert): // WriteSEPublicCert
            process_msg_WriteSEPublicCert(USB_IFACE_NUM, msg_size, buf);
            break;
        case MSG_NAME_TO_ID(ReadSEPublicCert): // ReadSEPublicCert
            process_msg_ReadSEPublicCert(USB_IFACE_NUM, msg_size, buf);
            break;
        case MSG_NAME_TO_ID(SESignMessage): // SESignMessage
            process_msg_SESignMessage(USB_IFACE_NUM, msg_size, buf);
            break;
        case MSG_NAME_TO_ID(SEInitialize): // SEInitialize
            process_msg_SEInitialize(USB_IFACE_NUM, msg_size, buf);
            break;
        case MSG_NAME_TO_ID(SEInitializeDone): // SEInitializeDone
            process_msg_SEInitializeDone(USB_IFACE_NUM, msg_size, buf);
            break;
        case MSG_NAME_TO_ID(SEBackToRomBoot): // SEInitializeDone
            process_msg_SEBackToRomBoot(USB_IFACE_NUM, msg_size, buf);
            break;
        case MSG_NAME_TO_ID(Reboot): // Reboot
            process_msg_Reboot(USB_IFACE_NUM, msg_size, buf);
            break;
        case MSG_NAME_TO_ID(EmmcFixPermission): // EmmcFixPermission
            process_msg_EmmcFixPermission(USB_IFACE_NUM, msg_size, buf);
            break;
        case MSG_NAME_TO_ID(EmmcPathInfo): // EmmcPathInfo
            process_msg_EmmcPathInfo(USB_IFACE_NUM, msg_size, buf);
            break;
        // case MSG_NAME_TO_ID(EmmcFileRead): // EmmcFileRead
        //   process_msg_EmmcFileRead(USB_IFACE_NUM, msg_size, buf);
        //   break;
        // case MSG_NAME_TO_ID(EmmcFileWrite): // EmmcFileWrite
        //   process_msg_EmmcFileWrite(USB_IFACE_NUM, msg_size, buf);
        //   break;
        // case MSG_NAME_TO_ID(EmmcFileDelete): // EmmcFileDelete
        //   process_msg_EmmcFileDelete(USB_IFACE_NUM, msg_size, buf);
        //   break;
        case MSG_NAME_TO_ID(EmmcDirList): // EmmcDirList
            process_msg_EmmcDirList(USB_IFACE_NUM, msg_size, buf);
            break;
        case MSG_NAME_TO_ID(EmmcDirMake): // EmmcDirMake
            process_msg_EmmcDirMake(USB_IFACE_NUM, msg_size, buf);
            break;
        case MSG_NAME_TO_ID(EmmcDirRemove): // EmmcDirRemove
            process_msg_EmmcDirRemove(USB_IFACE_NUM, msg_size, buf);
            break;
        default:
            process_msg_unknown(USB_IFACE_NUM, msg_size, buf);
            break;
        }
    }
    return sectrue;
}

static secbool handle_flash_ecc_error = secfalse;
static void set_handle_flash_ecc_error(secbool val)
{
    handle_flash_ecc_error = val;
}
secbool load_vendor_header_keys(const uint8_t* const data, vendor_header* const vhdr)
{
    return load_vendor_header(data, BOOTLOADER_KEY_M, BOOTLOADER_KEY_N, BOOTLOADER_KEYS, vhdr);
}

// static
secbool check_vendor_header_lock(const vendor_header* const vhdr)
{
    uint8_t lock[FLASH_OTP_BLOCK_SIZE];
    ensure(flash_otp_read(FLASH_OTP_BLOCK_VENDOR_HEADER_LOCK, 0, lock, FLASH_OTP_BLOCK_SIZE), NULL);
    if ( 0 == memcmp(
                  lock,
                  "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF"
                  "\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF\xFF",
                  FLASH_OTP_BLOCK_SIZE
              ) )
    {
        return sectrue;
    }
    uint8_t hash[32];
    vendor_header_hash(vhdr, hash);
    return sectrue * (0 == memcmp(lock, hash, 32));
}

static secbool validate_firmware_headers(vendor_header* const vhdr, image_header* const hdr)
{
    set_handle_flash_ecc_error(sectrue);
    secbool result = secfalse;
    while ( true )
    {
        // check
        if ( sectrue != load_vendor_header_keys((const uint8_t*)FIRMWARE_START, vhdr) )
            break;

        if ( sectrue != check_vendor_header_lock(vhdr) )
            break;

        if ( sectrue != load_image_header(
                            (const uint8_t*)(FIRMWARE_START + vhdr->hdrlen), FIRMWARE_IMAGE_MAGIC,
                            FIRMWARE_IMAGE_MAXSIZE, vhdr->vsig_m, vhdr->vsig_n, vhdr->vpub, hdr
                        ) )
            break;

        // passed, return true
        result = sectrue;
        break;
    }
    set_handle_flash_ecc_error(secfalse);
    return result;
}

static secbool validate_firmware_code(vendor_header* const vhdr, image_header* const hdr)
{
    set_handle_flash_ecc_error(sectrue);
    secbool result =
        check_image_contents(hdr, IMAGE_HEADER_SIZE + vhdr->hdrlen, FIRMWARE_SECTORS, FIRMWARE_SECTORS_COUNT);
    set_handle_flash_ecc_error(secfalse);
    return result;
}

static secbool need_stay_in_bootloader(void) {
    volatile secbool boot = secfalse;
    if (*STAY_IN_FLAG_ADDR == STAY_IN_BOOTLOADER_FLAG) {
        boot = sectrue;

        // clear flag
        *STAY_IN_FLAG_ADDR = 0;
    }

    if (emmc_fs_path_exist("0:/.stay_in_bootloader")) {
        boot = sectrue;
        // clear flag
        emmc_fs_file_delete("0:/.stay_in_bootloader");
    }

    return boot;
}

void low_power_detect(void) {
    hal_delay(10);
    if (is_usb_connect()) {
        return;
    }
    int soc = battery_read_SOC();
    // have power
    if (soc > 1) {
        return;
    }

    // lowlevel
    // draw a empty battery
#define B_COLOR  RGB16(0xCD, 0x2B, 0x31)
#define B_WIDTH 80
#define B_HIGHT 180
#define B_X1 (480-B_WIDTH)/2
#define B_X2 (B_X1 + B_WIDTH)
#define B_Y1 (800-B_HIGHT)/2
#define B_Y2 (B_Y1+B_HIGHT)

    lcd_init(DISPLAY_RESX, DISPLAY_RESY, LCD_PIXEL_FORMAT_RGB565);
    lcd_pwm_init();
    ui_fadein();
    fb_fill_rect(B_X1 + 20, B_Y1-7, 40, 7, B_COLOR);
    fb_draw_vline(B_X1, B_Y1, B_HIGHT, B_COLOR);
    fb_draw_vline(B_X2, B_Y1, B_HIGHT, B_COLOR);
    fb_draw_hline(B_X1, B_Y1, B_WIDTH, B_COLOR);
    fb_draw_hline(B_X1, B_Y2, B_WIDTH, B_COLOR);
    fb_fill_rect(B_X1, B_Y2, B_WIDTH, 5, B_COLOR);

    hal_delay(500);
    int count = 5;
    while (count--) {
        int soc = battery_read_SOC();
        int C = battery_read_current();
        if (soc > 1 && C > 0) {
            break;
        }
        HAL_Delay(1000);
    }
    device_power_off();
}

int main(void)
{
    bool serial_set = false, cert_set = false, res_set = false, vaild_firmware = false;

    // use log
    uart_log_init();
    printf("hello, bootloader\n");

    HAL_Init();

    SystemCoreClockUpdate();

    /* Enable the CPU Cache */
    cpu_cache_enable();

    mpu_config_bootloader();

    random_delays_init();
    motor_init();

    device_power_on();

    bus_fault_enable();
    /* Initialize the QSPI */
    qspi_flash_init();
    qspi_flash_config();
    qspi_flash_memory_mapped();

    wait_random();
    ensure_emmcfs(emmc_fs_init(), "emmc_fs_init");
    ensure_emmcfs(emmc_fs_mount(true, false), "emmc_fs_mount");

    // bus_fault_disable();
    // /* Initialize the LCD */
    sdram_init();
    battery_init();
    pm_init();
    i2c4_init();
#if !FACTORY_MODE // in factory mode have not battery
    low_power_detect();
#endif
    device_para_init();
    se_init();


    display_backlight(0);
#if FACTORY_MODE
#warning "You are buid factory mode bootloader, this binary can't use in production"
    touch_init();
    lcd_init(DISPLAY_RESX, DISPLAY_RESY, LCD_PIXEL_FORMAT_RGB565);
    lcd_pwm_init();
    ui_fadein();
    ui_bootloader_first(NULL);
    if ( bootloader_usb_loop_factory(NULL, NULL) != sectrue )
    {
        return 1;
    }
#endif

    if ( !serial_set )
    {
        serial_set = device_serial_set();
#if !PRODUCTION && !FACTORY_MODE
        serial_set = true; // TODO: need debug.
#endif
    }

    if ( !cert_set )
    {
        // if se certificate is not set
        size_t cert_len = 0;
        cert_set = se_get_certificate_len(&cert_len) == 0;
#if !PRODUCTION && !FACTORY_MODE
        cert_set = true; // TODO: need debug.
#endif
    }

    if ( emmc_fs_path_exist("0:/res/.HYPERMATE_RESOURCE") == true )
    {
        res_set = true;
    }

    vendor_header vhdr;
    image_header hdr;

    // check if firmware valid
    if ( sectrue == validate_firmware_headers(&vhdr, &hdr) )
    {
        if ( sectrue == validate_firmware_code(&vhdr, &hdr) )
        {
            // __asm("nop"); // all good, do nothing
            vaild_firmware = true;
        }
    }

    secbool stay_in_bootloader = need_stay_in_bootloader();
    // stay_in_bootloader = sectrue;

    // check all flags
    if ( stay_in_bootloader == sectrue || vaild_firmware == false || serial_set == false ||
         cert_set == false || res_set == false )
    {
        // pm_battery_long_press_reset(false);
        BLE_CTL_PIN_INIT();
        ble_function_on();

        touch_init();
        lcd_init(DISPLAY_RESX, DISPLAY_RESY, LCD_PIXEL_FORMAT_RGB565);
        lcd_pwm_init();
        ui_fadein();
        ui_bootloader_first(NULL);
        if ( bootloader_usb_loop(&vhdr, &hdr) != sectrue )
        {
            return 1;
        }
    }

    pm_battery_long_press_reset(true);
    bus_fault_disable();
    mpu_config_off();
    jump_to(FIRMWARE_START + vhdr.hdrlen + IMAGE_HEADER_SIZE);
    return 0;
}
