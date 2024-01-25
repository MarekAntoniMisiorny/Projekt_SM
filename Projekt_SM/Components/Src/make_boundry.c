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
  int size_temp = 10;
  int* Boundries = (int*) malloc(sizeof(*Boundries) * size);
  int Boundries_temp = 0;
  float H_Boundry_with_Bulb;
  float L_Boundry_With_Bulb;
  float H_Boundry_No_Bulb;
  float L_Boundry_No_Bulb;
  unsigned int illuminance_int = 0;

    Bulb_State(1); //Light Bulb
    LED_PWM_WriteDuty(&hld1, 99);
    delay_us(2000);

    for (int i = 0; i < 10;i++)
    {
      LED_PWM_WriteDuty(&hld1, 99);
      H_Boundry_with_Bulb = BH1750_ReadIlluminance_lux(&hbh1750);
      illuminance_int =  (unsigned int)(H_Boundry_with_Bulb * 1000.0f);
      Boundries_temp = Boundries_temp + illuminance_int;
      delay_us(100);
    }
    Boundries_temp = Boundries_temp / 10;
    Boundries[0] =   Boundries_temp;
    Boundries_temp = 0;
    LED_PWM_WriteDuty(&hld1, 0);
    delay_us(2000);


    for (int i = 0; i < 10;i++)
        {

      L_Boundry_With_Bulb = BH1750_ReadIlluminance_lux(&hbh1750);
          illuminance_int =  (unsigned int)(L_Boundry_With_Bulb * 1000.0f);
          Boundries_temp = Boundries_temp + illuminance_int;
          delay_us(200);
        }
        Boundries_temp = Boundries_temp / 10;
        Boundries[1] =   Boundries_temp;
        Boundries_temp = 0;
    Bulb_State(0); //OFF Bulb
    delay_us(2000);

    LED_PWM_WriteDuty(&hld1, 99); //Light LED
    delay_us(2000);

    for (int i = 0; i < 10;i++)
            {
      LED_PWM_WriteDuty(&hld1, 99);
              H_Boundry_No_Bulb = BH1750_ReadIlluminance_lux(&hbh1750);
              illuminance_int =  (unsigned int)(H_Boundry_No_Bulb * 1000.0f);
              Boundries_temp = Boundries_temp + illuminance_int;
              delay_us(200);
            }
            Boundries_temp = Boundries_temp / 10;
            Boundries[2] =   Boundries_temp;
            Boundries_temp = 0;
    LED_PWM_WriteDuty(&hld1, 0); //OFF LED
    delay_us(2000);

    for (int i = 0; i < 10;i++)
                {

                  L_Boundry_No_Bulb = BH1750_ReadIlluminance_lux(&hbh1750);
                  illuminance_int =  (unsigned int)(L_Boundry_No_Bulb * 1000.0f);
                  Boundries_temp = Boundries_temp + illuminance_int;
                  delay_us(200);
                }
                Boundries_temp = Boundries_temp / 10;
                Boundries[3] =   Boundries_temp;
                Boundries_temp = 0;
//  delay_us(1000);
//  Bulb_State(1); //OFF Bulb
//  delay_us(1000);
//  Bulb_State(0); //OFF Bulb
//  delay_us(1000);
//  Bulb_State(1); //OFF Bulb
//  delay_us(1000);
//  Bulb_State(0); //OFF Bulb
  return Boundries;
}
