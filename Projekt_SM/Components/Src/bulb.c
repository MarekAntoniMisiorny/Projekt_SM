/*
 * bulb.c
 *
 *  Created on: Jan 13, 2024
 *      Author: user
 */

#include <stdbool.h>
#include "stm32f7xx_hal.h"
#include "bulb.h"

void Bulb_State(bool state)
{
  HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, state);
}
