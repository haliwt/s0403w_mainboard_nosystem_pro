#ifndef __BSP_USART_H
#define __BSP_USART_H
#include "main.h"


typedef enum{

   COPY_NULL,
   COPY_NG,
   COPY_OK

}copy_cmd_t;

void parse_recieve_data_handler(void);

//void usart1_isr_callback_handler(uint8_t data);

void usart1_protocol_state_machine(void);

void usart1_invoke_callback(uint8_t data);

void callback_register_usart1_rx(void);


#endif 

