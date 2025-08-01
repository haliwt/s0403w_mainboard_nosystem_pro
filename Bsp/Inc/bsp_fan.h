#ifndef __BSP_FAN_H__
#define __BSP_FAN_H__
#include "main.h"

#define OLDER_FAN      0


#if OLDER_FAN

#define FAN_CCW       GPIO_PIN_6

#else 
#define FAN_CCW       GPIO_PIN_5

#endif 


#define FAN_CW        GPIO_PIN_7
#define FAN_GPIO      GPIOA

#define FAN_RUN_SetHigh()            HAL_GPIO_WritePin(FAN_GPIO,FAN_CCW,GPIO_PIN_SET)    // output high level
#define FAN_RUN_SetLow()             HAL_GPIO_WritePin(FAN_GPIO,FAN_CCW,GPIO_PIN_RESET)    // output low level

#define FAN_COM_SetHigh()            HAL_GPIO_WritePin(FAN_GPIO,FAN_CW,GPIO_PIN_SET)    // output high level
#define FAN_COM_SetLow()             HAL_GPIO_WritePin(FAN_GPIO,FAN_CW,GPIO_PIN_RESET)    // output low level


#define PTC_PIN     GPIO_PIN_0
#define PTC_GPIO    GPIOB

#define PTC_SetHigh()             HAL_GPIO_WritePin(PTC_GPIO,PTC_PIN ,GPIO_PIN_SET) // do{PTC_GPIO->BSRR |= PTC_PIN;}while(0)//    // output high level
#define PTC_SetLow()               HAL_GPIO_WritePin(PTC_GPIO,PTC_PIN ,GPIO_PIN_RESET)    // do{PTC_GPIO->BRR = PTC_PIN;}while(0) // HAL_GPIO_WritePin(PTC_GPIO,PTC_PIN ,GPIO_PIN_RESET)    // output low level



#define PLASMA_SetHigh()          HAL_GPIO_WritePin(PLASMA_GPIO_Port,PLASMA_Pin,GPIO_PIN_SET)// do{PLASMA_GPIO_Port-> BSRR |= PLASMA_Pin;}while(0) //HAL_GPIO_WritePin(PLASMA_GPIO,PLASMA_PIN,GPIO_PIN_SET)    // output high level
#define PLASMA_SetLow()            HAL_GPIO_WritePin(PLASMA_GPIO_Port,PLASMA_Pin,GPIO_PIN_RESET)    // //  do{PLASMA_GPIO_Port-> BRR = PLASMA_Pin;}while(0)// HAL_GPIO_WritePin(PLASMA_GPIO,PLASMA_PIN,GPIO_PIN_RESET)    // output low level

#define NEWPCB_FAN       1



void FAN_Stop(void);






void ShutDown_AllFunction(void);
void Dry_Function(void);
void Fan_Slowly_Speed(void);
void Fan_One_Speed(void);


void Fan_Two_Speed(void);
void Fan_One_Power_Off_Speed(void);


void Fan_RunSpeed_Fun(void);
void Fan_Full_Speed(void);

void ultrasonic_fun(uint8_t sel);
void plasma_fun(uint8_t sel);

void fan_run_fun(void);


//void fan_start_fun(void);


#endif 
