#ifndef __INTERRUPT_MANAGER_H_
#define __INTERRUPT_MANAGER_H_
#include "main.h"

extern volatile uint8_t counter_two_hours;

void tim17_isr_callback_handler(void);


#endif 
