#ifndef __BOOT_UPDATER_H__
#define __BOOT_UPDATER_H__
// wait confirm timeout 90 seconds
#define UPDATER_WAIT_CONFIRM_TIMEOUT  (90 * 1000)
void boot_updater_poll(void);

#endif
