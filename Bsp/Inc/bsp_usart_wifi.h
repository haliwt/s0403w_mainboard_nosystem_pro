#ifndef __BSP_USART_WIFI_H
#define __BSP_USART_WIFI_H
#include "main.h"

void usart2_isr_callback_handler(uint8_t data);

void USART2_DAM_Send(uint8_t *txBuf,uint16_t txlen);



#endif 

