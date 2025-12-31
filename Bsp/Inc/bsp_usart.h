#ifndef __BSP_USART_H
#define __BSP_USART_H
#include "main.h"



#define UART1_RX_BUF_SIZE 20


extern uint8_t uart1_rx_buf[UART1_RX_BUF_SIZE];


typedef enum{

   COPY_OK,
   COPY_NG,

}copy_cmd_t;

void parse_recieve_data_handler(void);

//void usart1_isr_callback_handler(uint8_t data);



void usart1_invoke_callback(uint8_t data);

void callback_register_usart1_rx(void);

void decoder_handler(void);



#endif 

