#ifndef __BSP_USART_H
#define __BSP_USART_H
#include "main.h"


typedef enum{

   COPY_OK,
   COPY_NG,

}copy_cmd_t;

void parse_recieve_data_handler(void);







void callback_register_usart1_rx(void);

void decoder_handler(void);



#endif 

