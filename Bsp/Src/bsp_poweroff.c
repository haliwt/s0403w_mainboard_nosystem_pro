#include "bsp.h"


static void power_off_stop_fun(void);


/**********************************************************************
    *
    *Functin Name: void power_off_handler(void)
    *Function : 
    *Input Ref:  key of value
    *Return Ref: NO
    *
************************************************************************/
void power_off_handler(void)
{

   // static uint8_t fan_run_one_minute_flag;
    switch(powerOffTunrOff_flag){

    case 1:

          gpro_t.gTimer_poweroff_fan=0;
          gctl_t.gTimer_fan_run_one_minute=0;
       
      
        gctl_t.set_wind_speed_value=10;
		gctl_t.gModel =1;
		gctl_t.app_timer_power_on_flag =0;
		
	
	  
        
	    gpro_t.stopTwoHours_flag=0;

		  gctl_t.ptc_warning =0;
		 //gctl_t.fan_warning =0;
       
		 gctl_t.gTimer_ptc_adc_times=0;
		 gctl_t.gTimer_fan_adc_times=0;
         gpro_t.process_run_step=0;//gpro_t.process_run_step

         

          SetPowerOff_ForDoing();
		  powerOffTunrOff_flag = 2;
       
      break;

      case 2:
        
       if(wifi_link_net_state() == 1){

          MqttData_Publish_PowerOff_Ref(); 
          vTaskDelay(pdMS_TO_TICKS(200)); //WT.EDTI 2024.11.19 
       }
         powerOffTunrOff_flag = 4;
       break;

       case 4:

          if(gctl_t.ptc_warning == 1){
		 	
		  	Publish_Data_Warning(ptc_temp_warning,0);
		  	vTaskDelay(pdMS_TO_TICKS(100));
            
          }
           powerOffTunrOff_flag = 5;
        break;

        case 5:
            if(gctl_t.ptc_warning == 1){
			Publish_Data_Warning(fan_warning,0);
			vTaskDelay(pdMS_TO_TICKS(100));
			
          }
        powerOffTunrOff_flag = 6;
      break;


      case 6:
	   
		if(gpro_t.gTimer_poweroff_fan < 60 && powerOffFanRun_flag ==1){
          
                   
			Fan_One_Power_Off_Speed();
                  
        }       
        else if(gpro_t.gTimer_poweroff_fan > 59   ){ //WT.EDTI 2024.11.19
		   
			       powerOffFanRun_flag=2;
				   FAN_Stop();
         }

        if(gpro_t.gTimer_poweroff_fan > 61){
              gpro_t.gTimer_poweroff_fan =0;

              powerOffFanRun_flag=2;
              gctl_t.fan_stop_flag =1;
              FAN_Stop();


        }
        gpro_t.stopTwoHours_flag =0;
       
        power_off_stop_fun();

     break;

     }

    
	  
}

/**********************************************************************
    *
    *Functin Name:void power_off_stop_fun(void)
    *Function : 
    *Input Ref:  key of value
    *Return Ref: NO
    *
************************************************************************/
static void power_off_stop_fun(void)
{
      
      PLASMA_SetLow(); //
      HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);//ultrasnoic Off 
      PTC_SetLow();
      

}

    
void power_off_action_fun(void)
{
    PLASMA_SetLow(); //
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);//ultrasnoic Off 
    PTC_SetLow();

   


}





