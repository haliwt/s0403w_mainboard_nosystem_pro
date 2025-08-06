#include "bsp.h"
volatile uint8_t check_time;
volatile uint8_t stopHoursCounter;



void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
   uint32_t temp;
	if(huart->Instance==USART2){

		if(__HAL_UART_GET_FLAG(&huart2,UART_FLAG_ORE)!=RESET){

             __HAL_UART_CLEAR_OREFLAG(&huart2);
		
			UART_Start_Receive_IT(&huart2,wifi_rx_inputBuf,1);

		}
		__HAL_UNLOCK(&huart2);
		   
       
          temp = USART2->RDR;
		UART_Start_Receive_IT(&huart2,wifi_rx_inputBuf,1);


	}
	if(huart->Instance==USART1){
	
		if(__HAL_UART_GET_FLAG(&huart1,UART_FLAG_ORE)!=RESET){

		__HAL_UART_CLEAR_OREFLAG(&huart1);
		UART_Start_Receive_IT(&huart1,inputBuf,1);

		}
		__HAL_UNLOCK(&huart1);
		//  temp = USART1 ->ISR;
		temp = USART1->RDR;
		UART_Start_Receive_IT(&huart1,inputBuf,1);
	
		}
}
/********************************************************************************
	**
	*Function Name:void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
	*Function :UART callback function  for UART interrupt for receive data
	*Input Ref: structure UART_HandleTypeDef pointer
	*Return Ref:NO
	*
*******************************************************************************/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
   static  uint16_t tm0, tm1;
	
  if (htim->Instance == TIM14)
  {
    HAL_IncTick();
  }
   else if(htim->Instance==TIM17){
		
	   tm0 ++ ;
       
     
	 if(tm0 > 999){//10ms *100 = 1000ms =1s
        tm0 =0;
        
        gpro_t.gTimer_link_net_timer_time++;
	    gctl_t.gTimer_senddata_panel++;
       

	   gctl_t.gTimer_fan_adc_times++;
	   gctl_t.gTimer_ptc_adc_times++;

	
	  gctl_t.gTimer_linkTencentCounter++;
	  gctl_t.gTimer_fan_run_one_minute++;
	  //wifi function 
       wifi_t.gTimer_auto_detected_net_state_times ++;
       wifi_t.gTimer_auto_link_net_time++ ;

	   wifi_t.gTimer_get_beijing_time++;

     //gprocess 
       gpro_t.gTimer_power_on_first_link_tencent++;
       gpro_t.gTimer_get_data_from_tencent_data++;
       gpro_t.gTimer_dc_power_on_auto_link_net++;
     

       gpro_t.gTimer_publis_dht11_data++ ;
       gpro_t.gTimer_detect_fan_error++;
       gpro_t.gTimer_again_send_power_on_off++;
	   gpro_t.gTimer_power_on_auto_link++;
	   gpro_t.gTimer_update_todisplay++;
	   gpro_t.gTimer_update_tencet_dht11++;

	   
	   gTimer_powerOffRunFan++;
      stopHoursCounter++;


      if(stopHoursCounter> 59){ //one minute
          stopHoursCounter =0;
       
          
          check_time ++;
         #if TEST_UNIT
          if(check_time >3  && stopHours_flag ==0){ //119
             check_time=0;
          
             stopHours_flag =1;
              
          }
         #else 
           if(check_time >119  && stopHours_flag ==0){ //119
                   check_time=0;
                
                   stopHours_flag =1;
                    
           }


         #endif 
       }


       


//      
	 
	  }
 	}
 }
