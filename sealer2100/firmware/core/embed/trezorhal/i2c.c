#include "i2c.h"
#include "common.h"
#include "secbool.h"
#include "stm32h7xx_hal_gpio.h"

I2C_HandleTypeDef hi2c4 = {0};
volatile uint32_t __lock__ = 0;
#define I2C4_LOCK_FLAG  (1<<0)

void i2c4_init(void) {
    if (hi2c4.Instance) {
        return;
    }
    // pin setup
    I2C4_GPIO_CLK_ENABLE();
    GPIO_InitTypeDef gpio;
    gpio.Pin = I2C4_GPIO_SCL_PIN | I2C4_GPIO_SDA_PIN;
    gpio.Mode = GPIO_MODE_AF_OD;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    gpio.Alternate = GPIO_AF4_I2C4;
    HAL_GPIO_Init(I2C4_GPIO_PORT, &gpio);

    // i2c setup
    I2C4_CLK_ENABLE();
    I2C4_FORCE_RESET();
    I2C4_RELEASE_RESET();

    hi2c4.Instance = I2C4;
    // hi2c4.Init.Timing = 0x70B03140;
    hi2c4.Init.Timing = 0x10C0ECFF;
    hi2c4.Init.OwnAddress1 = 0;  // master
    hi2c4.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c4.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c4.Init.OwnAddress2 = 0;
    hi2c4.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    hi2c4.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c4.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

    if (HAL_OK != HAL_I2C_Init(&hi2c4)) {
        ensure(secfalse, NULL);
    }

    if (HAL_I2CEx_ConfigAnalogFilter(&hi2c4, I2C_ANALOGFILTER_ENABLE) != HAL_OK) {
        ensure(secfalse, NULL);
        return;
    }

    if (HAL_I2CEx_ConfigDigitalFilter(&hi2c4, 0) != HAL_OK) {
        ensure(secfalse, NULL);
        return;
    }

}

int i2c4_try_lock(void) {
    // if have lock, tell caller that i2c is using, we can't lock again
    if (__lock__ & I2C4_LOCK_FLAG) {
        return 1;
    }
    __lock__ |= I2C4_LOCK_FLAG;
    return 0;
}

void i2c4_lock(void) {
    // wait until i2c is not using
    while (__lock__ & I2C4_LOCK_FLAG) {
    }

    __lock__ |= I2C4_LOCK_FLAG;
}

void i2c4_unlock(void) {
    __lock__ &= ~I2C4_LOCK_FLAG;
}

I2C_HandleTypeDef hi2c1 = {0};
void i2c1_init(void) {
    if (hi2c1.Instance) {
        return;
    }
    // pin setup
    I2C1_GPIO_CLK_ENABLE();
    GPIO_InitTypeDef gpio;
    gpio.Pin = I2C1_GPIO_SCL_PIN | I2C1_GPIO_SDA_PIN;
    gpio.Mode = GPIO_MODE_AF_OD;
    gpio.Pull = GPIO_NOPULL;
    gpio.Speed = GPIO_SPEED_FREQ_LOW;
    gpio.Alternate = GPIO_AF4_I2C1;
    HAL_GPIO_Init(I2C1_GPIO_PORT, &gpio);

    // i2c setup
    I2C1_CLK_ENABLE();
    I2C1_FORCE_RESET();
    I2C1_RELEASE_RESET();

    hi2c1.Instance = I2C1;
    // hi2c4.Init.Timing = 0x70B03140;
    hi2c1.Init.Timing = 0x10C0ECFF;
    hi2c1.Init.OwnAddress1 = 0;  // master
    hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2c1.Init.OwnAddress2 = 0;
    hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;

    if (HAL_OK != HAL_I2C_Init(&hi2c1)) {
        ensure(secfalse, NULL);
    }

    if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK) {
        ensure(secfalse, NULL);
        return;
    }

    if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK) {
        ensure(secfalse, NULL);
        return;
    }
}
