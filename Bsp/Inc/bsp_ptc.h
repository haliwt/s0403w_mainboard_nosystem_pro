#ifndef  __BSP_PTC_H
#define __BSP_PTC_H
#include "main.h"

//#define PTC_SetHigh()      do{RELAY_GPIO_Port->BSRR = RELAY_Pin;}while(0)//HAL_GPIO_WritePin(PTC_GPIO,PTC_PIN ,GPIO_PIN_SET) // do{PTC_GPIO->BSRR |= PTC_PIN;}while(0)//    // output high level
//#define PTC_SetLow()       do{RELAY_GPIO_Port->BSRR = (uint32_t)RELAY_Pin<<16;}while(0)//HAL_GPIO_WritePin(PTC_GPIO,PTC_PIN ,GPIO_PIN_RESET)    // do{PTC_GPIO->BRR = PTC_PIN;}while(0) // HAL_GPIO_WritePin(PTC_GPIO,PTC_PIN ,GPIO_PIN_RESET)    // output low level

#define PTC_SetHigh()      LL_GPIO_SetOutputPin(RELAY_GPIO_Port,RELAY_Pin)//do{RELAY_GPIO_Port->BSRR = RELAY_Pin;}while(0)//HAL_GPIO_WritePin(PTC_GPIO,PTC_PIN ,GPIO_PIN_SET) // do{PTC_GPIO->BSRR |= PTC_PIN;}while(0)//    // output high level
#define PTC_SetLow()       LL_GPIO_ResetOutputPin(RELAY_GPIO_Port,RELAY_Pin)//do{RELAY_GPIO_Port->BSRR = (uint32



void Dry_Function(uint8_t id);



void ptc_update_wifi_data(void);




#endif 

