#include STM32_HAL_H
#include "button.h"

#include <stdbool.h>
#include <stdio.h>

#include "common.h"

// 100ms 一次, 超过 5 秒 关机
#define FORCE_POWER_OFF_COUNT 50

#define BTN_POWER_PIN     GPIO_PIN_4
#define BTN_POWER_PORT    GPIOJ
#define BTN_POWER_CLK_ENA __HAL_RCC_GPIOJ_CLK_ENABLE
static char last_power = 0;


TIM_HandleTypeDef htim2;

static void button_timer_init(void)
{
    // 1. 启用 TIM2 时钟
    __HAL_RCC_TIM2_CLK_ENABLE();

    // 2. 配置 TIM2
    htim2.Instance = TIM2;
    // 100ms
    htim2.Init.Period = 999;
    htim2.Init.Prescaler = 19999;
    htim2.Init.ClockDivision = 0;
    htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim2.Init.RepetitionCounter = 0;
    HAL_TIM_Base_Init(&htim2);

    // 3. 启用定时器中断
    HAL_NVIC_SetPriority(TIM2_IRQn, 2, 0);
    HAL_NVIC_EnableIRQ(TIM2_IRQn);

    HAL_TIM_Base_Start_IT(&htim2);
}


void button_init(void)
{
    BTN_POWER_CLK_ENA();
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
    GPIO_InitStructure.Pull = GPIO_NOPULL;
    GPIO_InitStructure.Speed = GPIO_SPEED_FREQ_LOW;
    GPIO_InitStructure.Pin = BTN_POWER_PIN;
    HAL_GPIO_Init(BTN_POWER_PORT, &GPIO_InitStructure);

    button_timer_init();
}

uint32_t button_read(void)
{
    char power = (GPIO_PIN_SET == HAL_GPIO_ReadPin(BTN_POWER_PORT, BTN_POWER_PIN));
    if ( last_power != power )
    {
        last_power = power;
        if ( power )
        {
            return BTN_EVT_DOWN | BTN_POWER;
        }
        else
        {
            return BTN_EVT_UP | BTN_POWER;
        }
    }
    return 0;
}

void button_state_monitor(void)
{
  volatile bool pressed = false;
  volatile static uint16_t count = 0;
  pressed = HAL_GPIO_ReadPin(BTN_POWER_PORT, BTN_POWER_PIN) == GPIO_PIN_SET;
  if (!pressed) {
    // reset timer count
    count = 0;
    return;
  }

  // 100ms 一次
  if (count++ > 50) {
    printf("User clicked power button 5s, restart device\n");
    count = 0;
    // if user press power button more than 5 seconds, restart
    // can't shutdown because:
    // 1. if USB is connected, can't shutdown
    // 2. if BATTERY is connected, press power button means battery is supplying power, it can't shutdown
    restart();
  }
}

// callback handler at `timer.c`
void TIM2_IRQHandler(void)
{
    HAL_TIM_IRQHandler(&htim2);
}
