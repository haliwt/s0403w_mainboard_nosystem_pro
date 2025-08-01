#ifndef __BSP_H
#define __BSP_H
#include "main.h"

#include "main.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bsp_freertos.h"




#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"


#include "adc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

#include "bsp_cmd_link.h"
#include "bsp_buzzer.h"
#include "bsp_fan.h"
#define WIFI_RX_NUMBERS         1

extern uint8_t wifi_rx_inputBuf[WIFI_RX_NUMBERS];
extern uint8_t inputBuf[1];


typedef struct PROCESS_T{


   uint8_t gpower_on;
   uint8_t disp_rx_cmd_done_flag;
   uint8_t  wifi_rx_data_array[150];
   uint8_t wifi_rx_data_counter;


   uint8_t get_beijing_flag;
   volatile uint8_t stopTwoHours_flag;

   uint8_t receive_copy_cmd ;
   uint8_t send_ack_cmd;
   uint8_t gFan_pwm_duty_level;
  
   

   uint8_t process_run_step;
  
   uint8_t wifi_led_fast_blink_flag;
   uint8_t get_beijing_time_success;
   uint8_t link_net_step;

   
   uint8_t disp_works_hours ;    
   uint8_t disp_works_minutes ;
   uint8_t gTimer_works_time_seconds ;

   uint8_t gTimer_power_on_first_link_tencent;
   uint8_t gTimer_get_data_from_tencent_data;
   uint8_t gTimer_link_net_timer_time;
   uint8_t gTimer_dc_power_on_auto_link_net;
   uint8_t gTimer_publis_dht11_data;
   uint8_t gTimer_detect_fan_error;
   uint8_t gTimer_again_send_power_on_off;
   uint8_t  gTimer_power_on_auto_link;



}process_t;

extern process_t gpro_t;

uint8_t bcc_check(const unsigned char *data, int len) ;

#endif 
