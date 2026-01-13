#include "stm32h7xx_hal.h"
#include STM32_HAL_H
#include "battery.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "i2c.h"

#define RT9426_ADDR            0x55 << 1
#define MASK_REG_FLAG3_BSC_ACS 0x01
#define SWVER                  0x06// update this ver when change rt9426 initialize code

#define REG_CNTL               (0x00)
#define REG_RSVD               (0x02)
#define REG_CURR               (0x04)
#define REG_TEMP               (0x06)
#define REG_VBAT               (0x08)
#define REG_FLAG1              (0x0A)
#define REG_FLAG2              (0x0C)
#define REG_RM                 (0x10)
#define REG_FCC                (0x12)
#define REG_AC                 (0x14)
#define REG_TTE                (0x16)
#define REG_DUMMY              (0x1E)
#define REG_INTT               (0x28)
#define REG_CYC                (0x2A)
#define REG_SOC                (0x2C)
#define REG_SOH                (0x2E)
#define REG_VER                (0x20)
#define REG_FLAG3              (0x30)
#define REG_IRQ                (0x36)
#define REG_DC                 (0x3C)
#define REG_BDCNTL             (0x3E)
#define REG_CMD3F              (0x3F)
#define REG_SWINDOW1           (0x40)
#define REG_SWINDOW2           (0x42)
#define REG_SWINDOW3           (0x44)
#define REG_SWINDOW4           (0x46)
#define REG_SWINDOW5           (0x48)
#define REG_SWINDOW6           (0x4A)
#define REG_SWINDOW7           (0x4C)
#define REG_SWINDOW8           (0x4E)
#define REG_CMD60              (0x60)
#define REG_CMD61              (0x61)
#define REG_CMD78              (0x78)

#define OP_CHECK(r)     \
    do                  \
    {                   \
        if ( (r) != 0 ) \
        {               \
            return -1;  \
        }               \
    }                   \
    while ( 0 )

#define delay(x) HAL_Delay(x)
static inline int battery_write_register(uint8_t Reg_Addr, uint16_t data)
{
  // LE encoding, we can just use &
  // uint8_t buffer[2] = {0};
  // buffer[1] = (uint8_t)data;
  // buffer[2] = (uint8_t)(data >> 8);
  if (i2c1_write_reg(RT9426_ADDR, Reg_Addr, (uint8_t*)&data, 2) != HAL_OK) {
    return -1;
  }
  return 0;
}

static inline int battery_read_register(uint8_t Reg_Addr, int16_t* data)
{

  // uint8_t buf[8] = {0};
  // int result;
  // buf[0] = Reg_Addr;
  // result = i2c_master_write(RT9426_ADDR, buf, 1);
  // OP_CHECK(result);
  // result = i2c_master_read(RT9426_ADDR, buf, 2);
  // OP_CHECK(result);
  // *data = buf[1] << 8 | buf[0];
  // LE encoding just use
  if (i2c1_read_reg(RT9426_ADDR, Reg_Addr, (uint8_t*) data, 2)) {
    return -1;
  }
  return 0;
}

volatile uint32_t charge_full_count = 0;


int battery_read_current(void)
{
  int result = 0;
  int16_t ret;
  result = battery_read_register(REG_CURR, &ret);
  OP_CHECK(result);
  return ret;
}

int battery_read_voltage(void)
{
  int result = 0;
  int16_t ret;

  result = battery_read_register(REG_VBAT, &ret);
  OP_CHECK(result);

  return ret;
}

int battery_read_temperature(void)
{
  int result = 0;
  int16_t ret;

  result = battery_read_register(REG_TEMP, &ret);
  OP_CHECK(result);

  return ret;
}

int battery_read_remained_capacity(void)
{
  int result = 0;
  int16_t ret;

  result = battery_read_register(REG_RM, &ret);
  OP_CHECK(result);

  return ret;
}

int battery_read_full_capacity(void)
{
  int result = 0;
  int16_t ret;

  result = battery_read_register(REG_FCC, &ret);
  OP_CHECK(result);

  return ret;
}

int battery_read_average_current(void)
{
  int result = 0;
  int16_t ret;

  result = battery_read_register(REG_AC, &ret);
  OP_CHECK(result);

  return ret;
}

int battery_read_SOC(void)
{
  int result = 0;
  int16_t ret;

  result = battery_read_register(REG_SOC, &ret);
  OP_CHECK(result);

  if (ret == 99) {
    int C = battery_read_current();
    if (C >= 0 && charge_full_count < 120) {
      charge_full_count += 2;
    } else if (C < 0 && charge_full_count > 0) {
      charge_full_count--;
    }
    if (charge_full_count > 30) {
      return 100;
    }
  }
  // 隐藏5%电量
  if (ret <= 5) return 0;
  if (ret >= 100) return 100;
  // [5, 100] => [0, 100]
  int scaled = (ret - 5) * 100;
  ret = scaled / 95; // 正数部分
  int reminder = scaled % 95; // 小数部分
  // 四舍五入
  if (reminder * 2 >= 95) ret += 1;
  // printf("SOC after: %d\n", ret);

  return ret > 100 ?  100 : ret;
}

int battery_read_SOH(void)
{
  int result = 0;
  int16_t ret;

  result = battery_read_register(REG_SOH, &ret);
  OP_CHECK(result);

  return ret;
}

int battery_read_design_capacity(void)
{
  int result = 0;
  int16_t ret;

  result = battery_read_register(REG_DC, &ret);
  OP_CHECK(result);

  return ret;
}

int battery_read_time_to_empty(void)
{
  int result = 0;
  int16_t ret;

  result = battery_read_register(REG_TTE, &ret);
  OP_CHECK(result);

  return ret;
}

int battery_read_state_of_health(void)
{
  int result = 0;
  int16_t ret;

  result = battery_read_register(REG_SOH, &ret);
  OP_CHECK(result);

  return ret;
}

int RT9426_Initial(void)
{
  /*---------------Define RT9426 Initail Setting----------------------*/
#define RT9426_CFG_VERSION         (0x01)
#define RT9426_BATTERY_TYPE        (4200)
#define RT9426_DC                  (800)
#define RT9426_FCC                 (800)
#define RT9426_FC_VTH              (0x6C)
#define RT9426_FC_ITH              (0x05)
#define RT9426_FC_STH              (0x05)
#define RT9426_FD_VTH              (0xA3)
#define RT9426_OPCFG1              (0x3480)
#define RT9426_OPCFG2              (0x0040)
#define RT9426_OPCFG3              (0x3EFB)
#define RT9426_OPCFG4              (0x2001)
#define RT9426_OPCFG5              (0x097F)
#define RT9426_OTC_TTH             (0x0064)
#define RT9426_OTC_CHG_ITH         (0x0B5F)
#define RT9426_OTD_TTH             (0x0064)
#define RT9426_OTD_DCHG_ITH        (0x0B5F)
#define RT9426_UVOV_TH             (0x00FF)
#define RT9426_US_TH               (0x00)
#define RT9426_CURR_DB             (0x03)
#define RT9426_QS_EN               (0x01)
#define RT9426_FC_TIME_SOCTH       (0x41)

  uint32_t g_basic_key = 0x12345678;

  uint16_t RT9426_EXTREG_SIZE=29;

  const uint16_t RT9426_EXTREG_DATA[87] = {
    0x1, 0x4E, 0x140A,
    0x2, 0x4E, 0xF404,
    0x5, 0x46, 0x4105,
    0x5, 0x48, 0x0219,
    0x5, 0x4C, 0x3223,
    0x5, 0x4E, 0x000F,
    0x6, 0x40, 0x8813,
    0x6, 0x42, 0x8224,
    0x6, 0x44, 0xAD77,
    0x6, 0x48, 0xA312,
    0x6, 0x4A, 0x6852,
    0x6, 0x4C, 0x6457,
    0x6, 0x4E, 0x1428,
    0x7, 0x40, 0x0B0D,
    0x7, 0x42, 0x0206,
    0x7, 0x44, 0x0202,
    0x7, 0x46, 0x0000,
    0x7, 0x48, 0x0000,
    0x7, 0x4A, 0x0101,
    0x7, 0x4C, 0x0001,
    0x7, 0x4E, 0xFF00,
    0x8, 0x40, 0xF1FF,
    0x8, 0x42, 0xF1F1,
    0x8, 0x44, 0xFAFA,
    0x8, 0x46, 0xF8F9,
    0x8, 0x48, 0xF1F1,
    0x8, 0x4A, 0xF8F1,
    0x8, 0x4C, 0xEFF8,
    0x8, 0x4E, 0x00EC
  };

  const uint16_t RT9426_USR_OCV_TABLE[80] = {
    0x0013, 0x4C40, 0x5280, 0x57ED, 0x5B60, 0x5BF3, 0x5C9A, 0x5DFA,
    0x5ED3, 0x5F4D, 0x5FED, 0x6080, 0x6120, 0x62BA, 0x6400, 0x650D,
    0x65ED, 0x66E6, 0x676D, 0x706D, 0x1900, 0x056D, 0x02A7, 0x009B,
    0x002B, 0x0049, 0x001B, 0x001D, 0x003B, 0x0050, 0x005B, 0x0051,
    0x0050, 0x006E, 0x006B, 0x006F, 0x0027, 0x039A, 0x0013, 0xFF80,
    0x0000, 0x0200, 0x049A, 0x0680, 0x0E4D, 0x17E6, 0x281A, 0x309A,
    0x3600, 0x39B3, 0x3D33, 0x474D, 0x4F80, 0x5466, 0x589A, 0x5D1A,
    0x6400, 0x6900, 0x000A, 0x002F, 0x0060, 0x01A7, 0x0600, 0x037E,
    0x0988, 0x08F3, 0x0452, 0x0338, 0x02CD, 0x0328, 0x0337, 0x0255,
    0x0266, 0x024F, 0x0692, 0x0047, 0x0AE8, 0x0000, 0x0000, 0x0000
  };


  int retry = 0;
  uint16_t loop=0;
  uint16_t i,j;
  int16_t regval=0;
  int16_t sts=0;


  /*------------------------Unseal RT9426----------------------------*/
  retry = 5;
  while (retry--)
  {
    delay(1);
    if (battery_write_register(REG_CNTL, g_basic_key&0xFFFF) != 0) {
        continue;
    }
    delay(1);
    if (battery_write_register(REG_CNTL, g_basic_key>>16) != 0) {
        continue;
    }
    delay(1);
    battery_write_register(REG_DUMMY, 0x0000);
    delay(1);
    battery_read_register(REG_FLAG3, &sts);
    if (sts & MASK_REG_FLAG3_BSC_ACS) {
        break;
    }
  }
  if (retry < 0) {
    return -1;
  }
  delay(10);
  /*------------------------Unseal RT9426----------------------------*/

  /*---------------Read Reg IRQ to reinital Alert pin state------------------*/
  battery_read_register(REG_IRQ, &regval);
  /*---------------Read Reg IRQ to reinital Alert pin state------------------*/

  /*---------------------Set User Define OCV Table--------------------------*/
  if (RT9426_USR_OCV_TABLE[0] == 0x0013){
    retry = 3;
    while (retry--) {
      for (i = 0; i < 9; i++) {
        battery_write_register(REG_BDCNTL,0xCB00+i);
        battery_write_register(REG_BDCNTL,0xCB00+i);
        delay(1);
        for (j = 0; j < 8; j++) {
          battery_write_register(0x40+j*2,RT9426_USR_OCV_TABLE[i*8+j]);
          delay(1);
        }
      }

      battery_write_register(REG_BDCNTL,0xCB09);
      battery_write_register(REG_BDCNTL,0xCB09);
      delay(1);
      for (i = 0; i < 5; i++) {
        battery_write_register(0x40+i*2,RT9426_USR_OCV_TABLE[72+i]);
        delay(1);
      }
      battery_write_register(REG_DUMMY,0x0000);
      delay(10);
      battery_read_register(REG_FLAG2, &regval);
      if (regval & 0x0800) {
        break;
      }
    }
    if (retry < 0) {
        return -1;
    }
  }
  __asm("NOP");
  /*---------------------Set User Define OCV Table--------------------------*/

  /*------------------------Set Alert Threshold----------------------------*/
  volatile int ret = battery_write_register(REG_BDCNTL,0x6553);
  (void)ret;
  ret = battery_write_register(REG_BDCNTL,0x6553);
  delay(1);
  ret = battery_write_register(REG_SWINDOW1,RT9426_OTC_TTH);
  delay(1);
  ret = battery_write_register(REG_SWINDOW2,RT9426_OTC_CHG_ITH);
  delay(1);
  ret = battery_write_register(REG_SWINDOW3,RT9426_OTD_TTH);
  delay(1);
  ret = battery_write_register(REG_SWINDOW4,RT9426_OTD_DCHG_ITH);
  delay(1);
  ret = battery_write_register(REG_SWINDOW5,RT9426_UVOV_TH);
  delay(1);
  ret = battery_write_register(REG_SWINDOW6,((0x4600)|(RT9426_US_TH)));
  ret = battery_write_register(REG_DUMMY,0x0000);
  delay(1);
  /*------------------------Set Alert Threshold----------------------------*/


  /*-----------------------Set Default OCV Table---------------------------*/
  ret = battery_write_register(REG_BDCNTL,0x6552);
  ret = battery_write_register(REG_BDCNTL,0x6552);
  delay(1);
  if(RT9426_BATTERY_TYPE==4400)
  {
    battery_write_register(REG_SWINDOW1,0x8200);
  }
  else if(RT9426_BATTERY_TYPE==4352)
  {
    battery_write_register(REG_SWINDOW1,0x8100);
  }
  else if(RT9426_BATTERY_TYPE==4354)
  {
    battery_write_register(REG_SWINDOW1,0x8300);
  }
  else if(RT9426_BATTERY_TYPE==4200)
  {
    ret = battery_write_register(REG_SWINDOW1,0x8000);
  }
  else
  {
    battery_write_register(REG_SWINDOW1,0x8100);
  }
  ret = battery_write_register(REG_DUMMY,0x0000);
  delay(1);
  /*-----------------------Set Default OCV Table---------------------------*/

  /*-------------------------Set OP CONFIG------------------------------*/
  ret = battery_write_register(REG_BDCNTL,0x6551);
  ret = battery_write_register(REG_BDCNTL,0x6551);
  delay(5);
  ret = battery_write_register(REG_SWINDOW1,RT9426_OPCFG1);
  delay(5);
  ret = battery_write_register(REG_SWINDOW2,RT9426_OPCFG2);
  delay(5);
  ret = battery_write_register(REG_SWINDOW3,RT9426_OPCFG3);
  delay(5);
  ret = battery_write_register(REG_SWINDOW4,RT9426_OPCFG4);
  delay(5);
  ret = battery_write_register(REG_SWINDOW5,RT9426_OPCFG5);
  delay(5);
  ret = battery_write_register(REG_DUMMY,0x0000);
  delay(5);
  /*-------------------------Set OP CONFIG------------------------------*/

  /*----------------------Set Current Deadband----------------------------*/
  ret = battery_write_register(REG_BDCNTL,0x6551);
  ret = battery_write_register(REG_BDCNTL,0x6551);
  delay(5);
  ret = battery_write_register(REG_SWINDOW7, ((0x0012) | (RT9426_CURR_DB << 8 )));
  delay(5);
  ret = battery_write_register(REG_DUMMY,0x0000);
  delay(1);
  /*----------------------Set Current Deadband----------------------------*/

  /*-----------------------Set FC FD Threshold-----------------------------*/
  regval=(RT9426_FC_VTH) | (RT9426_FC_ITH << 8 );
  ret = battery_write_register(REG_BDCNTL,0x6555);
  ret = battery_write_register(REG_BDCNTL,0x6555);
  delay(1);
  ret = battery_write_register(REG_SWINDOW3,regval);
  delay(1);
  ret = battery_write_register(REG_SWINDOW4,(0x4100|RT9426_FC_STH));
  delay(1);
  ret = battery_write_register(REG_SWINDOW6,(0x1200|RT9426_FD_VTH));
  ret = battery_write_register(REG_DUMMY,0x0000);
  delay(1);
  /*-----------------------Set FC FD Threshold-----------------------------*/

  /*--------------------------Set DC & FCC-------------------------------*/
  ret = battery_write_register(REG_BDCNTL,0x6552);
  ret = battery_write_register(REG_BDCNTL,0x6552);
  delay(1);
  ret = battery_write_register(REG_SWINDOW6,RT9426_DC);
  delay(1);
  ret = battery_write_register(REG_SWINDOW7,RT9426_FCC);
  ret = battery_write_register(REG_DUMMY,0x0000);
  delay(1);

  /*--------------------------Set DC & FCC-------------------------------*/

  /*--------------------------Set Ext Register-----------------------------*/
  if(RT9426_EXTREG_SIZE!=0){
    for (loop = 0; loop < RT9426_EXTREG_SIZE; loop++){
      ret = battery_write_register(REG_BDCNTL,0x6550+(RT9426_EXTREG_DATA[loop*3]));
      ret = battery_write_register(REG_BDCNTL,0x6550+(RT9426_EXTREG_DATA[loop*3]));
      delay(1);
      ret = battery_write_register(RT9426_EXTREG_DATA[(loop*3)+1],RT9426_EXTREG_DATA[(loop*3)+2]);
      delay(1);
    }
    ret = battery_write_register(REG_DUMMY,0x0000);
  }
  /*--------------------------Set Ext Register-----------------------------*/

  /*-------------------------Read Back Check Register-----------------------*/
  // ret = battery_write_register(REG_BDCNTL,0x6501);
  // ret = battery_write_register(REG_BDCNTL,0x6501);
  // (void)ret;
  // delay(5);
  // battery_read_register(REG_SWINDOW1,&regval);
  // if (regval != RT9426_OPCFG1) {
  //   return -1;
  // }
  // battery_read_register(REG_SWINDOW2,&regval);
  // if (regval != RT9426_OPCFG2) {
  //   return -1;
  // }
  // battery_read_register(REG_SWINDOW3,&regval);
  // if (regval != RT9426_OPCFG3) {
  //   return -1;
  // }
  // battery_read_register(REG_SWINDOW4,&regval);
  // if (regval != RT9426_OPCFG4) {
  //   return -1;
  // }
  // battery_read_register(REG_SWINDOW5,&regval);
  // if (regval != RT9426_OPCFG5) {
  //   return -1;
  // }
  // battery_read_register(REG_SWINDOW7,&regval);
  // if (regval != ((0x0012) | (RT9426_CURR_DB<<8))) {
  //   return -1;
  // }

  // battery_write_register(REG_BDCNTL,0x6502);
  // battery_write_register(REG_BDCNTL,0x6502);
  // delay(5);

  // battery_read_register(REG_SWINDOW6,&regval);
  // if (regval != RT9426_DC) {
  //   return -1;
  // }
  // battery_read_register(REG_SWINDOW7,&regval);
  // if (regval != RT9426_FCC) {
  //   return -1;
  // }

  // battery_write_register(REG_BDCNTL,0x6503);
  // battery_write_register(REG_BDCNTL,0x6503);
  // delay(5);
  // battery_read_register(REG_SWINDOW1,&regval);
  // if (regval != RT9426_OTC_TTH) {
  //   return -1;
  // }
  // battery_read_register(REG_SWINDOW2,&regval);
  // if (regval != RT9426_OTC_CHG_ITH) {
  //   return -1;
  // }
  // battery_read_register(REG_SWINDOW3,&regval);
  // if (regval != RT9426_OTD_TTH) {
  //   return -1;
  // }
  // battery_read_register(REG_SWINDOW4,&regval);
  // if (regval != RT9426_OTD_DCHG_ITH) {
  //   return -1;
  // }
  // battery_read_register(REG_SWINDOW5,&regval);
  // if (regval != RT9426_UVOV_TH) {
  //   return -1;
  // }
  // battery_read_register(REG_SWINDOW6,&regval);
  // if (regval != ((0x4600) | (RT9426_US_TH))) {
  //   return -1;
  // }

  // battery_write_register(REG_BDCNTL,0x6505);
  // battery_write_register(REG_BDCNTL,0x6505);
  // delay(5);
  // battery_read_register(REG_SWINDOW3,&regval);
  // if (regval != ((RT9426_FC_VTH) | (RT9426_FC_ITH << 8 ))) {
  //   return -1;
  // }
  // battery_read_register(REG_SWINDOW4,&regval);
  // if (regval != ((RT9426_FC_TIME_SOCTH<<8) | RT9426_FC_STH)) {
  //   return -1;
  // }
  // battery_read_register(REG_SWINDOW6,&regval);
  // if (regval != (0x1200|RT9426_FD_VTH)) {
  //   return -1;
  // }

  // if (RT9426_EXTREG_SIZE != 0) {
  //   for (loop = 0; loop < RT9426_EXTREG_SIZE; loop++) {
  //     battery_write_register(REG_BDCNTL,0x6500+(RT9426_EXTREG_DATA[loop*3]));
  //     battery_write_register(REG_BDCNTL,0x6500+(RT9426_EXTREG_DATA[loop*3]));
  //     delay(5);
  //     battery_read_register(RT9426_EXTREG_DATA[(loop*3)+1],&regval);
  //     if (regval != RT9426_EXTREG_DATA[(loop*3)+2]) {
  //       return -1;
  //     }
  //   }
  // }
  /*-------------------------Read Back Check Register-----------------------*/

  /*---------------------------Quick Start--------------------------------*/
  if(RT9426_QS_EN == 1)
  {
    battery_write_register(REG_CNTL,0x4000);
    battery_write_register(REG_DUMMY,0x0000);
    delay(5);
  }
  /*---------------------------Quick Start--------------------------------*/

  /*-----------------------------Clear RI--------------------------------*/
  battery_read_register(REG_FLAG3, &regval);
  regval = (regval & ~0x0100);
  battery_write_register(REG_FLAG3,regval);
  battery_write_register(REG_DUMMY,0x0000);
  delay(1);
  /*-----------------------------Clear RI--------------------------------*/

  /*------------------------Seal RT9426----------------------------*/
  battery_write_register(REG_CNTL,0x0020);
  battery_write_register(REG_DUMMY,0x0000);
  delay(5);
  /*------------------------Seal RT9426----------------------------*/

  return 0;
}

bool battery_is_exist(void){
  return battery_read_SOC() >= 0;
}

int battery_init(void)
{
  int result = 0;
  uint16_t count = 0;
  int16_t regval = 0;

  i2c1_init();
    /*-----------------------Standard initial-----------------------*/
    while(1)
    {
        count++;
        battery_read_register(REG_FLAG2, &regval); //--check RDY
        if(regval & 0x0080)
        {
            delay(60);
            battery_read_register(REG_FLAG3, &regval); //--check RI
            if(regval & 0x0100)
            {
                result = RT9426_Initial();
                if (result)
                {
                    return -1;
                }
                battery_write_register(REG_RSVD,SWVER); //Record SW VERSION to RSVD Register
                battery_write_register(REG_DUMMY,0x0000);
                delay(5);
                break;
            }
            else
            {
                // no need reinitialize
                break;
            }
        }
        if(count>20)
        {
            return -1;
        }
        delay(20);
    }
    /*-----------------------Standard initial-----------------------*/

    /*------------------------Force initial--------------------------*/
    count=0;
    battery_read_register(REG_RSVD, &regval);
    if(regval!=(SWVER))                    //Compare Present SWVER & Previous SWVER
    {
        /* Depends on differnet SW update condition to Reset RT9426 or not */
            // battery_write_register(REG_CNTL,0x0041); //---reset RT9426
            // Delay_msecond(DELAY_1000MS);
        /* Depends on differnet SW update condition to Reset RT9426 or not */
        while(1)
        {
            count++;
            battery_read_register(REG_FLAG2, &regval); //--check RDY
            if(regval & 0x0080)
            {
                delay(60);
				        result = RT9426_Initial();
                if (result)
                {
                    // initialize failed
                    break;
                }
                battery_write_register(REG_RSVD,SWVER);   //Record SW VERSION to RSVD Register
                battery_write_register(REG_DUMMY,0x0000);
                delay(5);
                break;
            }
            if(count>20)
            {
                // initialize failed
                return -1;
            }
            delay(20);
        }
    }
    /*------------------------Force initial-------------------------*/
  return 0;
}

void battery_test(void)
{
  int16_t bERR;
  static int16_t count;
  int result;
  short int ret;
  count++;
  printf("--------------------- %d", count);
  printf("--------------------- \n");
  bERR = battery_read_current();
  printf("电流 = %d mA\n", bERR);
  bERR = battery_read_voltage();
  printf("电压 = %d mV\n", bERR);
  bERR = battery_read_temperature();
  printf("温度 = %d\n", bERR);
  bERR = battery_read_remained_capacity();
  printf("剩余电量 = %d mAh\n", bERR);
  bERR = battery_read_full_capacity();
  printf("满电电量 = %d mAh\n", bERR);
  bERR = battery_read_average_current();
  printf("平均电流 = %d mA\n", bERR);
  bERR = battery_read_SOC();
  printf("电量 = %d%%\n", bERR);
  bERR = battery_read_design_capacity();
  printf("设计电量 = %d mAh\n", bERR);
  bERR = battery_read_time_to_empty();
  printf("可用时间 = %d M\n ", bERR);

  bERR = battery_read_state_of_health();
  printf("电池健康 = %d%%\n ", bERR);
  printf("--------------------------------------------------\n ");

  result = battery_read_register(REG_CNTL, &ret);
  if ( result != 0 )
  {
    printf("__ERR_REG_CNTL___\n ");
  }
  printf("REG_CNTL= %d \n", ret);

  result = battery_read_register(REG_RSVD, &ret);
  if ( result != 0 )
  {
    printf("__ERR_REG_RSVD___\n ");
  }
  printf("REG_RSVD= %d \n", ret);
  result = battery_read_register(REG_CURR, &ret);
  if ( result != 0 )
  {
    printf("__ERR_REG_CURR___\n ");
  }
  printf("REG_CURR= %d \n", ret);
  result = battery_read_register(REG_TEMP, &ret);
  if ( result != 0 )
  {
    printf("__ERR_REG_TEMP___\n ");
  }
  printf("REG_TEMP= %d \n", ret);

  result = battery_read_register(REG_VBAT, &ret);
  if ( result != 0 )
  {
    printf("__ERR_REG_VBAT___\n ");
  }
  printf("REG_VBAT= %d \n", ret);

  result = battery_read_register(REG_FLAG1, &ret);
  if ( result != 0 )
  {
    printf("__ERR_REG_FLAG1___\n ");
  }
  printf("REG_FLAG1= %d \n", ret);

  result = battery_read_register(REG_FLAG2, &ret);
  if ( result != 0 )
  {
    printf("__ERR_REG_FLAG2___\n ");
  }
  printf("REG_FLAG2= %d \n", ret);

  result = battery_read_register(REG_RM, &ret);
  if ( result != 0 )
  {
    printf("__ERR_REG_RM___\n ");
  }
  printf("REG_RM= %d \n", ret);

  result = battery_read_register(REG_FCC, &ret);
  if ( result != 0 )
  {
    printf("__ERR_REG_FCC___\n ");
  }
  printf("REG_FCC= %d \n", ret);

  result = battery_read_register(REG_AC, &ret);
  if ( result != 0 )
  {
    printf("__ERR_REG_AC___\n ");
  }
  printf("REG_AC= %d \n", ret);

  result = battery_read_register(REG_TTE, &ret);
  if ( result != 0 )
  {
    printf("__ERR_REG_TTE___\n ");
  }
  printf("REG_TTE= %d \n", ret);

  result = battery_read_register(REG_DUMMY, &ret);
  if ( result != 0 )
  {
    printf("__ERR_REG_DUMMY___\n ");
  }
  printf("REG_DUMMY= %d \n", ret);

  result = battery_read_register(REG_INTT, &ret);
  if ( result != 0 )
  {
    printf("__ERR_REG_INTT___\n ");
  }
  printf("REG_INTT= %d \n", ret);

  result = battery_read_register(REG_CYC, &ret);
  if ( result != 0 )
  {
    printf("__ERR_REG_CYC___\n ");
  }
  printf("REG_CYC= %d \n", ret);

  result = battery_read_register(REG_SOC, &ret);
  if ( result != 0 )
  {
    printf("__ERR_REG_SOC___\n ");
  }
  printf("REG_SOC= %d \n", ret);

  result = battery_read_register(REG_SOH, &ret);
  if ( result != 0 )
  {
    printf("__ERR_REG_SOH___\n ");
  }
  printf("REG_SOH= %d \n", ret);

  result = battery_read_register(REG_VER, &ret);
  if ( result != 0 )
  {
    printf("__ERR_REG_VER___\n ");
  }
  printf("REG_VER= %d \n", ret);

  result = battery_read_register(REG_FLAG3, &ret);
  if ( result != 0 )
  {
    printf("__ERR_REG_FLAG3___\n ");
  }
  printf("REG_FLAG3= %d \n", ret);

  result = battery_read_register(REG_IRQ, &ret);
  if ( result != 0 )
  {
    printf("__ERR_REG_IRQ___\n ");
  }
  printf("REG_IRQ= %d \n", ret);

  result = battery_read_register(REG_DC, &ret);
  if ( result != 0 )
  {
    printf("__ERR_REG_DC___\n ");
  }
  printf("REG_DC= %d \n", ret);

  result = battery_read_register(REG_BDCNTL, &ret);
  if ( result != 0 )
  {
    printf("__ERR_REG_BDCNTL___\n ");
  }
  printf("REG_BDCNTL= %d \n", ret);

  result = battery_read_register(REG_CMD3F, &ret);
  if ( result != 0 )
  {
    printf("__ERR_REG_CMD3F___\n ");
  }
  printf("REG_CMD3F= %d \n", ret);

  result = battery_read_register(REG_SWINDOW1, &ret);
  if ( result != 0 )
  {
    printf("__ERR_REG_SWINDOW1___\n ");
  }
  printf("REG_SWINDOW1= %d \n", ret);

  result = battery_read_register(REG_SWINDOW2, &ret);
  if ( result != 0 )
  {
    printf("__ERR_REG_SWINDOW2___\n ");
  }
  printf("REG_SWINDOW2= %d \n", ret);

  result = battery_read_register(REG_SWINDOW3, &ret);
  if ( result != 0 )
  {
    printf("__ERR_REG_SWINDOW3___\n ");
  }
  printf("REG_SWINDOW3= %d \n", ret);

  result = battery_read_register(REG_SWINDOW4, &ret);
  if ( result != 0 )
  {
    printf("__ERR_REG_SWINDOW4___\n ");
  }
  printf("REG_SWINDOW4= %d \n", ret);

  result = battery_read_register(REG_SWINDOW5, &ret);
  if ( result != 0 )
  {
    printf("__ERR_REG_SWINDOW5___\n ");
  }
  printf("REG_SWINDOW5= %d \n", ret);

  result = battery_read_register(REG_SWINDOW6, &ret);
  if ( result != 0 )
  {
    printf("__ERR_REG_SWINDOW6___\n ");
  }
  printf("REG_SWINDOW6= %d \n", ret);

  result = battery_read_register(REG_SWINDOW7, &ret);
  if ( result != 0 )
  {
    printf("__ERR_REG_SWINDOW7___\n ");
  }
  printf("REG_SWINDOW7= %d \n", ret);

  result = battery_read_register(REG_SWINDOW8, &ret);
  if ( result != 0 )
  {
    printf("__ERR_REG_SWINDOW8___\n ");
  }
  printf("REG_SWINDOW8= %d \n", ret);

  result = battery_read_register(REG_CMD60, &ret);
  if ( result != 0 )
  {
    printf("__ERR_RREG_CMD60___\n ");
  }
  printf("REG_CMD60= %d \n", ret);

  result = battery_read_register(REG_CMD61, &ret);
  if ( result != 0 )
  {
    printf("__ERR_REG_CMD61___\n ");
  }
  printf("REG_CMD61= %d \n", ret);

  result = battery_read_register(REG_CMD78, &ret);
  if ( result != 0 )
  {
    printf("__ERR_REG_CMD61___\n ");
  }
  printf("REG_CMD61= %d \n", ret);

  printf("--------------------------------------------------\n ");
}
