#ifndef _SE_H_
#define _SE_H_

#include <stdint.h>
#include <stddef.h>

#define SE_POWER_GPIO_CLK_ENABLE() __HAL_RCC_GPIOD_CLK_ENABLE()
#define SE_POWER_GPIO_PORT GPIOD
#define SE_POWER_GPIO_PIN GPIO_PIN_4

#define SE_POWER_ON() HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_SET)
#define SE_POWER_OFF() HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_RESET)

// use PJ15 as handshake pin
#define SE_COMBUS_GPIO_PORT GPIOJ
#define SE_COMBUS_GPIO_PIN GPIO_PIN_15

/**SPI5 GPIO Configuration
PK0     ------> SPI5_SCK
PK1     ------> SPI5_NSS  --> se_cs
PF8     ------> SPI5_MISO
PJ10     ------> SPI5_MOSI
*/

#define SPI_GPIO_CLK_ENABLE()     \
  do {                            \
    __HAL_RCC_GPIOD_CLK_ENABLE(); \
    __HAL_RCC_GPIOF_CLK_ENABLE(); \
    __HAL_RCC_GPIOK_CLK_ENABLE(); \
    __HAL_RCC_GPIOJ_CLK_ENABLE(); \
  } while (0)

// CLK
#define SPI_CLK_GPIO_PORT GPIOK
#define SPI_CLK_GPIO_PIN GPIO_PIN_0
// NSS
#define SPI_NSS_GPIO_PORT GPIOK
#define SPI_NSS_GPIO_PIN GPIO_PIN_1
// MISO
#define SPI_MISO_GPIO_PORT GPIOF
#define SPI_MISO_GPIO_PIN GPIO_PIN_8
// MOSI
#define SPI_MOSI_GPIO_PORT GPIOJ
#define SPI_MOSI_GPIO_PIN GPIO_PIN_10

#define SE_COMBUS_IS_HIGH() (HAL_GPIO_ReadPin(SE_COMBUS_GPIO_PORT, SE_COMBUS_GPIO_PIN) == GPIO_PIN_SET)
#define SE_COMBUS_IS_LOW() (HAL_GPIO_ReadPin(SE_COMBUS_GPIO_PORT, SE_COMBUS_GPIO_PIN) == GPIO_PIN_RESET)

#if !EMULATOR
int se_spi_init(void);
int se_send(uint8_t *buf, size_t size, uint32_t timeout);
int se_recv(uint8_t *buf, size_t size, uint32_t timeout);
#endif

#endif
