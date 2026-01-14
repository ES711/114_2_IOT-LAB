/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "cmsis_os.h"
#include "spi.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "max7219_7segment.h"
#include "stdio.h"
#include "stdbool.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
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
TaskHandle_t handle7Segment;
TaskHandle_t handleBtn;

QueueHandle_t queueSwitch;
//QueueHandle_t queueTimer;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
void task7Segment(void *pvParm){
	uint8_t data[9];	
	uint32_t Number = 12345678;
	uint32_t defaultNum = 00000000;
	uint8_t displayMode = 0;
	while(1){		
		xQueueReceiveFromISR(queueSwitch, &displayMode, NULL);
		
		switch(displayMode){
				case 0:
					snprintf((char *)data, 9, "%8d", Number);
					break;
				
				default:
					snprintf((char *)data, 9, "%8d", defaultNum);
					break;
		}
		
		for(uint8_t i = 0;i<8;i++){
			Write_Max7219(8-i, data[i]);			
		}	
		
		vTaskDelay(100);
	}
}





/****
sw1, 2 -> rising edge
sw3, 4 -> falling edge
****/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	uint8_t mode = 0;
	
	switch(GPIO_Pin){
		//sw1 PE3
		case GPIO_PIN_3:
		{
			if(HAL_GPIO_ReadPin(SW1_GPIO_Port, SW1_Pin) == 1){
				mode = 0;
				printf("display mode:default \r\n");
				xQueueSendFromISR(queueSwitch, &mode, NULL);
			}
			break;
		}
		
		//sw2 PE4
		case GPIO_PIN_4:
		{
			if(HAL_GPIO_ReadPin(SW2_GPIO_Port, SW2_Pin) == 1){
				mode = 1;
				printf("display mode:student id \r\n");
				xQueueSendFromISR(queueSwitch, &mode, NULL);
			}
			break;
		}
		
		//sw3 PE5
		case GPIO_PIN_5:
		{
			if(HAL_GPIO_ReadPin(SW3_GPIO_Port, SW3_Pin) == 0){
				mode = 2;
				printf("display mode:system startup timer \r\n");
				xQueueSendFromISR(queueSwitch, &mode, NULL);
			}
			break;
		}
		
		//sw4 PE6
		case GPIO_PIN_6:
		{
			if(HAL_GPIO_ReadPin(SW4_GPIO_Port, SW4_Pin) == 0){
				mode = 3;
				printf("display mode:reserved, back to default \r\n");
				xQueueSendFromISR(queueSwitch, &mode, NULL);
			}
			break;
		}
		
		default:
			mode = 0;
			xQueueSendFromISR(queueSwitch, &mode, NULL);
			printf("unknown irq happend \r\n");
			break;
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
  MX_SPI1_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
	Init_Max7219();
	printf("init success \r\n");
	queueSwitch = xQueueCreate(3, sizeof(uint8_t));
	
	xTaskCreate(task7Segment, "7segment", 128, NULL, 1, &handle7Segment);
	//xTaskCreate(taskBtn, "btn_scan", 128, NULL, 1, &handleBtn);
	
	printf("start scheduler \r\n");
	printf("display mode:default \r\n");
	
	vTaskStartScheduler();
  /* USER CODE END 2 */

  /* Init scheduler */


  /* Start scheduler */


  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		
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
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
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
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
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
