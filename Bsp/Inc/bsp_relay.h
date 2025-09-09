#ifndef __BSP_RELAY_H
#define __BSP_RELAY_H
#include "main.h"

#define PTC_SetHigh()      do{RELAY_GPIO_Port->BSRR = RELAY_Pin;}while(0)//HAL_GPIO_WritePin(PTC_GPIO,PTC_PIN ,GPIO_PIN_SET) // do{PTC_GPIO->BSRR |= PTC_PIN;}while(0)//    // output high level
#define PTC_SetLow()       do{RELAY_GPIO_Port->BSRR |= (uint32_t)PTC_Pin<<16;}while(0)//HAL_GPIO_WritePin(PTC_GPIO,PTC_PIN ,GPIO_PIN_RESET)    // do{PTC_GPIO->BRR = PTC_PIN;}while(0) // HAL_GPIO_WritePin(PTC_GPIO,PTC_PIN ,GPIO_PIN_RESET)    // output low level




void Dry_Function(uint8_t id);




#endif 

