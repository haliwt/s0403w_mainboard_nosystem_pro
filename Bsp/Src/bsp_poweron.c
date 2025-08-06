#include "bsp.h"


/**********************************************************************
	*
	*Functin Name: 
	*Function : be check key of value 
	*Input Ref:  key of value
	*Return Ref: NO
	*
**********************************************************************/
void power_on_handler(void)
{

   static uint8_t send_dht11;
    switch(gpro_t.process_run_step){

	case 0: //1


         gpro_t.power_on_prority_flag = 1;
         gctl_t.gTimer_senddata_panel=0; //main board function run action.
		

		 //error detected times 
		 gctl_t.ptc_warning =0;
		// gctl_t.fan_warning =0;
         gpro_t.fan_warning_flag =0;
		 gctl_t.gTimer_ptc_adc_times=0;
		 gctl_t.gTimer_fan_adc_times=0;
		
		 gctl_t.set_wind_speed_value= 100;
		 gctl_t.set_temperature_value=40;
        //POWER OFF REF 
     
        powerOffTunrOff_flag = 1;
        powerOffFanRun_flag =1;
        //
        gctl_t.first_link_tencent_cloud_flag=1;
        check_time=0;
        gpro_t.stopTwoHours_flag =0;
        stopHours_flag =0;
        gpro_t.gTimer_detect_fan_error=0;
        gpro_t.fan_run_initial_times = 0; //WT.EDIT 2025.07.31
        gctl_t.set_temperature_value=40; //power on default set temperature value is 40 degree,don't compare
         updateDht11_sensorData_toDisp();
		 gpro_t.process_run_step= 1;
	break; 


  case 1:
        
    smartphone_timer_power_on_and_normal_handler();
		 
	 gpro_t.process_run_step= 2;
	   
  break;
	
	
	case 2:
		if(gctl_t.app_timer_power_on_flag > 1)gctl_t.app_timer_power_on_flag=0;
        every_power_on_run();
	
	 gpro_t.process_run_step= 3;
	break;
	
	
	case 3:
	
         if(wifi_link_net_state() ==1 ){
    
		    
	         gctl_t.set_wind_speed_value =100;

		     MqttData_Publish_SetOpen(1);  
			 vTaskDelay(pdMS_TO_TICKS(200));
		
		 }
	
      gpro_t.process_run_step= 4;
	break;

  case 4: //5

     if(wifi_link_net_state() ==1 &&  gctl_t.app_timer_power_on_flag==0){
    
		  MqttData_Publish_Init();
		  vTaskDelay(pdMS_TO_TICKS(200));
     	}
	  gpro_t.process_run_step= 5;
	 
	 break;
	     

    case 5:
		
	
		if(wifi_link_net_state() ==1){
         
			Update_Dht11_Totencent_Value();
          
         }
	     gpro_t.process_run_step=6 ;
	break;



  case 6:

       if(gpro_t.gTimer_update_todisplay > 1){
			gpro_t.gTimer_update_todisplay=0;

			updateDht11_sensorData_toDisp();
			freertos_set_prority();
		           
        }

       gpro_t.process_run_step=7 ;


 break;
        
  case 7: 

   if(gpro_t.wifi_led_fast_blink_flag==0 && gctl_t.first_link_tencent_cloud_flag ==1 && wifi_link_net_state() ==1){
      if( gctl_t.app_timer_power_on_flag==0){
	
		  gctl_t.first_link_tencent_cloud_flag++;

               MqttData_Publish_Update_Data();
			   vTaskDelay(pdMS_TO_TICKS(200));//HAL_Delay(200);

          
    	}
	    else if(gctl_t.first_link_tencent_cloud_flag < 4){
			 gctl_t.first_link_tencent_cloud_flag++;

            Subscriber_Data_FromCloud_Handler();
    	   vTaskDelay(pdMS_TO_TICKS(200));//HAL_Delay(100);//HAL_Delay(350);
	    }
	
	  }
    
      
	 gpro_t.process_run_step=8 ;
 break; 


	  
  case 8:
      if(wifi_link_net_state() ==1 && gpro_t.gTimer_update_tencet_dht11 >4){
    	  gpro_t.gTimer_update_tencet_dht11=0;
    	  Update_Dht11_Totencent_Value();
      }

      gpro_t.process_run_step= 9;

     break;

  case 9:

     works_run_two_hours_state();

     gpro_t.process_run_step= 6;
  break;

     default:
		//gpro_t.process_run_step= 1;
		break;
  }
}


void power_on_first_handler(void)
{

   if(wifi_link_net_state() ==1 && gctl_t.app_timer_power_on_flag==0){
	
           MqttData_Publish_SetOpen(0x01);
           osDelay(100);//HAL_Delay(100);


           Publish_Data_ToTencent_Initial_Data();
			osDelay(100);//HAL_Delay(200);

           

            Subscriber_Data_FromCloud_Handler();
    		osDelay(30);//HAL_Delay(100);//HAL_Delay(350);

             SendWifiData_To_Data(0x1F,0x01);
             osDelay(20);

             updateDht11_sensorData_toDisp();
              osDelay(20);
	
	  }



}

