#include "bsp.h"

uint8_t plasma_open_flag ;
uint8_t ultrasonic_open_flag;




void (*Single_Usart_ReceiveData)(uint8_t cmd);

void smartphone_timer_power_on_and_normal_handler(void)
{
    
  switch(gctl_t.app_timer_power_on_flag){
		case 0:
        gctl_t.gModel=1;
	    gctl_t.gFan = 1;
		gctl_t.gDry = 1;
     
		plasma_open_flag=1;//gctl_t.gPlasma =1;       //"閺�?閼�?"
		ultrasonic_open_flag=1;//gctl_t.gUlransonic = 1; // "妞硅精娅�"
	    gctl_t.gTimer_fan_run_one_minute=0;

   
       	
			
	    Fan_RunSpeed_Fun();//FAN_CCW_RUN();
	    PLASMA_SetHigh();
	    HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);//ultrasnoic ON 
	    PTC_SetHigh();

        if(wifi_link_net_state() ==1){
    
		     MqttData_Publish_SetOpen(1);  
			 HAL_Delay(200);
		     updateDht11_sensorData_toDisp();
			 HAL_Delay(200);
	         gctl_t.set_wind_speed_value =100;
		
			 MqttData_Publish_Update_Data();
			 HAL_Delay(200);

         }
  
	break;

	case 1: //app timer timing power of 
	       gctl_t.gModel =1;

          Parse_Json_Statement();
		  
           if(plasma_open_flag==1){//if( gctl_t.gPlasma==1){ //Anion
			

				SendWifiData_To_Cmd(0x03,0x01);
                osDelay(100);
			
			}
			else{
				plasma_open_flag=0;//gctl_t.gPlasma =0;
				SendWifiData_To_Cmd(0x03,0x0);
				 osDelay(100);
			}


			if(ultrasonic_open_flag==1){//if(gctl_t.gUlransonic==1){

					SendWifiData_To_Cmd(0x04,0x01);
					 osDelay(100);
			}
			else {
					ultrasonic_open_flag=0;//gctl_t.gUlransonic=0;
					SendWifiData_To_Cmd(0x04,0x0);
					 osDelay(100);
			}



			if(gctl_t.gDry==1){

				SendWifiData_To_Cmd(0x02,0x01);
				 osDelay(100);
			}
			else{
					gctl_t.gDry=0;
                  
					SendWifiData_To_Cmd(0x02,0x0);
					 osDelay(100);

			}

		     gctl_t.set_wind_speed_value =100;
	
		     MqttData_Publish_Update_Data();
		     HAL_Delay(200);

			
	     break;
		}
			
}

void SetPowerOff_ForDoing(void)
{
   
     gctl_t.set_wind_speed_value =10;
 
    gctl_t.gFan = 0;
    gctl_t.gDry = 0;
  
	plasma_open_flag=0;//gctl_t.gPlasma =0;       //"閺�?閼�?"
	ultrasonic_open_flag=0;//gctl_t.gUlransonic = 0; // "妞硅精娅�"
	gctl_t.gModel =1;


    
	PLASMA_SetLow(); //
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);//ultrasnoic Off 
	PTC_SetLow();
	

}

void Single_Usart_RxData(void(*rxHandler)(uint8_t dat))
{

      Single_Usart_ReceiveData=  rxHandler;

}

void ActionEvent_Handler(void)
{


   if( gctl_t.gDry==1){

 

    if(gpro_t.fan_warning_flag ==0){
      
       
           PTC_SetHigh();
        

	  }
    }
    else{
   
         PTC_SetLow();
   }

  
	if(plasma_open_flag==1){//if(gctl_t.gPlasma == 1){
		
	     PLASMA_SetHigh();
	}
	else{

		PLASMA_SetLow();
	}
	//driver bug
	if(ultrasonic_open_flag==1){//if(gctl_t.gUlransonic ==1){
	
	 
		HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);//ultrasnoic ON 
	}
	else{
	  HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);//ultrasnoic off

	}

	Fan_RunSpeed_Fun();
		
 }



void every_power_on_run(void)
{

   Fan_Full_Speed();//WT.EDIT 2025.01.03//Fan_RunSpeed_Fun();//FAN_CCW_RUN();
   if(gctl_t.app_timer_power_on_flag==0){
     
      gctl_t.gModel=1;
      gctl_t.gFan = 1;
      gctl_t.gDry = 1;
      //g_dry_open_flag =1;
      plasma_open_flag=1;//gctl_t.gPlasma =1;       //"閺�?閼�?"
      ultrasonic_open_flag=1;//gctl_t.gUlransonic = 1; // "妞硅精娅�"
      gctl_t.gTimer_fan_run_one_minute=0;
       gpro_t.process_run_step=0;
      PLASMA_SetHigh();
      HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);//ultrasnoic ON 
      PTC_SetHigh();

    }
}



