#include "bsp.h"


static void power_off_stop_fun(void);

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

  // static uint8_t send_dht11;
    switch(gpro_t.process_run_step){

	case 0: //1

          /*power on initial reference---start */
         gctl_t.gTimer_senddata_panel=0; //main board function run action.
         gpro_t.fan_run_initial_times = 0; //WT.EDIT 2025.07.31
         gctl_t.set_temperature_value=40; //power on default set temperature value is 40 degree,don't compare
         gctl_t.gTimer_ptc_adc_times=0;
		 gctl_t.gTimer_fan_adc_times=0;
		
		 gctl_t.set_wind_speed_value= 100;
		
		 gctl_t.first_link_tencent_cloud_flag=1;
		 gctl_t.ptc_on_off_flag =0; //WT.EDIT 2025.09.18
		 gctl_t.set_temperature_flag=0;
		  gctl_t.set_temp_first_closeptc =0;
		  gctl_t.rx_set_temp_flag =0;

		 /*end*/
         
	     /*clear error detected flag --start*/
		
		 gctl_t.ptc_warning =0;
		 gctl_t.ptc_warning =0;
	     gpro_t.fan_warning_flag =0;
	
		 gpro_t.gTimer_detect_fan_error=0;
		/*end*/
       
	
        /*POWER OFF REF-start */
        powerOffTunrOff_flag = 1;
        powerOffFanRun_flag =1;
        /*end*/
		
       
        /*this works two hours reference start -WT.EDIT 2025.08.11*/
	    gpro_t.gTimer_check_twohours=0;
	    counter_two_hours=0;
    
		 gpro_t.stopTwoHours_flag =0;
		/*end */
       
     
         updateDht11_sensorData_toDisp();
		 gpro_t.process_run_step= 1;
	break; 


  case 1:
  	if(gctl_t.app_timer_power_on_flag > 1)gctl_t.app_timer_power_on_flag=0;
    if(gctl_t.app_timer_power_on_flag ==1){
     smartphone_timer_power_on_and_normal_handler();
    }
	else 
		every_power_on_run();
		 
	 gpro_t.process_run_step= 2;
	   
    break;
	
	
	case 2:
		
    case 3:
	
         if(wifi_link_net_state() ==1 ){
    
		    
	         gctl_t.set_wind_speed_value =100;

		     MqttData_Publish_SetOpen(1);  
			 //vTaskDelay(pdMS_TO_TICKS(200));
		
		 }
	
      gpro_t.process_run_step= 4;
	break;

  case 4: //5

     if(wifi_link_net_state() ==1 &&  gctl_t.app_timer_power_on_flag==0){
    
		  MqttData_Publish_Init();
		 // vTaskDelay(pdMS_TO_TICKS(200));
     	}
	  counter_two_hours=0;
	  gpro_t.gTimer_check_twohours=0;
	  gpro_t.process_run_step= 6;
	 
	 break;
	     
		
    case 6:


     
	  if(gpro_t.gTimer_update_todisplay > 2){
			gpro_t.gTimer_update_todisplay=0;

			updateDht11_sensorData_toDisp();
	
		           
       }
       
       gpro_t.process_run_step=7 ;


 break;
        
  case 7: 

   if(gpro_t.wifi_led_fast_blink_flag==0 && net_t.wifi_link_net_success ==1){
      if( gctl_t.app_timer_power_on_flag==0 && gctl_t.first_link_tencent_cloud_flag ==1){
	
		  gctl_t.first_link_tencent_cloud_flag++;

               MqttData_Publish_Update_Data();
			  // vTaskDelay(pdMS_TO_TICKS(200));//HAL_Delay(200);
             SendWifiData_To_Cmd(0x1F,0x01); //link wifi order 1 --link wifi net is success.
             vTaskDelay(pdMS_TO_TICKS(5));
          
    	}
	    else if(gctl_t.first_link_tencent_cloud_flag < 4){
			 gctl_t.first_link_tencent_cloud_flag++;

            Subscriber_Data_FromCloud_Handler();
    	  
	    }
		SendWifiData_To_Data(0x1F,0x01);
		 //SendWifiData_To_Cmd(0x1F,0x01); //link wifi order 1 --link wifi net is success.
         vTaskDelay(pdMS_TO_TICKS(5));
	
	  }
      else if(net_t.wifi_link_net_success ==0 && gctl_t.gTimer_wifi_detected_counter >1 && gpro_t.wifi_led_fast_blink_flag==0){
	  	 gctl_t.gTimer_wifi_detected_counter=0;

	       SendWifiData_To_Data(0x1F,0x0);//SendWifiData_To_Cmd(0x1F,0x0); //link wifi order 1 --link wifi net is success.
		   vTaskDelay(pdMS_TO_TICKS(5));
			//printf("wifi is not !!!\r\n");
		    

	  }
      
	 gpro_t.process_run_step=8 ;
 break; 


 case 8:
     if(gpro_t.fan_warning_flag > 1 || gpro_t.ptc_warning  > 1 || gpro_t.stopTwoHours_flag>1 || gctl_t.gDry >1){
        if(gpro_t.fan_warning_flag > 1 ) gpro_t.fan_warning_flag = 0; //strictly forbid 
	    if(gpro_t.ptc_warning  > 1)gpro_t.ptc_warning = 0;
		if(gpro_t.stopTwoHours_flag>1)gpro_t.stopTwoHours_flag=0;
		if(gctl_t.gDry > 1) gctl_t.gDry =0;

     }
	
	 works_run_two_hours_state();

     gpro_t.process_run_step= 9;
  break;


  case 9:
  	 
       adc_detected_hundler();
    

	   if(wifi_link_net_state() ==1 && gpro_t.gTimer_update_tencet_dht11 >5){
				gpro_t.gTimer_update_tencet_dht11=0;
				Update_Dht11_Totencent_Value();
        }
      
       gpro_t.process_run_step= 10; 


  break;

  case 10:

     if(gctl_t.set_temperature_flag > 1 || gctl_t.set_temperature_value > 40 || gctl_t.ptc_on_off_flag > 1
	 	  ||gctl_t.app_timer_power_on_flag > 1 || gctl_t.set_temp_first_closeptc > 1){
	 	if(gctl_t.set_temperature_flag > 1)gctl_t.set_temperature_flag =0;
		if(gctl_t.set_temperature_value > 40 && gctl_t.set_temperature_flag ==0)gctl_t.set_temperature_value =40;
        if(gctl_t.ptc_on_off_flag > 1)gctl_t.ptc_on_off_flag =0;
		if(gctl_t.app_timer_power_on_flag > 1)gctl_t.app_timer_power_on_flag=0;
		if( gctl_t.set_temp_first_closeptc > 1)  gctl_t.set_temp_first_closeptc =0;
	 }


	 if(gctl_t.gTimer_read_dht11_counter>2 && gpro_t.stopTwoHours_flag==0){
		   gctl_t.gTimer_read_dht11_counter=0;
           set_temperature_compare_value_fun();
	   
	  }
	   gpro_t.process_run_step= 6;	

   break;

     default:
		//gpro_t.process_run_step= 1;
		break;
  }
}


/************************************************************************************
*
*Function Name: void ActionEvent_Handler(void)
*Fucntion :
*Input Ref:NO
*Return Ref:NO
*
************************************************************************************/
void ActionEvent_Handler(void)
{

   static uint8_t ptc_default =0xff,plasma_default =0xff,ultrasonic_default =0xff;
   
   if( gctl_t.gDry==1 && gctl_t.ptc_on_off_flag ==0){
	if(gpro_t.fan_warning_flag !=1 && gpro_t.ptc_warning !=1 &&  gpro_t.stopTwoHours_flag==0){ //PTC warning flag
		
		PTC_SetHigh();
		if(ptc_default!=gpro_t.ptc_switch_flag){
		   gpro_t.ptc_switch_flag++;
		   ptc_default = gpro_t.ptc_switch_flag;
		if(wifi_link_net_state()==1){ 
			MqttData_Publish_SetPtc(0x01);
			//vTaskDelay(pdMS_TO_TICKS(200));
		}
	  }
		   	
		
	  }
	}
	else{
		gctl_t.gDry =0;
	
		PTC_SetLow();
		if(ptc_default!=gpro_t.ptc_switch_flag){
			 gpro_t.ptc_switch_flag++;
			ptc_default = gpro_t.ptc_switch_flag;
			if(wifi_link_net_state()==1){ 
				MqttData_Publish_SetPtc(0x0);
				//vTaskDelay(pdMS_TO_TICKS(200));//osDelay(100);//HAL_Delay(350);
			}
		}
   }
   

   //plasma
    if(gctl_t.gPlasma == 1){
		
	     PLASMA_SetHigh();
		 if(plasma_default!=gpro_t.plasma_switch_flag){
		 	gpro_t.plasma_switch_flag++;
			plasma_default = gpro_t.plasma_switch_flag;	
		 if(wifi_link_net_state()==1){ 
		   MqttData_Publish_SetPlasma(0x01);
		 
		 }
		}
	}
	else{

		PLASMA_SetLow();
		 if(plasma_default!=gpro_t.plasma_switch_flag){
		 	gpro_t.plasma_switch_flag++;
			plasma_default = gpro_t.plasma_switch_flag;
		 if(wifi_link_net_state()==1){ 
		   MqttData_Publish_SetPlasma(0);
		
		 }
		}
	}
	//driver bug
	if(gctl_t.gUlransonic ==1){
	
	 
	//	HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);//ultrasnoic ON 
	 if(ultrasonic_default!=gpro_t.ultrasonic_switch_flag){
	 	gpro_t.ultrasonic_switch_flag++;
	   ultrasonic_default = gpro_t.ultrasonic_switch_flag;
		 if(wifi_link_net_state()==1){ 
		   MqttData_Publish_SetUltrasonic(0x01);
		
		 }
		}
	}
	else{
//	  HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);//ultrasnoic off
		if(ultrasonic_default!=gpro_t.ultrasonic_switch_flag){
			gpro_t.ultrasonic_switch_flag++;
			ultrasonic_default = gpro_t.ultrasonic_switch_flag;	
			if(wifi_link_net_state()==1){ 
			MqttData_Publish_SetUltrasonic(0);
		
			}
		}

	}

	Fan_RunSpeed_Fun();
		
 }

/************************************************************************************
************************************************************************************/
void twoHours_afterWorks_Handler(void)

{

  // static uint8_t ptc_default =1,plasma_default =1,ultrasonic_default =1;
   
   if( gctl_t.gDry==1){
	if(gpro_t.fan_warning_flag !=1 && gpro_t.ptc_warning != 1 &&  gpro_t.stopTwoHours_flag==0){ //PTC warning flag
		
		   
			PTC_SetHigh();
		
		
		}
	}
	else{
		gctl_t.gDry =0;
	
		PTC_SetLow();
		
   }
   

   //plasma
    if(gctl_t.gPlasma == 1){
		
	     PLASMA_SetHigh();
		
	}
	else{

		PLASMA_SetLow();
		
	}

	
	//driver bug
	if(gctl_t.gUlransonic ==1){
	
	  ultrasonic_open();//ultrasnoic ON 
	
	}
	else{
     ultrasonic_close();//ultrasnoic off
		

	}

	Fan_RunSpeed_Fun();
		
 }

void every_power_on_run(void)
{

   Fan_Full_Speed();//WT.EDIT 2025.01.03//Fan_RunSpeed_Fun();//FAN_CCW_RUN();
   if(gctl_t.app_timer_power_on_flag==0){
     
     // gctl_t.gModel=1;
      gctl_t.gFan = 1;
      gctl_t.gDry = 1;
	 
      //g_dry_open_flag =1;
      gctl_t.gPlasma =1;       //"é„1¤7?é‘„1¤7?"
      gctl_t.gUlransonic = 1; // "æ¤¹è¾«æ«„1¤7"
      gctl_t.gTimer_fan_run_one_minute=0;
       gpro_t.process_run_step=0;

	  gpro_t.ptc_switch_flag ++;
	  gpro_t.ultrasonic_switch_flag++;
	  gpro_t.plasma_switch_flag++;
      PLASMA_SetHigh();
      ultrasonic_open();   //ultrasnoic ON 
      PTC_SetHigh();
	  

    }
    gctl_t.gModel=1;
}

void smartphone_timer_power_on_and_normal_handler(void)
{
 

	if(gctl_t.app_timer_power_on_flag==1){
	       gctl_t.gModel =1;

          Parse_Json_Statement();
		  
           if( gctl_t.gPlasma==1){ //Anion
			

				SendWifiData_To_Cmd(0x03,0x01);
                vTaskDelay(pdMS_TO_TICKS(10));
			
			}
			else{
				gctl_t.gPlasma =0;
				SendWifiData_To_Cmd(0x03,0x0);
				vTaskDelay(pdMS_TO_TICKS(10));
			}


			if(gctl_t.gUlransonic==1){

					SendWifiData_To_Cmd(0x04,0x01);
					vTaskDelay(pdMS_TO_TICKS(10));
			}
			else {
					gctl_t.gUlransonic=0;
					SendWifiData_To_Cmd(0x04,0x0);
					vTaskDelay(pdMS_TO_TICKS(10));
			}



			if(gctl_t.gDry==1){

				SendWifiData_To_Cmd(0x02,0x01);
				vTaskDelay(pdMS_TO_TICKS(5));
			}
			else{
					gctl_t.gDry=0;
                  
					SendWifiData_To_Cmd(0x02,0x0);
					 vTaskDelay(pdMS_TO_TICKS(5));

			}

		     gctl_t.set_wind_speed_value =100;
	         
		     MqttData_Publish_Update_Data();
		    // vTaskDelay(pdMS_TO_TICKS(200));

			
	 
		}
			
}

void SetPowerOff_ForDoing(void)
{
   
     gctl_t.set_wind_speed_value =10;
 
    gctl_t.gFan = 0;
    gctl_t.gDry = 0;
  
	gctl_t.gPlasma =0;       //"é„1¤7?é‘„1¤7?"
    gctl_t.gUlransonic = 0; // "æ¤¹è¾«æ«„1¤7"
	gctl_t.gModel =1;


    
	PLASMA_SetLow(); //
    ultrasonic_close();//ultrasnoic Off 
	PTC_SetLow();
	

}






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
		  SendWifiData_Answer_Cmd(0x01,0x02); //compatible older version 
	      vTaskDelay(pdMS_TO_TICKS(10));
          gpro_t.gTimer_poweroff_fan=0;
          gctl_t.gTimer_fan_run_one_minute=0;
       
      
        gctl_t.set_wind_speed_value=10;
		gctl_t.gModel =1;
		gctl_t.app_timer_power_on_flag =0;
		
	
	  
        
	    gpro_t.stopTwoHours_flag=0;

		  gctl_t.ptc_warning =0;
		// gctl_t.fan_warning =0;
       
		 gctl_t.gTimer_ptc_adc_times=0;
		 gctl_t.gTimer_fan_adc_times=0;
         gpro_t.process_run_step=0;//gpro_t.process_run_step
          gctl_t.rx_set_temp_flag=0; 
         gctl_t.set_temperature_flag = 0; 
		 fan_detect_voltage=1000;
		

          SetPowerOff_ForDoing();
		  powerOffTunrOff_flag = 2;
       
      break;

      case 2:
        
       if(wifi_link_net_state() == 1){

          MqttData_Publish_PowerOff_Ref(); 
          //vTaskDelay(pdMS_TO_TICKS(200)); //WT.EDTI 2024.11.19 
       }
         powerOffTunrOff_flag = 4;
       break;

       case 4:

          if(gctl_t.ptc_warning == 1){
		 	
		  	Publish_Data_Warning(ptc_temp_warning,0);
		  	//vTaskDelay(pdMS_TO_TICKS(100));
            
          }
           powerOffTunrOff_flag = 5;
        break;

        case 5:
            if(gctl_t.ptc_warning == 1){
			Publish_Data_Warning(fan_warning,0);
			//vTaskDelay(pdMS_TO_TICKS(100));
			
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
      ultrasonic_close();// HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);//ultrasnoic Off 
      PTC_SetLow();
      

}

    
void power_off_action_fun(void)
{
    PLASMA_SetLow(); //
    ultrasonic_open();//ultrasnoic Off 
    PTC_SetLow();

   


}




