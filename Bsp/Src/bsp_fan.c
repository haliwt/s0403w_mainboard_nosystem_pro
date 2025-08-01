#include "bsp.h"

#define FAN_PWM_100     40

#define FAN_PWM_90      36

#define FAN_PWM_80      32

uint8_t fan_switch_gears_flag;
/******************************************************************************
*
*Fan adjut speed frequency is : 18KHz~25KHz 
*
*
*******************************************************************************/
static void SetLevel_Fan_PWMA(uint8_t levelval);


void fan_run_fun(void)
{
    FAN_COM_SetLow();
	FAN_RUN_SetHigh();
	#if NEWPCB_FAN
	  SetLevel_Fan_PWMA(FAN_PWM_100);
	#endif 
	

}
void FAN_Stop(void)
{
   FAN_COM_SetLow(); //brake
   FAN_RUN_SetLow();//SetLevel_Fan_PWMA(0);//SetLevel_Fan_PWMA(16);
  
}

void Fan_One_Power_Off_Speed(void)
{
	
	 fan_run_fun();//SetLevel_Fan_PWMA(10);
}


void Fan_One_Speed(void)
{
   static uint8_t one_speed=0xff;
	 FAN_COM_SetLow();
     FAN_RUN_SetHigh();
     if(one_speed != fan_switch_gears_flag){
        fan_switch_gears_flag++;
        one_speed = fan_switch_gears_flag ;  //one_speed =2,5,8
        #if NEWPCB_FAN
	       SetLevel_Fan_PWMA(FAN_PWM_80);
		#endif 

     }


}

void Fan_Two_Speed(void)
{
     static uint8_t two_speed=0xff;
      FAN_COM_SetLow();
     FAN_RUN_SetHigh();
      if(two_speed != fan_switch_gears_flag){
         fan_switch_gears_flag++;
         two_speed = fan_switch_gears_flag;  //two_speed = 3;6,9

         #if NEWPCB_FAN
	        SetLevel_Fan_PWMA(FAN_PWM_90);
		 #endif 
       }

}

 void Fan_Full_Speed(void)
{
   static uint8_t full_speed = 0xff;
     FAN_COM_SetLow();
     FAN_RUN_SetHigh();
    if(full_speed != fan_switch_gears_flag){
        fan_switch_gears_flag++;
         full_speed = fan_switch_gears_flag;  //full_speed =1,4,7,10
         #if  NEWPCB_FAN
              SetLevel_Fan_PWMA(FAN_PWM_100);
		 #endif 
    }

}

//void ShutDown_AllFunction(void)
//{
//	
//	PLASMA_SetLow();
//	HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);//ultrasnoic ON 
//	PTC_SetLow();
//	FAN_Stop();
//   


//}
////"Êù?ÊØ?" 
//void ultrasonic_fun(uint8_t sel)
//{
//    if(sel==0){//open 
//		
//		
//		 HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);//ultrasnoic ON 

//	}
//	else{ //close

//			
//		
//		HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);//ultrasnoic off
//	

//	}



//}

//void Dry_Function(void)
//{
// 
//  switch(dry_open_flag ){

//   case 1:

//      if(gctl_t.ptc_warning ==0){
//  
//          PTC_SetHigh();
//           
//        }
//         
//      break;
//    
//      case 0 :
//       
//            PTC_SetLow();
//    
//      }
//             
//      
//}


//void plasma_fun(uint8_t sel)
//{
//     if(sel ==0){
//        PLASMA_SetHigh();

//     }
//     else{

//        PLASMA_SetLow();
//     }

//}



//void Fan_RunSpeed_Fun(void)
//{

//    if(gctl_t.set_wind_speed_value < 34 ){
//        Fan_One_Speed();
//    }
//    else if(gctl_t.set_wind_speed_value > 33  && gctl_t.set_wind_speed_value < 67 ){

//        Fan_Two_Speed();

//    }
//    else if(gctl_t.set_wind_speed_value > 66){

//         Fan_Full_Speed();

//    }

//      

//}



void Fan_RunSpeed_Fun(void)
{

    if(gctl_t.set_wind_speed_value < 34 ){
        Fan_One_Speed();
    }
    else if(gctl_t.set_wind_speed_value > 33  && gctl_t.set_wind_speed_value < 67 ){

        Fan_Two_Speed();

    }
    else if(gctl_t.set_wind_speed_value > 66){

         Fan_Full_Speed();

    }

      

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
   gpro_t.gFan_pwm_duty_level = levelval;
   MX_TIM16_Init();
   HAL_TIM_PWM_Start(&htim16,TIM_CHANNEL_1);
	
}


#if 0
void fan_start_fun(void)
{

   SetLevel_Fan_PWMA(10);
   osDelay(100);
   FAN_Stop();
   SetLevel_Fan_PWMA(10);
   osDelay(100);
   FAN_Stop();
   SetLevel_Fan_PWMA(10);
   osDelay(200);


}
#endif 

