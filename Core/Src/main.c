/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main Board
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * UYIJIA Version 4.2  DATA.2023.07.27
  * S0403W and S03 the same software version: 2.2 .DATA.2024.12.23
  *
  * 
  * 
  * 
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "delay.h"
#include "esp8266.h"
#include "cmd_link.h"
#include "mqtt_iot.h"
#include "run.h"
#include "subscription.h"
#include "publish.h"
#include "interrupt_manager.h"
#include "wifi_fun.h"
#include "buzzer.h"
#include "dht11.h"
#include "adc.h"
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

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
   delay_init(24);
 
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();

  MX_ADC1_Init();


  MX_TIM1_Init();
  MX_TIM3_Init();
  
 // MX_TIM16_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  /* USER CODE BEGIN 2 */
  
   HAL_TIM_Base_Start_IT(&htim3);//HAL_TIM_Base_Start(&htim3);
   UART_Start_Receive_IT(&huart1,inputBuf,1);
   //DMA usart2
   UART_Start_Receive_IT(&huart2,wifi_usart_data.wifi_inputBuf,1);
  /* USER CODE END 2 */

    __HAL_UART_ENABLE_IT(&huart1,UART_IT_ERR);
    __HAL_UART_ENABLE_IT(&huart2,UART_IT_ERR);
	
    //power_on_enable_flag= POWER_OFF;
    run_init();
  

  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
   
    /* USER CODE BEGIN 3 */
	MainBoard_Self_Inspection_PowerOn_Fun();

	Decode_Function();

    if( run_t.decodeFlag ==0){
	
       RunCommand_Connect_Handler();
    	
       RunWifi_Command_Handler();
        if(wifi_t.get_rx_beijing_time_enable==0){
    	     Tencent_Cloud_Rx_Handler();
    		 Json_Parse_Command_Fun();
    	}
        USART1_Cmd_Error_Handler(&huart1);
        USART2_Cmd_Error_Handler(&huart2);
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

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSIDiv = RCC_HSI_DIV1;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
  RCC_OscInitStruct.PLL.PLLN = 9;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV6;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
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
   uint32_t temp;
  /* USER CODE BEGIN Error_Handler_Debug */
   if(run_t.usart2_error_falg ==1){
   	   run_t.usart2_error_falg=0;
      __HAL_UART_CLEAR_OREFLAG(&huart2);
	   temp = USART2->RDR;

	   UART_Start_Receive_IT(&huart2,(uint8_t *)wifi_usart_data.wifi_inputBuf,1);
  }
  /* User can add his own implementation to report the HAL error return state */
 // __disable_irq();
 // while (1)
 // {
 //        
 // }
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
