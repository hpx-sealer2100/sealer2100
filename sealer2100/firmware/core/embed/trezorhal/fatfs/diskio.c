// clang-format off
#include "ff.h"
#include "diskio.h"
//#include "emmc.h"
#include "stm32h747i_discovery_sd.h"
#include <stdio.h>
// clang-format on

static volatile DSTATUS Stat = STA_NOINIT;
DSTATUS disk_status(BYTE pdrv) {
    Stat = STA_NOINIT;

  if(BSP_SD_GetCardState(0) == BSP_ERROR_NONE)
  {
    Stat &= ~STA_NOINIT;
  }

  return Stat;
  }

DSTATUS disk_initialize(BYTE pdrv) { return disk_status(pdrv);}

uint32_t enter_critical(void)
{
    // 保存当前 PRIMASK 值
    uint32_t regPrimask = __get_PRIMASK();
    // 关闭系统全局中断（其实就是将 PRIMASK 设为 1）
    __disable_irq();

    return regPrimask;
}

void exit_critical(uint32_t primask)
{
    // 恢复 PRIMASK
    __set_PRIMASK(primask);
}

DRESULT disk_read(BYTE pdrv, BYTE *buff, LBA_t sector, UINT count) {
  DRESULT res = RES_ERROR;
  uint32_t primask = enter_critical();
  if(BSP_SD_ReadBlocks(0,(uint32_t*)buff,
                       (uint32_t) (sector),
                       count) == BSP_ERROR_NONE)
  {
    /* wait until the read operation is finished */
    while(BSP_SD_GetCardState(0)!= BSP_ERROR_NONE)
    {

    }
    res = RES_OK;
  }
  exit_critical(primask);
  return res;
}
DRESULT disk_write(BYTE pdrv, const BYTE *buff, LBA_t sector, UINT count) {
  DRESULT res = RES_ERROR;

  if(BSP_SD_WriteBlocks(0,(uint32_t*)buff,
                        (uint32_t)(sector),
                        count) == BSP_ERROR_NONE)
  {
  /* wait until the Write operation is finished */
    while(BSP_SD_GetCardState(0) != BSP_ERROR_NONE)
    {
    }
    res = RES_OK;
  }

  return res;
}

DRESULT disk_ioctl(BYTE pdrv, BYTE cmd, void *buff) {
  DRESULT res = RES_ERROR;
  BSP_SD_CardInfo CardInfo;

  if (Stat & STA_NOINIT) return RES_NOTRDY;

  switch (cmd)
  {
  /* Make sure that no pending write process */
  case CTRL_SYNC :
    res = RES_OK;
    break;

  /* Get number of sectors on the disk (DWORD) */
  case GET_SECTOR_COUNT :
    BSP_SD_GetCardInfo(0,&CardInfo);
    *(DWORD*)buff = CardInfo.LogBlockNbr;
    res = RES_OK;
    break;

  /* Get R/W sector size (WORD) */
  case GET_SECTOR_SIZE :
    BSP_SD_GetCardInfo(0,&CardInfo);
    *(WORD*)buff = CardInfo.LogBlockSize;
    res = RES_OK;
    break;

  /* Get erase block size in unit of sector (DWORD) */
  case GET_BLOCK_SIZE :
    BSP_SD_GetCardInfo(0,&CardInfo);
    *(DWORD*)buff = CardInfo.LogBlockSize;
  res = RES_OK;
    break;

  default:
    res = RES_PARERR;
  }

  return res;
}
