/*
 * program.c
 *
 *  Created on: Jan 13, 2024
 *      Author: user
 */

#include "bulb.h"
#include <stdbool.h>
#include "stm32f7xx_hal.h"
#include "led_config.h"
#include "tim.h"
#include "delay_us.h"
#include "bh1750_config.h"
#include <stdio.h>
#include <stdlib.h>
int* Light_Boundries()
{
  int size = 4;
  int* Boundries = (int*) malloc(sizeof(*Boundries) * size);
  float H_Boundry_with_Bulb;
  float L_Boundry_With_Bulb;
  float H_Boundry_No_Bulb;
  float L_Boundry_No_Bulb;
  unsigned int illuminance_int = 0;

    Bulb_State(1); //Light Bulb
    LED_PWM_WriteDuty(&hld1, 99);
    HAL_Delay(500);
    H_Boundry_with_Bulb = BH1750_ReadIlluminance_lux(&hbh1750);
    illuminance_int =  H_Boundry_with_Bulb * 1000.0f;
    Boundries[0] =   illuminance_int;
    HAL_Delay(100);
    Bulb_State(1);
    LED_PWM_WriteDuty(&hld1, 0);
    HAL_Delay(500);
    L_Boundry_With_Bulb =  BH1750_ReadIlluminance_lux(&hbh1750);
    illuminance_int =  L_Boundry_With_Bulb * 1000.0f;
    Boundries[1]=  illuminance_int;

    HAL_Delay(100);
    Bulb_State(0); //OFF Bulb
    LED_PWM_WriteDuty(&hld1, 99); //Light LED
    HAL_Delay(500);// wait 0.5s
    H_Boundry_No_Bulb =  BH1750_ReadIlluminance_lux(&hbh1750);
    HAL_Delay(100);
    illuminance_int = H_Boundry_No_Bulb * 1000.0f;
    Boundries[2]= illuminance_int;

    HAL_Delay(100);
    Bulb_State(0); //OFF Bulb
    LED_PWM_WriteDuty(&hld1, 0); //OFF LED
    HAL_Delay(500);// wait 0.5s
    L_Boundry_No_Bulb =  BH1750_ReadIlluminance_lux(&hbh1750);
    illuminance_int = L_Boundry_No_Bulb * 1000.0f;
    Boundries[3] =  illuminance_int;





  return Boundries;
}
