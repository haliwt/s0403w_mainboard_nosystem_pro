#ifndef __BSP_CTL_H__
#define __BSP_CTL_H__
#include "main.h"

#define WIFI_TUYA      1  //
#define AI_ENABLE      1


#define  TEST_UNIT     0


typedef enum {                   /* enumeration for CProcess signals */
    kill =1,notkill,dry,notdry,ai,notai
}Signal;

typedef enum
{
  AIENABLE = 0,
  AIDISABLE = !AIENABLE
} AiStatus;

typedef enum
{
  WIFIENABLE =1,
  WIFIDISABLE = !WIFIENABLE

}WifiStatus;

typedef enum{

  // POWER_ON =0x01,
  // POWER_OFF,
   RUN_COMMAND,
   POWER_CONNECTOR_WIFI,
   UPDATE_TO_PANEL_DATA,
   WIFI_RESTART_INIT,
   FAN_CONTINUCE_RUN_ONE_MINUTE,
   POWER_ON_FAN_CONTINUCE_RUN_ONE_MINUTE,
  
   POWER_NULL, //9
   KEY_NULL

 }run_state_t;

typedef enum{

	 DRY_ON = 0x40,
	 DRY_OFF = 0x41,

	 DRY_ON_NO_BUZZER = 0x90,
	 DRY_OFF_NO_BUZZER = 0x91,

	 PLASMA_ON = 0x20,
	 PLASMA_OFF = 0x21,

	 FAN_ON =0x10,
	 FAN_OFF = 0x11,

	 MODE_AI = 0x04,
	 MODE_TIMER = 0x14,
	 MODE_AI_NO_BUZZER= 0x44,

	 WIFI_CONNECT_FAIL = 0x55,
     DISPLAY_PANNEL_CONNECT_WIFI_SUCCESS= 0xAA,





}works_t;



typedef struct _RUN_T{
    
	
   volatile  uint8_t gDht11_humidity ;
	volatile uint8_t gDht11_temperature;
    volatile uint8_t set_temperature_flag;
   
   volatile uint8_t app_timer_power_on_flag ;

   volatile uint8_t response_wifi_signal_label;
  volatile uint8_t ptc_on_off_flag ;
  volatile uint8_t set_temp_first_closeptc ;

 //fan:
    
	volatile uint8_t  gFan;
    //ptc:
   
   volatile   uint8_t  gDry;

	volatile  uint8_t  gUlransonic;
	volatile  uint8_t  gModel;
	volatile  uint8_t gPlasma;

	volatile  uint8_t  mode_ai_switch_flag;
    

	 
	  //wifi link ref
	volatile  uint8_t first_link_tencent_cloud_flag ;
	

	  
	  volatile uint8_t  set_temperature_value;


	volatile  uint8_t  set_wind_speed_value;
	volatile  uint8_t auto_link_cloud_flag;
	 volatile uint8_t reconnect_tencent_cloud_flag;
	volatile  uint8_t rx_set_temp_flag;
	
	volatile uint8_t  wind_speed_decade;
	volatile  uint8_t  wind_speed_unit;
   volatile   uint8_t wifi_config_net_lable;
    
	  //ADC
	volatile  uint8_t ptc_warning;
	volatile  uint8_t fan_warning;


	  
      //timer ref 
	volatile  uint8_t gTimer_fan_adc_times;
	volatile  uint8_t gTimer_ptc_adc_times;
   
	

	volatile uint8_t gTimer_senddata_panel;
	
	volatile uint8_t gTimer_fan_run_one_minute;
   volatile  uint8_t gTimer_read_dht11_counter;
	volatile uint8_t gTimer_linkTencentCounter;
	 
	volatile uint16_t gTimer_copy_cmd_counter;
	volatile uint8_t gTimer_wifi_detected_counter;
	
	volatile uint32_t randomName[1];
	

}_run_t;

//#define CProcessInit(me_) ((me_)->cmdCtr__ =0,(me_)->state__ = IDLE)
extern uint8_t ReceiveBuffer[1];
extern _run_t gctl_t; 

extern uint8_t powerOffFanRun_flag ;

extern uint8_t powerOffTunrOff_flag;


void copy_cmd_notice_hanlder(void);


//void main_function_detected_handler(uint8_t cmd);


void set_temperature_compare_value_fun(void);

void ai_mode_display_fun(void);


#endif 


