/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.h
  * @brief   This file contains all the function prototypes for
  *          the gpio.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GPIO_H__
#define __GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
//WIFI USART
#define WIFI_TX_Pin GPIO_PIN_2
#define WIFI_TX_GPIO_Port GPIOA
#define WIFI_RX_Pin GPIO_PIN_3
#define WIFI_RX_GPIO_Port GPIOA

//#define WIFI_EN_Pin GPIO_PIN_12       //GPIO_PIN_12 is S03 WIFI :GPIO_PIN_4 is s04w


#define WIFI_EN_Pin GPIO_PIN_5       //GPIO_PIN_12 is S03 WIFI :GPIO_PIN_4 is s04w
#define WIFI_EN_GPIO_Port GPIOB


//main boadr function GPIO

//PLASMA GPIO

#define OLDER_PCB  0

#if  OLDER_PCB //2024.07.24

#define PLASMA_Pin              GPIO_PIN_5
#define PLASMA_GPIO_Port        GPIOA

#define FAN_CCW_Pin             GPIO_PIN_6

#define FAN_GPIO_Port           GPIOA


#define FAN_CW_Pin               GPIO_PIN_7
#define FAN_CW_GPIO_Port         GPIOA

#else

#define PLASMA_Pin              GPIO_PIN_4
#define PLASMA_GPIO_Port        GPIOA

#define FAN_CCW_Pin             GPIO_PIN_5
#define FAN_GPIO_Port           GPIOA

#define FAN_CW_Pin               GPIO_PIN_7
#define FAN_CW_GPIO_Port         GPIOA

#define RELAY_Pin                   GPIO_PIN_0
#define RELAY_GPIO_Port             GPIOB
#endif

/* USER CODE END Private defines */

void MX_GPIO_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */

