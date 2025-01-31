#include "run.h"
#include "wifi_fun.h"
#include "dht11.h"
#include "fan.h"
#include "tim.h"
#include "cmd_link.h"
#include "special_power.h"
#include "buzzer.h"
#include "esp8266.h"
#include "mqtt_iot.h"
#include "publish.h"
#include "flash.h"
#include "usart.h"
#include "subscription.h"
#include "adc.h"


RUN_T run_t; 

static void Single_Power_ReceiveCmd(uint8_t cmd);
static void Single_Command_ReceiveCmd(uint8_t cmd); 

static void power_on_handler(void);

uint8_t power_on_enable_flag;


uint8_t tencent_cloud_flag;//,dc_power_on_flag;
uint8_t power_on_process_step;
uint8_t  power_off_step;
uint8_t run_main_process_flag;


void run_init(void)
{
   power_on_process_step = 1;
   power_off_step=2;
   power_on_enable_flag = POWER_OFF;

   

}
/**********************************************************************
*
*Function Name:void Decode_RunCmd(void)
*Function: receive usart touchkey of command 
*Input Ref:NO
*Return Ref:NO
*
**********************************************************************/
void Decode_RunCmd(void)
{

 uint8_t cmdType_1=inputCmd[0],cmdType_2 = inputCmd[1];
 uint32_t temp;

    
  switch(cmdType_1){
  
      case 'P': //power on and off
        
          
          Single_Power_ReceiveCmd(cmdType_2);  
           
      break;
      

	  case 'W': //wifi-linking_tencentcloud
	      if(run_t.gPower_flag==POWER_ON){
	      if(cmdType_2==1 && run_t.wifi_run_set_restart_flag ==0){
              tencent_cloud_flag=0;
              Buzzer_KeySound();
			  SendWifiData_To_Cmd(0x52); //0x52= 'R'
		      esp8266data.esp8266_login_cloud_success=0;
	          run_t.wifi_config_net_lable=wifi_set_restor;
			  run_t.gTimer_linking_tencen_counter=0;
              run_t.wifi_run_set_restart_flag =1;
	          wifi_t.runCommand_order_lable= wifi_link_tencent_cloud;//2 
	          
             run_t.decodeFlag =0;
          }
		  else if(cmdType_2==0){
                
                Buzzer_KeySound();
                
              run_t.decodeFlag =0;
		   }
		   else if(cmdType_2==0x14){
                run_t.gModel =2; //Timer timing of model
                
                Buzzer_KeySound();
           
                run_t.decodeFlag =0;
		        MqttData_Publish_SetState(0x0); //Ai model->timer_time
		        HAL_Delay(200);
                
            }
            else if(cmdType_2==0x04){
                run_t.gModel =1;  // AI model 
                Buzzer_KeySound();
            
                run_t.decodeFlag =0;
			    MqttData_Publish_SetState(0x1); //Ai model->beijing_time
			    HAL_Delay(200);
            }
           
           
        }
       
	   break;
        
      case 'C':
           if(run_t.gPower_flag==POWER_ON){
            
              run_t.decodeFlag =0;
              Single_Command_ReceiveCmd(cmdType_2); //Single_ReceiveCmd(cmdType_2); 
              
           }
     
         
      break;

	  case 'M':
	  	if(run_t.gPower_flag==POWER_ON){
             
             run_t.decodeFlag =0;
           
             run_t.set_temperature_value = cmdType_2;
			 if(esp8266data.esp8266_login_cloud_success==1)
			       MqttData_Publis_SetTemp(run_t.set_temperature_value);
			 	   HAL_Delay(200);
			  
         }
	  

	  break;

	  case 'T':
		  if(run_t.gPower_flag==POWER_ON){
             #if 0
             run_t.set_temperature_value = cmdType_2;
			 if(esp8266data.esp8266_login_cloud_success==1)
				 MqttData_Publis_SetTime(run_t.set_temperature_value);
			#endif    
         }
	  

	  break;


	  case 'Z' ://buzzer sound 
	    if(run_t.gPower_flag==POWER_ON){

            

		    if(cmdType_2== 'Z'){//turn off AI
		     
		       Buzzer_KeySound();
               
               run_t.decodeFlag =0;

                
			}
			 
		
		}
     
	    break;
 	}
    
}
/**********************************************************************
	*
	*Functin Name: void Single_ReceiveCmd(uint8_t cmd)
	*Function : resolver is by usart port receive data  from display panle  
	*Input Ref:  usart receive data
	*Return Ref: NO
	*
**********************************************************************/
static void Single_Power_ReceiveCmd(uint8_t cmd)
{
    static uint8_t buzzer_sound,buzzer_power_Off_sound;
    

    
    switch(cmd){

    case 0x01: // power on

        buzzer_power_Off_sound=1;
        if(buzzer_sound==0){ 
            buzzer_sound++;

            Buzzer_KeySound();
        }

        run_t.decodeFlag =0;

        SendWifiData_To_Cmd(0x54); //0x52= 'R'
        PTC_SetHigh();
        Update_DHT11_Value(); 

        power_on_enable_flag=POWER_ON;

        run_t.gTimer_send_dit=60;
	 
	cmd=0xff;  
     break;


	

    case 0x00: //power off

         buzzer_sound=0;
       if(buzzer_power_Off_sound==1){
            buzzer_power_Off_sound++;
           Buzzer_KeySound();
           //HAL_Delay(100);

        }
        run_t.decodeFlag =0;

        
       
       // SendWifiData_To_Cmd(0x53); //0x52= 'R' WT.EDIT 2024.12.25
        PTC_SetLow();
      
        power_on_enable_flag=POWER_OFF;
       
        
        cmd = 0xff;
    break;
         
    case 0xAA: //power_on 
//      if(run_t.gPower_flag == POWER_ON){
//       run_t.works_break_power_on = 0;
//
//      	}
	   
    break;

	case 0x55: //power off
//	   if(run_t.gPower_flag == POWER_ON){
//		PTC_SetLow();
//		PLASMA_SetLow();
//		HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);//ultrasnoic off	
//        run_t.gFan_continueRun=1; 
//		run_t.gFan_counter=0;
//		run_t.works_break_power_on = 1;
//	   	}

	break;

   

    

     default:

     break;

    }

}
/**********************************************************************
	*
	*Functin Name: void Single_ReceiveCmd(uint8_t cmd)
	*Function : resolver is by usart port receive data  from display panle  
	*Input Ref:  usart receive data
	*Return Ref: NO
	*
**********************************************************************/
static void Single_Command_ReceiveCmd(uint8_t cmd)
{
    static uint8_t no_buzzer_sound_dry_off;
	switch(cmd){

	    case DRY_ON_NO_BUZZER:

	        run_t.noBuzzer_sound_dry_flag =1;

       case DRY_ON:
         run_t.gDry = 1;
	     run_t.gFan_continueRun =0;
	   if(run_t.noBuzzer_sound_dry_flag !=1){
		     Buzzer_KeySound();
		 }
		if(esp8266data.esp8266_login_cloud_success==1)
		 MqttData_Publish_SetPtc(0x01);
		 HAL_Delay(200);
		 
       break;

	   case DRY_OFF_NO_BUZZER :

	         no_buzzer_sound_dry_off=1;

	  case DRY_OFF:
 			run_t.gDry = 0;
			if( no_buzzer_sound_dry_off !=1)
			    Buzzer_KeySound();
			 if(run_t.gPlasma ==0){ //plasma turn off flag
			  run_t.gFan_counter =0;
			   run_t.gFan_continueRun =1;

		     }
			if(esp8266data.esp8266_login_cloud_success==1)
			MqttData_Publish_SetPtc(0x0);
			HAL_Delay(200);
			
       break;

       case PLASMA_ON:
       		run_t.gPlasma=1;
       		run_t.gUlransonic =1;
	    Buzzer_KeySound();
	   if(esp8266data.esp8266_login_cloud_success==1){
	        MqttData_Publish_SetPlasma(1) ;//杀菌
	        HAL_Delay(200);
	        MqttData_Publish_SetUltrasonic(1); //超声波
	        HAL_Delay(200);
	   	}
	   
       break;

       case PLASMA_OFF:
           run_t.gPlasma=0;
           run_t.gUlransonic =0;
	    Buzzer_KeySound();
	   if(esp8266data.esp8266_login_cloud_success==1){
	       MqttData_Publish_SetPlasma(0) ;//杀菌
	        HAL_Delay(200);
	        MqttData_Publish_SetUltrasonic(0); //超声波
	        HAL_Delay(200);
	   	}
	   
       break;

	   case MODE_AI_NO_BUZZER :
	   	  run_t.gModel =1;  //AI_Works_Model 
		  MqttData_Publish_SetState(0x1); //Ai model->beijing_time
		  HAL_Delay(200);

	   break;

	   case WIFI_CONNECT_FAIL:

	       run_t.dp_link_wifi_fail =1;


	   break;

	   case DISPLAY_PANNEL_CONNECT_WIFI_SUCCESS:

	      
	        run_t.dp_link_wifi_fail =0;


	   break;




      default :
        cmd =0;

      break; 


    }



}
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
	*Functin Name: void  RunCommand_MainBoard_Fun(void)
	*Function : be check key of value 
	*Input Ref:  key of value
	*Return Ref: NO
	*
**********************************************************************/
void RunCommand_MainBoard_Fun(void)
{
   uint8_t i;
   static uint8_t tm,power_on_flag;//dc_power_on_flag;
  
    if(run_t.buzzer_sound_flag == 1){
	 	run_t.buzzer_sound_flag = 0;
	    Buzzer_KeySound();

	 }
    
  
   switch(run_main_process_flag){

	case POWER_ON: //1
	     
		 SetPowerOn_ForDoing();
	     run_t.gPower_On=POWER_ON;
         run_t.gTimer_send_dit=0;
	     run_t.gTimer_senddata_panel=0;
		 run_t.gTimer_app_power_on=0;
		 run_t.app_timer_power_off_flag =0;
		 run_t.gTimer_continuce_works_time=0;
		 //error detected times 
		 run_t.ptc_warning =0;
		 run_t.fan_warning =0;
		 run_t.gTimer_ptc_adc_times=0;
		 run_t.gTimer_fan_adc_times=0;
		 run_t.ptc_first_detected_times=0;
		 
	    power_on_flag=0;
        run_t.gFan_continueRun =1; //WT.EDIT .2024.12.23
	   run_main_process_flag= UPDATE_TO_PANEL_DATA;
	   
    
	break;

     case UPDATE_TO_PANEL_DATA: //05

      if(power_on_flag ==0 && esp8266data.esp8266_login_cloud_success==1){
	  	power_on_flag++;
    	Subscriber_Data_FromCloud_Handler();
		HAL_Delay(350);

      	}

     switch(run_t.interval_time_stop_run){

	 case 0: //works timing 
	  if(run_t.gTimer_senddata_panel >50 ){ //300ms
	   	    run_t.gTimer_senddata_panel=0;
	        ActionEvent_Handler();
	    }

	 if(run_t.gTimer_ptc_adc_times > 28 ){ //65s//3 minutes 120s
         run_t.gTimer_ptc_adc_times=0;
		 
		 Get_PTC_Temperature_Voltage(ADC_CHANNEL_1,10);
	     

	 }
     else if(run_t.gTimer_fan_adc_times > 45){ //2 minute 180s
	     run_t.gTimer_fan_adc_times =0;
	     Get_Fan_Adc_Fun(ADC_CHANNEL_0,10);
	     

		 
	 }
	 else if(run_t.gTimer_send_dit > 49){
	
			 run_t.gTimer_send_dit=0;
			 Update_DHT11_Value();
		
	}
	 
	if(run_t.gTimer_app_power_on >37 &&	 run_t.app_timer_power_on_flag == 1){
		run_t.gTimer_app_power_on=0;
		run_t.app_timer_power_on_flag++;
		   for(i=0;i<36;i++){
				 TCMQTTRCVPUB[i]=0;
			 }
		}
	
       if(run_t.first_link_tencent_cloud_flag ==1){
	
		  run_t.first_link_tencent_cloud_flag++;
			MqttData_Publish_SetOpen(0x01);
			HAL_Delay(100);
			Publish_Data_ToTencent_Initial_Data();
			HAL_Delay(100);
	
		   Subscriber_Data_FromCloud_Handler();
		   HAL_Delay(100);
	   }
    

	if(run_t.gTimer_continuce_works_time > 7200){//if(run_t.gTimer_continuce_works_time > 600){
	
	     run_t.gTimer_continuce_works_time =0;
         run_t.interval_time_stop_run =1;
	     run_t.gFan_continueRun =1;
		 run_t.gFan_counter=0;
         run_t.fan_first_run_flag++;   //WT.EDIT 2024.12.23
    }
    
	 break;

	 case 1:  //interval times 10 minutes,stop works
	 	
		PLASMA_SetLow(); //
		HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);//ultrasnoic Off 
		PTC_SetLow();

	
	  if(run_t.gTimer_continuce_works_time > 600){
             run_t.gTimer_continuce_works_time=0;
		    run_t.interval_time_stop_run =0;
      }

	 if(run_t.gFan_continueRun ==1){

	      if(run_t.gFan_counter < 60){
	  
	              Fan_One_Power_Off_Speed();//Fan_RunSpeed_Fun();// FAN_CCW_RUN();
	          }       

	       if(run_t.gFan_counter > 59){
	           
			   run_t.gFan_counter=0;
			
			   run_t.gFan_continueRun=0;
			   FAN_Stop();
	       }

	  }
	 

	 break;

     }
     
    break;

    }
}


void power_off_handler(void)
{
     uint8_t i;
      switch(run_main_process_flag){

      case POWER_OFF: //2
     
        run_t.gPower_On=POWER_OFF;
        run_t.gPower_flag = POWER_OFF;
       
		 run_t.set_wind_speed_value=10;
		 run_t.gModel =1;
		run_t.app_timer_power_on_flag =0;
		
		run_t.interval_time_stop_run =0;
		run_t.gTimer_continuce_works_time=0;
		wifi_t.gTimer_subscriber_send=0;

		  run_t.ptc_warning =0;
		 run_t.fan_warning =0;
		 run_t.gTimer_ptc_adc_times=0;
		 run_t.gTimer_fan_adc_times=0;
		 run_t.ptc_first_detected_times=0;
		
		
       
         run_t.gFan_counter=0;
	     run_main_process_flag = FAN_CONTINUCE_RUN_ONE_MINUTE;  //FAN_CONTINUCE_RUN_ONE_MINUTE: //7

		
		 
         
       if(run_t.app_timer_power_off_flag==1){ 
         	run_t.app_timer_power_off_flag=0;
		      for(i=0;i<36;i++){
		      TCMQTTRCVPUB[i]=0;
		     }

		}

	     //SetPowerOff_ForDoing();

    run_t.gPower_On = POWER_OFF;
	run_t.wifi_gPower_On = 0;
    run_t.set_wind_speed_value =10;
 
    run_t.gFan = 0;
    run_t.gDry = 0;
	run_t.gPlasma =0;       //"杀菌"
	run_t.gUlransonic = 0; // "驱虫"
	run_t.gModel =1;
    run_t.gFan_counter=0;

    
	PLASMA_SetLow(); //
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);//ultrasnoic Off 
	PTC_SetLow();
	  
     run_main_process_flag = FAN_CONTINUCE_RUN_ONE_MINUTE;  //FAN_CONTINUCE_RUN_ONE_MINUTE: //7

     if(esp8266data.esp8266_login_cloud_success==1){
        MqttData_Publish_PowerOff_Ref(); 
	    HAL_Delay(200);
        

        if( run_t.ptc_remove_warning_send_data ==0){
		 	run_t.ptc_remove_warning_send_data++;
		  	Publish_Data_Warning(ptc_temp_warning,0);
		  	HAL_Delay(200);
			Publish_Data_Warning(fan_warning,0);
			HAL_Delay(200);
			
            }
            Subscriber_Data_FromCloud_Handler();
		    HAL_Delay(200);
        }
       
	break;


  

	case FAN_CONTINUCE_RUN_ONE_MINUTE: //7

     if(run_t.gFan_continueRun ==1){ //WT.EIDT 2024.12.18//run_t.app_timer_power_off_flag ==0){
		 if(run_t.gFan_counter < 60){
          
                   // Fan_One_Speed();
			Fan_One_Power_Off_Speed();
                  
        } 
       }

      if(run_t.gFan_counter > 60){
        
		           
		     run_t.gFan_counter=0;
		    // run_main_process_flag = POWER_NULL;
			      
		     FAN_Stop();
             run_t.gFan_continueRun =2; //WT.EDIT 2024.12.24
                  
				  
	       
	  
     }
	
	break;

	

    default:

    break;



   }
	
}
/**********************************************************************
	*
	*Functin Name: void MainBoard_Itself_PowerOn_Fun(void)
	*Function :
	*Input Ref:  key of value
	*Return Ref: NO
	*
**********************************************************************/
void MainBoard_Self_Inspection_PowerOn_Fun(void)
{
    static uint8_t self_power_on_flag=0,send_power_off_flag=0;
    

	if(self_power_on_flag==0){
        self_power_on_flag ++ ;
	
      //  Buzzer_KeySound();

    
		InitWifiModule_Hardware();//InitWifiModule();
		HAL_Delay(1000);
        Buzzer_KeySound();
        SmartPhone_TryToLink_TencentCloud();
         run_t.gTimer_ptc_adc_times=0;
		if(esp8266data.esp8266_login_cloud_success==1){
			
			
			wifi_t.runCommand_order_lable= wifi_tencent_subscription_data;//04
	

			SendWifiData_To_Cmd(0x01) ;
            HAL_Delay(5);
			

			
		}
       run_t.gTimer_ptc_adc_times=0;
    }

	 if(esp8266data.esp8266_login_cloud_success==1 && run_t.gPower_On  !=POWER_ON ){
       
           if(send_power_off_flag==0){
            send_power_off_flag++;
		    //run_main_process_flag=POWER_OFF;
		    power_on_enable_flag= POWER_OFF;
			//wifi_t.runCommand_order_lable = wifi_publish_update_tencent_cloud_data
			SendWifiData_To_Cmd(0x01) ;
			HAL_Delay(50);
               
           }
   			
	}
    
   
}


/**************************************************************
	*
	*Function Name: void RunCommand_Connect_Handler(void)
	*Function: power on and run command
	*
	*
	*
**************************************************************/
void RunCommand_Connect_Handler(void)
{
   uint8_t i;
   if(power_on_enable_flag == POWER_ON){
       power_off_step = 2;
       
       power_on_handler();
       


   }
   else{
      power_on_process_step=01;

     switch(power_off_step){
      case POWER_OFF:
		  PTC_SetLow();
           run_t.wifi_run_set_restart_flag =0;
          run_t.ptc_remove_warning_send_data =0;
	      run_t.gPower_On=POWER_OFF;
          run_t.gPower_flag = POWER_OFF;
        
		 run_t.set_wind_speed_value=10;
		 run_t.gModel =1;
	
          power_off_step= POWER_OFF_RUN_FAN;
         run_main_process_flag = POWER_OFF;
        
       break;

       case POWER_OFF_RUN_FAN:
            //power_off_handler();
               run_t.gPower_On=POWER_OFF;
               run_t.gPower_flag = POWER_OFF;
              
                run_t.set_wind_speed_value=10;
                run_t.gModel =1;
               run_t.app_timer_power_on_flag =0;
               
               run_t.interval_time_stop_run =0;
               run_t.gTimer_continuce_works_time=0;
               wifi_t.gTimer_subscriber_send=0;
        
                 run_t.ptc_warning =0;
                run_t.fan_warning =0;
                run_t.gTimer_ptc_adc_times=0;
                run_t.gTimer_fan_adc_times=0;
                run_t.ptc_first_detected_times=0;
               
               
              
                run_t.gFan_counter=0;
                power_off_step = FAN_CONTINUCE_RUN_ONE_MINUTE;  //FAN_CONTINUCE_RUN_ONE_MINUTE: //7
        
               
                
                
              if(run_t.app_timer_power_off_flag==1){ 
                   run_t.app_timer_power_off_flag=0;
                     for(i=0;i<36;i++){
                     TCMQTTRCVPUB[i]=0;
                    }
        
               }
        
                //SetPowerOff_ForDoing();
        
           run_t.gPower_On = POWER_OFF;
           run_t.wifi_gPower_On = 0;
           run_t.set_wind_speed_value =10;
        
           run_t.gFan = 0;
           run_t.gDry = 0;
           run_t.gPlasma =0;       //"杀菌"
           run_t.gUlransonic = 0; // "驱虫"
           run_t.gModel =1;
           run_t.gFan_counter=0;
        
           
           PLASMA_SetLow(); //
           HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);//ultrasnoic Off 
           PTC_SetLow();
             
           
        
            if(esp8266data.esp8266_login_cloud_success==1){
               MqttData_Publish_PowerOff_Ref(); 
               HAL_Delay(200);
               
        
               if( run_t.ptc_remove_warning_send_data ==0){
                   run_t.ptc_remove_warning_send_data++;
                   Publish_Data_Warning(ptc_temp_warning,0);
                   HAL_Delay(200);
                   Publish_Data_Warning(fan_warning,0);
                   HAL_Delay(200);
                   
                   }
                   Subscriber_Data_FromCloud_Handler();
                   HAL_Delay(200);
               }
              
           break;
        
        
         
        
           case FAN_CONTINUCE_RUN_ONE_MINUTE: //7
        
            if(run_t.gFan_continueRun ==1){ //WT.EIDT 2024.12.18//run_t.app_timer_power_off_flag ==0){
                if(run_t.gFan_counter < 60){
                 
                          // Fan_One_Speed();
                   Fan_One_Power_Off_Speed();
                         
               } 
              }
        
             if(run_t.gFan_counter > 60){
               
                          
                    run_t.gFan_counter=0;
                   // run_main_process_flag = POWER_NULL;
                         
                    FAN_Stop();
                    run_t.gFan_continueRun =2; //WT.EDIT 2024.12.24
                         
                         
                  
             
            }
           
           break;
        
           

        }



   }

}

static  void power_on_handler(void)
{


    switch(power_on_process_step){

        case POWER_ON:
          run_t.fan_first_run_flag++;   //WT.EDIT 2024.12.23
		//  fan_first_run_fun() ; //WT.EDIT,2024.12.23 AT ONCE OPEN FAN 
		  Fan_One_Speed();
          PTC_SetHigh();
          run_t.gPower_flag = POWER_ON;
		 run_t.gPower_On = POWER_ON;
         run_main_process_flag= POWER_ON;
		 run_t.gModel =1;
		 run_t.set_wind_speed_value=100;
         run_t.wifi_run_set_restart_flag =0;

		 //error detected times 
		 run_t.ptc_warning =0;
		 run_t.fan_warning =0;
		 run_t.gTimer_ptc_adc_times=0;
		 run_t.gTimer_fan_adc_times=0;
		 run_t.ptc_first_detected_times=0;
      
         if( run_t.decodeFlag ==0){
		 Update_DHT11_Value();
		 HAL_Delay(10);
		 if(esp8266data.esp8266_login_cloud_success==1){
		 	 run_t.gUlransonic =1;
			 run_t.gPlasma =1;
		     run_t.gDry =1;
			 run_t.set_wind_speed_value=100;
             run_t.wifi_gPower_On=1;
	        run_t.ptc_remove_warning_send_data =0;

			 MqttData_Publish_SetOpen(1);  
				HAL_Delay(200);
			 MqttData_Publish_Update_Data();//MqttData_Publish_SetOpen(1);  //MqttData_Publish_SetOpen(0x01);
	         HAL_Delay(350);
	      
		 }
        
        
		 power_on_process_step =RUN_COMMAND ;//KEY_NULL;
		  run_main_process_flag= POWER_ON;
         }
	    break;


        
         case RUN_COMMAND: //0x03
            if( run_t.decodeFlag ==0){
              RunCommand_MainBoard_Fun();
  
           }
  
  
        break;
  }
}

	   

       

	  




    
