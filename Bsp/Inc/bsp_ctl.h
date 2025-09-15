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
    
	uint8_t gPower_On;



	uint8_t gDht11_flag;
	uint8_t gTemperature;

	uint8_t gDht11_humidity ;
	uint8_t gDht11_temperature;


    


	uint8_t dp_link_wifi_fail;
	uint8_t wifi_set_temperature_value_flag;

    //iwdg ref
	uint8_t process_run_guarantee_flag;
	uint8_t usart2_error_falg ;
	uint8_t power_off_fan_state ;

    uint8_t process_run_step;
    uint8_t fan_stop_flag ;
	uint8_t set_temperature_flag;
   
  uint8_t iwdg_the_first_falg;
   
	//uint8_t gTimer_usart_error;
    //app timer
	//uint8_t app_timer_power_on_ref;
	uint8_t app_timer_power_on_flag ;


    
    uint8_t decodeFlag;
	uint8_t sendtimes;
    uint8_t setup_timer_flag;

    uint8_t gTimer_send_0xaa;
	uint8_t response_wifi_signal_label;
    uint8_t flash_write_data_error;
	uint8_t flash_write_data_flag;
    uint8_t flash_read_data;





	//fan:
	uint8_t gFan_pwm_duty_level;

	uint8_t gTimer_fan_run_one_minute;
	uint8_t  gFan;
    //ptc:
   
  
      

      uint8_t  gAi;
 
      uint8_t  gDry;

	  uint8_t  gUlransonic;
	  uint8_t  gModel;
	  uint8_t gPlasma;
    

	  
      uint8_t  Single_cmd ;
	  uint8_t send_link_cloud_times;
	  //wifi link ref
	  uint8_t first_link_tencent_cloud_flag ;
	
	  uint8_t noBuzzer_sound_dry_flag;
	  
	  uint8_t  set_temperature_value;
	  uint8_t  set_temperature_decade;
	  uint8_t  set_temperature_unit;
	  uint8_t  set_wind_speed_value;
	  uint8_t auto_link_cloud_flag;
	  uint8_t reconnect_tencent_cloud_flag;
	  uint8_t rx_set_temp_flag;
	
	
      
  
	  uint8_t  wind_speed_decade;
	  uint8_t  wind_speed_unit;
      uint8_t wifi_config_net_lable;
    
	  //ADC
	  uint8_t ptc_warning;
	  uint8_t ptc_remove_warning_send_data;



	
	  

	  uint8_t gTimer_fan_adc_times;
	  uint8_t gTimer_ptc_adc_times;
     // uint8_t fan_warning;
	

	 uint8_t gTimer_senddata_panel;
	
	 uint16_t ptc_temp_voltage;
   
	 uint8_t gTimer_linkTencentCounter;
	


    
	uint32_t randomName[1];
	

	

	
}_run_t;

//#define CProcessInit(me_) ((me_)->cmdCtr__ =0,(me_)->state__ = IDLE)
extern uint8_t ReceiveBuffer[1];
extern _run_t gctl_t; 

extern uint8_t powerOffFanRun_flag ;

extern uint8_t powerOffTunrOff_flag;









void main_function_detected_handler(uint8_t cmd);




#endif 


