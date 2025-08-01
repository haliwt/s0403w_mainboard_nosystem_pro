#ifndef __BSP_SPECIAL_POWER_H_
#define __BSP_SPECIAL_POWER_H_
#include "main.h"


extern uint8_t plasma_open_flag ;
extern uint8_t ultrasonic_open_flag;
extern uint8_t dry_open_flag;

void smartphone_timer_power_on_and_normal_handler(void);
void SetPowerOff_ForDoing(void);


void ActionEvent_Handler(void);

void every_power_on_run(void);

#endif 
