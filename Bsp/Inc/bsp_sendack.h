#ifndef __BSP_SENDACK_H
#define __BSP_SENDACK_H
#include "main.h"

typedef enum ack_sig{

  ack_null,
  ack_power_on ,
  ack_power_off,
  ack_wifi_on,
  ack_ptc_on,
  ack_ptc_off,
  ack_plasma_on,
  ack_plasma_off,
  ack_ultra_on,
  ack_ultra_off,
  //wifi cmd

  ack_app_power_on,
  ack_app_power_off,
  ack_app_timer_power_on,
  
  

}ack_e;



void receive_data_from_display(uint8_t *pdata);



void send_cmd_ack_hanlder(void);







#endif 



