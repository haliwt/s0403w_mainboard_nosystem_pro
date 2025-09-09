#ifndef __BSP_USART_H
#define __BSP_USART_H
#include "main.h"


void parse_recieve_data_handler(void);

void usart1_isr_callback_handler(uint8_t data);


#endif 

