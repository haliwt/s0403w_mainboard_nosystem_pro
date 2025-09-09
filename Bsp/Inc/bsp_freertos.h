#ifndef __BSP_FREERTOS_H
#define __BSP_FREERTOS_H
#include "bsp.h"



void freeRTOS_Handler(void);

void freertos_set_prority(void);

void app_decoder_task_isr_handler(void);

#endif 

