#include "fan.h"
#include "main.h"
#include "tim.h"
#include "run.h"
#include "delay.h"
#include "tim.h"
#include "gpio.h"


static void SetLevel_Fan_PWMA(uint8_t levelval);



void FAN_CCW_RUN(void)
{
   FAN_CW_SetLow();
   //SetLevel_Fan_PWMA(100);
   FAN_RUN_SetHigh();
  
}

void FAN_Stop(void)
{
   FAN_CW_SetLow(); //brake
 
   //SetLevel_Fan_PWMA(0);//SetLevel_Fan_PWMA(16);
   FAN_RUN_SetLow();
}

void Fan_One_Power_Off_Speed(void)
{
	
	 FAN_CW_SetLow(); //brake
     FAN_RUN_SetHigh();

	


}
#if 1
void Fan_One_Speed(void)
{
	FAN_CW_SetLow();

   FAN_RUN_SetHigh();


}

void Fan_Two_Speed(void)
{
	
	FAN_CW_SetLow();
  
   FAN_RUN_SetHigh();
}

 void Fan_Full_Speed(void)
{
   FAN_CW_SetLow();
   FAN_RUN_SetHigh();

}
#endif 



void ShutDown_AllFunction(void)
{
	
	PLASMA_SetLow(); //
	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);//ultrasnoic ON 
	PTC_SetLow();
	FAN_Stop();



}
//"杀毒" 
void SterIlization(uint8_t sel)
{
    if(sel==0){//open 
		
		 PLASMA_SetHigh();
		 HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);//ultrasnoic ON 

	}
	else{ //close

			
		PLASMA_SetLow();
		HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);//ultrasnoic off
	

	}



}

void Dry_Function(uint8_t sel)
{
   if(sel ==0){

  
		//FAN_CCW_RUN();
		PTC_SetHigh();

   }
   else{

       PTC_SetLow();

   }

}

void Fan_RunSpeed_Fun(void)
{
         if(run_t.set_wind_speed_value < 34 ){
              Fan_One_Speed();
		 }
		 else if(run_t.set_wind_speed_value > 33  && run_t.set_wind_speed_value < 67 ){

             Fan_Two_Speed();

		 }
		 else if(run_t.set_wind_speed_value > 66)
		 	Fan_Full_Speed();


}
/********************************************************
*
*Function Name:void SetLevel_Fan_PWMA(uint8_t levelval)
*Function: 
*
*
********************************************************/
static void SetLevel_Fan_PWMA(uint8_t levelval)
{
     run_t.gFan_pwm_duty_level = levelval;
     FAN_CW_SetLow();
	// MX_TIM16_Init();
	 HAL_TIM_PWM_Start(&htim16,TIM_CHANNEL_1);
}


