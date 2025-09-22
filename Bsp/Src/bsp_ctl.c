#include "bsp.h"


_run_t gctl_t; 

 uint8_t first_open_set_temp_flag;
 uint8_t first_set_ptc_on ,set_temp_first;


uint8_t powerOffFanRun_flag ;

uint8_t powerOffTunrOff_flag;

uint8_t set_temp_first_closeptc  ;




/**********************************************************************
	*
	*Functin Name: void Single_ReceiveCmd(uint8_t cmd)
	*Function : resolver is by usart port receive data  from display panle  
	*Input Ref:  usart receive data
	*Return Ref: NO
	*
**********************************************************************/
void SystemReset(void)
{
    
		
		__set_PRIMASK(1) ;
		HAL_NVIC_SystemReset();
		

}




/**********************************************************************
    *
    *Functin Name: void main_function_detected_handler(uint8_t cmd)
    *Function : 
    *Input Ref:  key of value
    *Return Ref: NO
    *
************************************************************************/
//void main_function_detected_handler(uint8_t cmd)
//{
//
//if(gctl_t.gTimer_senddata_panel >1  &&  cmd ==0){ //300ms
//         gctl_t.gTimer_senddata_panel=0;
//         
//           ActionEvent_Handler();
//     }
//
//
//}

/**********************************************************************
    *
    *Functin Name:  void set_temperature_compare_value_fun(void)
    *Function : 
    *Input Ref:  key of value
    *Return Ref: NO
    *
************************************************************************/
 void set_temperature_compare_value_fun(void)
{
   switch(gctl_t.set_temperature_flag){

    case 1:
     if(gctl_t.set_temperature_value  <= gctl_t.gDht11_temperature ){ //gpro_t.temp_real_value && gpro_t.smart_phone_turn_off_ptc_flag ==0){

              PTC_SetLow();
              gctl_t.gDry=0;
		      if(gctl_t.rx_set_temp_flag ==1){
				  gctl_t.rx_set_temp_flag++;
				  gctl_t.set_temp_first_closeptc =1;
			  }
           
			 
	           SendData_Set_Command(0x02,0x00); //close ptc 
	           vTaskDelay(pdMS_TO_TICKS(20));
			   gpro_t.copy_cmd_notice_buff[2]=COPY_NULL;
			   gctl_t.gTimer_copy_cmd_counter=0;
  }
  else{

	      if(gctl_t.set_temp_first_closeptc ==0 && gctl_t.ptc_on_off_flag ==0){//the first open ptc heating //WT.DEDIT 2028.08.27 modify this flow codes
	          
              PTC_SetHigh();
              gctl_t.gDry=1;//
	          SendData_Set_Command(0x02,0x01); //open ptc 
	          vTaskDelay(pdMS_TO_TICKS(10));
			  gpro_t.copy_cmd_notice_buff[2]=COPY_NULL;
			  gctl_t.gTimer_copy_cmd_counter=0;
			  gctl_t.rx_set_temp_flag=1;
			   #if DEBUG_FLAG 
 
                 printf("temp - 0 = value !!!\r\n");
				#endif 
			 
            
	      }
		   else if(gctl_t.set_temp_first_closeptc ==1 && (gctl_t.set_temperature_value -3) >= gctl_t.gDht11_temperature  && gctl_t.ptc_on_off_flag==0){//WT.DEDIT 2028.08.27 modify this flow codes
                
				PTC_SetHigh();
                gctl_t.gDry=1;//
	            SendData_Set_Command(0x02,0x01); //open ptc 
	            vTaskDelay(pdMS_TO_TICKS(10));
				gpro_t.copy_cmd_notice_buff[2]=COPY_NULL;
				 gctl_t.gTimer_copy_cmd_counter=0;
				 #if DEBUG_FLAG
 
                  printf("temp - 3 >= value !!!\r\n");
				 #endif 
				 
	      
			}


      }

   break;

	case 0 :
        
    if(gctl_t.gDht11_temperature > 39){ // must be clouse ptc.
    
            PTC_SetLow();
            gctl_t.gDry=0;
            set_temp_first_closeptc  = 1 ;
               
            SendData_Set_Command(0x02,0x00); //close ptc 
            vTaskDelay(pdMS_TO_TICKS(20));
			gpro_t.copy_cmd_notice_buff[2]=COPY_NULL;
			 gctl_t.gTimer_copy_cmd_counter=0;
      }
    else{
      if(set_temp_first_closeptc  == 1 && gctl_t.ptc_on_off_flag ==0){
               
              if(gctl_t.gDht11_temperature < 38){
                 PTC_SetHigh();
                  gctl_t.gDry=1;
                     
                  SendData_Set_Command(0x02,0x01); //open ptc  
                 vTaskDelay(pdMS_TO_TICKS(20));
				  gpro_t.copy_cmd_notice_buff[2]=COPY_NULL;
				   gctl_t.gTimer_copy_cmd_counter=0;
                }
                   

      }
      else if(gctl_t.ptc_on_off_flag ==0 &&  set_temp_first_closeptc==0){

        PTC_SetHigh();
        gctl_t.gDry=1;
          
        SendData_Set_Command(0x22,0x01); //open ptc  
        vTaskDelay(pdMS_TO_TICKS(5));
		gpro_t.copy_cmd_notice_buff[2]=COPY_NULL;
		 gctl_t.gTimer_copy_cmd_counter=0;

      }
              
    }
    break;
  }

} 


/**********************************************************************
    *
    *Functin Name: void copy_cmd_notice_handler(void)
    *Function : 
    *Input Ref:  key of value
    *Return Ref: NO
    *
************************************************************************/
void copy_cmd_notice_hanlder(void)
{
 
    if(gctl_t.gTimer_copy_cmd_counter > 399 && (gpro_t.copy_cmd_notice_buff[2]==COPY_NG || gpro_t.copy_cmd_notice_buff[2]==COPY_NULL)){
	     gctl_t.gTimer_copy_cmd_counter =0;

	if(gpro_t.copy_cmd_notice_buff[2]==COPY_NG || gpro_t.copy_cmd_notice_buff[2]==COPY_NULL){//ptc open or close copy cmd

         if( gctl_t.gDry==0){
           SendData_Set_Command(0x02,0x00); //close ptc 
            osDelay(5);

		 }
		 else{
           SendData_Set_Command(0x02,0x01); //close ptc 
            osDelay(5);
		 }
	}

    }



}

void ai_mode_display_fun(void)
{
	if(gctl_t.mode_ai_switch_flag == 1 && wifi_link_net_state()==1){
	gctl_t.mode_ai_switch_flag=0;
	if(gctl_t.gModel ==1){
	   MqttData_Publish_SetState(1);

	}
	else if(gctl_t.gModel ==2){
	  MqttData_Publish_SetState(2);
	
	}
	}

}

