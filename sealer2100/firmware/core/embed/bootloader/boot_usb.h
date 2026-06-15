#ifndef __BOOT_USB_H__
#define __BOOT_USB_H__
#include <stdbool.h>
#include <stdint.h>

void boot_usb_init(bool usb21_landing);
void boot_usb_poll(void);
void boot_usb_state_poll(void);

void boot_usb_msc_init(void);


#endif
