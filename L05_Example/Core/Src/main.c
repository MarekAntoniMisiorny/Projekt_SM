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
#include "adc.h"
#include "dma.h"
#include "i2c.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "bh1750.h"
#include "bh1750_config.h"
#include <stdio.h>
#include "pid_controller_config.h"
#include "arm_math.h"

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
float light;
uint32_t cnt = 0;
int light_int;
float current;
float current_ref=3;
uint8_t tx_buffer[10];
uint8_t tx_buffer_received[4];
int duty;
float duty_pid;
const int tx_msg_len = 3;

float ain;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{


 if(htim == &htim4)
 {

	 HAL_ADC_Start(&hadc1);
	 	if(HAL_ADC_PollForConversion(&hadc1, 1) == HAL_OK)
	 	{
	 		 ain = 3.3f * HAL_ADC_GetValue(&hadc1) / 4095.0f; // [V]
	 		    current = 1000.0f * (ain / 1000);              // [mA]

	 		   duty_pid = PID_GetOutput(&hpid1, current_ref, current);  // [%]

	 	//__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, (uint32_t)(duty*10)); // ARR = 999

	 	}


	 //odczyt z czujnika
	 cnt=cnt+1;
	  light = BH1750_ReadIlluminance_lux(&hbh1750_1);
	  light = light * 1000; // mili lux
	light_int = (int)light;

	uint8_t tx_buffer[64];
	duty = strtol((char*)&tx_buffer_received, 0, 10);
	int msg_len = sprintf((char*)tx_buffer, "t %d r %d c %d\n", light_int, current, duty);
	HAL_UART_Transmit(&huart3, tx_buffer, msg_len, 100);


//	if(cnt%5 ==0){
//	uint8_t tx_buffer[64];
//	// int msg_len = sprintf((char*)tx_buffer, "mili lux: %d\n\r", light_int);
//	int msg_len = sprintf((char*)tx_buffer, "%d\n", light_int);
//	HAL_UART_Transmit(&huart3, tx_buffer, msg_len, 100);
//	}
 }

// else if(htim==&htim1)
// {
//
//	HAL_ADC_Start(&hadc1);
//	if(HAL_ADC_PollForConversion(&hadc1, 1) == HAL_OK)
//	{
//		float ain = 3.3f * HAL_ADC_GetValue(&hadc1) / 4095.0f; // [V]
//		    current = 1000.0f * (ain / 100);              // [mA]
//	}
// }


}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
if(huart == &huart3)
{
duty = strtol((char*)&tx_buffer_received, 0, 10);
int pulse = duty*10;

__HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, pulse);

sscanf((char*)&rx_buffer[1], "%f", &duty_pid);

//if(rx_buffer[0] == 'R')
//    {
//
//      sscanf((char*)&rx_buffer[1], "%f", &current_ref);
//      sscanf((char*)&rx_buffer[1], "%f", &duty_pid);
//    __HAL_TIM_SET_COMPARE(&htim3, TIM_CHANNEL_1, (uint32_t)(duty_pid*10));
//
//    }

//HAL_UART_Transmit(&huart3, tx_buffer, tx_msg_len,10);
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
  MX_DMA_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM5_Init();
  MX_I2C1_Init();
  MX_TIM6_Init();
  MX_TIM4_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  MX_USART3_UART_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim1);
  HAL_TIM_Base_Start_IT(&htim4);
  HAL_TIM_PWM_Start (& htim3 , TIM_CHANNEL_1 ) ;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
while (1)
  {
	HAL_UART_Receive_IT(& huart3 , tx_buffer_received , tx_msg_len );
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
