#ifndef __BSP_PLASMA_H
#define __BSP_PLASMA_H
#include "main.h"

#define PLASMA_SetHigh()        LL_GPIO_SetOutputPin(PLASMA_GPIO_Port,PLASMA_Pin)
#define PLASMA_SetLow()         LL_GPIO_ResetOutputPin(PLASMA_GPIO_Port,PLASMA_Pin)



void plasma_fun(uint8_t sel);


#endif 

