/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include <stdlib.h>
#include "bh1750_config.h"
#include "bulb.h"
#include "led_config.h"
#include "lcd.h"
#include "arm_math.h"
#include "pid_controller_config.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
float Illuminance_lux = 0.0f;
unsigned int Illuminance_lux_Int = 0;
float LCD_Illuminance_lux = 0;
int lux_ref = 120000;
char buffer[6];
Lcd_HandleTypeDef lcd;
float duty_pid;
float duty_p;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/**
  * @brief  Period elapsed callback in non-blocking mode
  * @param  htim TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  if(htim == &htim7)
  {
    static unsigned int cnt = 0;
    cnt++;
    Illuminance_lux = BH1750_ReadIlluminance_lux(&hbh1750);
    Illuminance_lux_Int = Illuminance_lux * 1000.0f;

    duty_pid = PID_GetOutput(&hpid1, lux_ref, Illuminance_lux_Int);  // [%]
    duty_p = duty_pid;
    if(duty_pid > 99)
    {
      duty_pid = 99;
    }
    LED_PWM_WriteDuty(&hld1,duty_pid );


    if(cnt == 5)
    {
      uint8_t tx_buffer[32];
      int tx_msg_len = sprintf((char*)tx_buffer, "Illuminance: %5u.%03u\r", Illuminance_lux_Int / 1000, Illuminance_lux_Int % 1000);
      HAL_UART_Transmit(&huart3, tx_buffer, tx_msg_len, 100);
      cnt = 0;
      LCD_Illuminance_lux = Illuminance_lux_Int;

      gcvt(LCD_Illuminance_lux,6,buffer); //Przepisz wartość do buffera
    }
  }
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART3_UART_Init();
  MX_I2C1_Init();
  MX_TIM9_Init();
  MX_TIM7_Init();
  MX_TIM3_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */

  float Illuminance_lux_min = 0.0f;
  float Illuminance_lux_max = 0.0f;

  int size = 4;


  BH1750_Init(&hbh1750);
  HAL_TIM_Base_Start_IT(&htim7);
  HAL_TIM_Base_Start(&htim1);
  //HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
  LED_PWM_Init(&hld1);


  //Bulb_State(1);

  //Swtórz granice światła
  int* tab =Light_Boundries();
  free(tab);
  //Ustaw LCD
  // Lcd_PortType ports[] = { D4_GPIO_Port, D5_GPIO_Port, D6_GPIO_Port, D7_GPIO_Port };
   Lcd_PortType ports[] = { GPIOE, GPIOE, GPIOE, GPIOE };
   // Lcd_PinType pins[] = {D4_Pin, D5_Pin, D6_Pin, D7_Pin};
   Lcd_PinType pins[] = {GPIO_PIN_2, GPIO_PIN_3, GPIO_PIN_4, GPIO_PIN_5};

   // Lcd_create(ports, pins, RS_GPIO_Port, RS_Pin, EN_GPIO_Port, EN_Pin, LCD_4_BIT_MODE);
   lcd = Lcd_create(ports, pins, GPIOE, GPIO_PIN_6, GPIOF, GPIO_PIN_0, LCD_4_BIT_MODE);

   //Wyświetlanie napisu
   Lcd_cursor(&lcd, 0,0);
              Lcd_string(&lcd, "Pomiar w lux ");

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
   // __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1,1000);
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

    //LCD
        //Wypisz wartość lux
        Lcd_cursor(&lcd, 1,7);
                  Lcd_string(&lcd, buffer);
                  //usun poprzednie znaki
        for (int i = 0; i < 6; ++i) {
                            if (buffer[i] == NULL) {
                              Lcd_cursor(&lcd, 1,7 + i);
                                Lcd_string(&lcd, "    ");
                                continue;
                            }
                        }
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure LSE Drive Capability
  */
  HAL_PWR_EnableBkUpAccess();

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 216;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 3;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_7) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
