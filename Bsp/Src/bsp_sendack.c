#include "bsp.h"





/**********************************************************************
    *
    *Function Name:void receive_data_from_display(uint8_t *pdata,uint8_t len)
    *Function: receive usart touchkey of command 
    *Input Ref:NO
    *Return Ref:NO
    *
**********************************************************************/
void receive_data_from_display(uint8_t *pdata)
{

 
   if(pdata[1] == 0x01){

    switch(pdata[2]){

     case 0:


     break;

     case 0x01: //??????????

        if(pdata[3] == 0x01){ //open
          
           buzzer_sound();//buzzer_sound_fun();
            SendWifiData_Answer_Cmd(0x01,0x01);
            vTaskDelay(pdMS_TO_TICKS(10));
            
           
           	gpro_t.gpower_on = power_on;
           
  

        }
        else if(pdata[3] == 0x0){ //close 

          
	
              
			  SendWifiData_Answer_Cmd(0x01,0x02); //power off .
              vTaskDelay(pdMS_TO_TICKS(10)); 
              buzzer_sound();
              freertos_set_prority();
             
              
             powerOffTunrOff_flag=1;
             gpro_t.gpower_on = power_off;
			 
			
        }

     break;

     case 0x02: //PTC?????????

     if(pdata[3] == 0x01){
          buzzer_sound();
          gctl_t.gDry = 1;
	      gpro_t.ptc_switch_flag =open;

        
      if(gpro_t.stopTwoHours_flag==0){
         if(gpro_t.pct_warning ==0 && gpro_t.fan_warning_flag ==0){ //PTC warning flag
              PTC_SetHigh();
              gpro_t.ptc_switch_flag =open;
		     gctl_t.gTimer_senddata_panel=7;
          }
      
       }
       else if(pdata[3] == 0x0){
        buzzer_sound();
          gctl_t.gDry =0;
        
         PTC_SetLow();
         gpro_t.ptc_switch_flag =close;
         gctl_t.gTimer_senddata_panel=7;

       }

     break;

     case 0x03: //PLASMA ACTIVE OPEN OR CLOSE

        if(pdata[3] == 0x01){
           
            buzzer_sound();
           
           gctl_t.gPlasma = 1;
           gpro_t.plasma_switch_flag =1;
          if(gpro_t.stopTwoHours_flag==0){
            PLASMA_SetHigh();
          }
        }
        else if(pdata[3] == 0x0){
           buzzer_sound();
           
           gctl_t.gPlasma = 0;
           gpro_t.plasma_switch_flag =2;
        
          PLASMA_SetLow();

        }


     break;


      case 0x04: //ultrasonic  ACTIVE OPEN OR CLOSE

        if(pdata[3] == 0x01){  //open 
          
          gctl_t.gUlransonic =1;
          gpro_t.ultrasonic_switch_flag =1;

        }
        else if(pdata[3] == 0x0){ //close 

          gctl_t.gUlransonic = 0;
          gpro_t.ultrasonic_switch_flag =2;

        }


     break;

      case 0x05: // link wifi command

       if(pdata[3] == 0x01){  // link wifi 
        
         
          gpro_t.link_net_step =0;
	      net_t.wifi_link_net_success=0;
          gpro_t.wifi_led_fast_blink_flag =1;
          gctl_t.wifi_config_net_lable=wifi_set_restor;
		  wifi_t.runCommand_order_lable= wifi_link_tencent_cloud;//2 
		  
          gctl_t.gTimer_linkTencentCounter=0; //total times is 120s
          SendWifiData_Answer_Cmd(0x05,0x01); //WT.EDIT 2024.12.28
          vTaskDelay(pdMS_TO_TICKS(10));

        }
        else if(pdata[3] == 0x0){ //don't link wifi 

        }


     break;

     case 0x06: //buzzer sound command 

          buzzer_sound();
		  vTaskDelay(pdMS_TO_TICKS(5));
     break;

     case 0x16 : //buzzer sound command with answer .

        buzzer_sound();
        

		  gpro_t.answer_buzzer_flag = 1;//WT.EDIT 2025.07.28 
          SendWifiData_Answer_Cmd(0x16,0x01); //WT.EDIT 2025.07.28
		  vTaskDelay(pdMS_TO_TICKS(5));
		  
       break;


      case 0x1A: //??????

        if(pdata[3] == 0x0F){ //???

           gctl_t.set_temperature_value = pdata[5] ;
           if(wifi_link_net_state()==1){
             MqttData_Publis_SetTemp(gctl_t.set_temperature_value);
		     vTaskDelay(pdMS_TO_TICKS(200));//osDelay(200);//HAL_Delay(350);
            }

        }
      break;

      case 0x1B: //??????

        if(pdata[3] == 0x0F){ //???
            

        }
      break;

      case 0x1C: //?????????????????

        if(pdata[3] == 0x0F){ //???

          


        }
      break;

        case 0x1D: //???????? ????????

        if(pdata[3] == 0x0F){ //???

             
            

        }
      break;

     case 0x22: //PTC?????????,????????0????

      if(pdata[3] == 0x01){
        

        gctl_t.gDry = 1;
		gpro_t.ptc_switch_flag = open;
   
        if(gpro_t.stopTwoHours_flag ==0){
              PTC_SetHigh();
              gpro_t.ptc_switch_flag =open;
              gctl_t.gTimer_senddata_panel=7;
          }
          
      }
      else if(pdata[3] == 0x0){
        
         gctl_t.gDry =0;
         PTC_SetLow();
          gpro_t.ptc_switch_flag =close;
          gctl_t.gTimer_senddata_panel=7;
      }

     break;

     case 0x27:

      if(pdata[3] == 0x02){
       
         gctl_t.gModel=2;
         //SendWifiData_Answer_Cmd(0x27,0x02); //don't AI mode,   WT.EDIT 2025.01.06
          if(wifi_link_net_state()==1){
            MqttData_Publish_SetState(2);
	        vTaskDelay(pdMS_TO_TICKS(200));//osDelay(100);//HAL_Delay(350);
           }
        
          
       }
       else if(pdata[3] == 0x01){ //AI mode 
       
         gctl_t.gModel=1;
        // SendWifiData_Answer_Cmd(0x27,0x01); //AI mode,WT.EDIT 2025.01.06
         if(wifi_link_net_state()==1){
             MqttData_Publish_SetState(1);
    	     vTaskDelay(pdMS_TO_TICKS(200));//osDelay(100);//HAL_Delay(350);
         }
       }


     break;

     case 0xFF: //copy send cmd acknowlege
     //power on or power off 
        if(pdata[3]==0x31){ //smart phone normal :power on
            if(pdata[4]==1){ //power on

                gpro_t.receive_copy_cmd = ack_app_power_on;

            }
            else if(pdata[4]==2){ //smart phone normal :power off
               gpro_t.receive_copy_cmd = ack_app_power_off;
            }

        }
        else if(pdata[3]==0x21){ //smart phone of App timer power on .

            if(pdata[4]==1){ //power on

                gpro_t.receive_copy_cmd = ack_app_timer_power_on;

            }
            else if(pdata[4]==2){ //smart phone normal :power off
               gpro_t.receive_copy_cmd = ack_app_power_off;
            }


        }
        else if(pdata[3] == 0x05){ //link wifi command copy command..

        if(pdata[4]==1){

            gpro_t.receive_copy_cmd = ack_wifi_on;

        }


        }

      break;
        
     
     }

   }
  }
}


/**********************************************************************
*
*Function Name:void send_cmd_ack_hanlder(void);
*Function: 
*Input Ref:NO
*Return Ref:NO
*
**********************************************************************/
void send_cmd_ack_hanlder(void)
{

    
    switch(gpro_t.send_ack_cmd){
    
        case ack_null:
    
    
        break;
    
        case ack_app_power_on:
            
          if(gpro_t.receive_copy_cmd == ack_app_power_on){
             gpro_t.receive_copy_cmd =0;
             gpro_t.send_ack_cmd = 0;
            
          }
          else if(gpro_t.receive_copy_cmd != 0 && gpro_t.gTimer_again_send_power_on_off >1){
             gpro_t.gTimer_again_send_power_on_off =0;
              SendWifiData_To_Cmd(0x31,0x01); //smart phone is power on
              vTaskDelay(pdMS_TO_TICKS(10));
          }
                    
         
        break;
    
        case ack_app_power_off :
    
         if(gpro_t.receive_copy_cmd == ack_app_power_off){
            gpro_t.receive_copy_cmd =0;
             gpro_t.send_ack_cmd = 0;
             
          }
          else if(gpro_t.receive_copy_cmd != 0 && gpro_t.gTimer_again_send_power_on_off >1){
              gpro_t.gTimer_again_send_power_on_off =0;
               SendWifiData_To_Cmd(0x31,0x0); //smart phone is power off
               vTaskDelay(pdMS_TO_TICKS(10));
          }
    
    
        break;

        case ack_app_timer_power_on:

           if(gpro_t.receive_copy_cmd == ack_app_timer_power_on){
             gpro_t.receive_copy_cmd =0;
             gpro_t.send_ack_cmd = 0;
            
          }
          else if(gpro_t.receive_copy_cmd != 0 && gpro_t.gTimer_again_send_power_on_off >1){
             gpro_t.gTimer_again_send_power_on_off =0;
              SendWifiData_To_Cmd(0x21,0x01); //smart phone is power on
              vTaskDelay(pdMS_TO_TICKS(10));
          }

        break;
    
        case ack_wifi_on:
    
          if(gpro_t.receive_copy_cmd == ack_wifi_on){
             gpro_t.receive_copy_cmd =0;
             gpro_t.send_ack_cmd = 0;
            
          }
          else if(gpro_t.receive_copy_cmd != 0 && gpro_t.gTimer_again_send_power_on_off >1){
             gpro_t.gTimer_again_send_power_on_off =0;
             SendWifiData_To_Data(0x1F,0x0);
		     vTaskDelay(pdMS_TO_TICKS(10));

			 
          }
    
    
        break;
    
        case ack_ptc_on:
    
    
        break;
    
        case ack_ptc_off:
    
        break;
    
    
        }
 

}


