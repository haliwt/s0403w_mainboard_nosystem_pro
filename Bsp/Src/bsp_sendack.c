#include "bsp.h"


uint8_t power_on_sound_flag;


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
             power_on_sound_flag=1;
             gpro_t.power_on_prority_flag =1;

        }
        else if(pdata[3] == 0x0){ //close 


		     if(power_on_sound_flag==1){
			  	power_on_sound_flag++;
			    vTaskDelay(pdMS_TO_TICKS(30));
                 buzzer_sound();
			  }
              
			  SendWifiData_Answer_Cmd(0x01,0x02); //power off .
              vTaskDelay(pdMS_TO_TICKS(10)); 
             
              
             powerOffTunrOff_flag=1;
             gpro_t.gpower_on = power_off;
			 
			
        }

     break;

     case 0x02: //PTC?????????

     if(pdata[3] == 0x01){
          buzzer_sound();
         dry_open_flag=1;//gctl_t.gDry = 1;
       

      if(gpro_t.stopTwoHours_flag==0){
           PTC_SetHigh();
          if(wifi_link_net_state()==1){
              MqttData_Publish_SetPtc(0x01);
	  	      osDelay(100);//HAL_Delay(350);
           }
       
       }
       }
       else if(pdata[3] == 0x0){
        buzzer_sound();
          dry_open_flag=0;//gctl_t.gDry =0;
        
         PTC_SetLow();
         if(wifi_link_net_state()==1){
              MqttData_Publish_SetPtc(0x0);
	  	      osDelay(100);//HAL_Delay(350);
          }

       }

     break;

     case 0x03: //PLASMA ?????????

        if(pdata[3] == 0x01){
           
            buzzer_sound();
           
           //gctl_t.gPlasma = 1;
           plasma_open_flag=1;
          
           PLASMA_SetHigh();
        }
        else if(pdata[3] == 0x0){
           buzzer_sound();
           
         // gctl_t.gPlasma = 0;
           plasma_open_flag=0;
        
          PLASMA_SetLow();

        }


     break;


      case 0x04: //ultrasonic  ?????????

        if(pdata[3] == 0x01){  //open 
          
           ultrasonic_open_flag=1;//gctl_t.gUlransonic =1;

        }
        else if(pdata[3] == 0x0){ //close 

          ultrasonic_open_flag=0; //gctl_t.gUlransonic = 0;

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

        }
        else if(pdata[3] == 0x0){ //don't link wifi 

        }


     break;

     case 0x06: //buzzer sound command 

           power_on_sound_flag=3;
           gpro_t.stop_run_wifi_pro =1; //stop wifi process
           
            buzzer_sound();
             gpro_t.stop_run_wifi_pro =1; //stop wifi process
			//gpro_t.answer_buzzer_flag = 1;//WT.EDIT 2025.07.30
           // pdata[2] =0xff;
          
           

       


     break;

     case 0x16 : //buzzer sound command with answer .

        gpro_t.stop_run_wifi_pro =1; //stop wifi process
      
		   buzzer_sound();
         gpro_t.stop_run_wifi_pro =1; //stop wifi process

		  gpro_t.answer_buzzer_flag = 1;//WT.EDIT 2025.07.28 
          SendWifiData_Answer_Cmd(0x16,0x01); //WT.EDIT 2025.07.28
		  vTaskDelay(pdMS_TO_TICKS(5));
		  
         
          // pdata[2] =0xff;

            
           
 
        
      


     break;


      case 0x1A: //??????

        if(pdata[3] == 0x0F){ //???

           gctl_t.set_temperature_value = pdata[5] ;
           if(wifi_link_net_state()==1){
             MqttData_Publis_SetTemp(gctl_t.set_temperature_value);
		     osDelay(200);//HAL_Delay(350);
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
        

         dry_open_flag=1;//gctl_t.gDry = 1;
   
        if(gpro_t.stopTwoHours_flag ==1){
              PTC_SetHigh();
             if(wifi_link_net_state()==1){
                  MqttData_Publish_SetPtc(0x01);
    	  	      osDelay(100);//HAL_Delay(350);
               }
          }
          
      }
      else if(pdata[3] == 0x0){
        
         dry_open_flag=0;//gctl_t.gDry =0;
        
      if(gpro_t.stopTwoHours_flag ==0){
        PTC_SetLow();
          if(wifi_link_net_state()==1){
              MqttData_Publish_SetPtc(0x0);
	  	      osDelay(100);//HAL_Delay(350);
           }

       }
      }

     break;

     case 0x27:

      if(pdata[3] == 0x02){
       
         gctl_t.gModel=2;
         //SendWifiData_Answer_Cmd(0x27,0x02); //don't AI mode,   WT.EDIT 2025.01.06
          if(wifi_link_net_state()==1){
            MqttData_Publish_SetState(2);
	        osDelay(100);//HAL_Delay(350);
           }
        
          
       }
       else if(pdata[3] == 0x01){ //AI mode 
       
         gctl_t.gModel=1;
        // SendWifiData_Answer_Cmd(0x27,0x01); //AI mode,WT.EDIT 2025.01.06
         if(wifi_link_net_state()==1){
             MqttData_Publish_SetState(1);
    	     osDelay(100);//HAL_Delay(350);
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
          }
    
    
        break;
    
        case ack_ptc_on:
    
    
        break;
    
        case ack_ptc_off:
    
        break;
    
    
        }


}


