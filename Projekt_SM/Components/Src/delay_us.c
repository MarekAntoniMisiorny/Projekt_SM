/*
 * delay_us.c
 *
 *  Created on: Jan 16, 2024
 *      Author: user
 */

#include <stdio.h>
#include "tim.h"

void delay_us(uint16_t us)
{
uint16_t cnt =0;
//  __HAL_TIM_SET_COUNTER(&htim1,0); //set counter value to 0
//  int timer_check =  __HAL_TIM_GET_COUNTER(&htim1);
//  while(__HAL_TIM_GET_COUNTER(&htim1) < us); //wait for counter to reach the us input in the parameter

  __HAL_TIM_SET_COUNTER(&htim1,0); //set counter value to 0
  while( cnt<us)
  {
  if(__HAL_TIM_GET_COUNTER(&htim1) >=999 ){__HAL_TIM_SET_COUNTER(&htim1,0); cnt=cnt+1; };
};
}
