#include "bsp.h"








void ptc_update_wifi_data(void)
{
  if(get_ptc_value()==1){
   if(gpro_t.fan_warning_flag ==0 && gpro_t.ptc_warning ==0 &&	gpro_t.stopTwoHours_flag==0){ //PTC warning flag
	   
		
		   PTC_SetHigh();
		
		   if(wifi_link_net_state()==1){ 
			   MqttData_Publish_SetPtc(0x01);
			   vTaskDelay(pdMS_TO_TICKS(200));
		   }
		   
	   
	   }
   }
   else if(get_ptc_value() ==0){
	  

	   PTC_SetLow();
	    if(wifi_link_net_state()==1){ 
			   MqttData_Publish_SetPtc(0x0);
			   vTaskDelay(pdMS_TO_TICKS(200));//osDelay(100);//HAL_Delay(350);
		   }
	   }
 }



