#include "bsp.h"
#include "adc.h"

#define ADC_CHANNEL_COUNT 2
#define ADC_BUFFER_SIZE   30  // 每个通道采样30次

__IO uint16_t adc_dma_buffer[ADC_CHANNEL_COUNT * ADC_BUFFER_SIZE];  // DMA采样缓冲区
float adc_voltage[ADC_CHANNEL_COUNT];  // 存储转换后的电压值



/* USER CODE BEGIN 0 */




uint16_t fan_detect_voltage;
uint16_t ptc_detect_voltage;



/**********************************************************************
	*
	*Functin Name: void adc_detected_hundler(void)
	*Function :
	*Input Ref:  key of value
	*Return Ref: NO
	*
**********************************************************************/
void adc_detected_hundler(void)
{
   
    if(gctl_t.gTimer_fan_adc_times > 8 && gpro_t.stopTwoHours_flag ==0 && gpro_t.fan_warning_flag  == 0){ //detected 3 times is 60s 
        gctl_t.gTimer_fan_adc_times =0;
       // Get_Fan_ADC_Fun(ADC_CHANNEL_0,20);
        
    }
	
   fan_warning_sound();
	

}




/*****************************************************************
*
	*Function Name: void Start_ADC_DMA(void)
	*Function: 
	*Input Ref: 
	*Return Ref: No
	*
	*
*****************************************************************/
void Start_ADC_DMA(void)
{
    LL_ADC_StartCalibration(ADC1);
    while (LL_ADC_IsCalibrationOnGoing(ADC1));

    LL_ADC_Enable(ADC1);
    while (LL_ADC_IsEnabled(ADC1) == 0);

    LL_ADC_REG_StartConversion(ADC1);

    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_3);
}


/*****************************************************************
*
	*Function Name: void Process_ADC_Data(void)
	*Function:
	*Input Ref: 
	*Return Ref: No
	*
	*
*****************************************************************/
void Process_ADC_Data(void)
{
    uint32_t sum[ADC_CHANNEL_COUNT] = {0};

    for (int i = 0; i < ADC_BUFFER_SIZE; i++) {
        sum[0] += adc_dma_buffer[i * ADC_CHANNEL_COUNT + 0];  // ADC_IN0
        sum[1] += adc_dma_buffer[i * ADC_CHANNEL_COUNT + 1];  // ADC_IN1
    }

    adc_voltage[0] = (float)sum[0] / ADC_BUFFER_SIZE * 3.3f / 4096.0f;
    adc_voltage[1] = (float)sum[1] / ADC_BUFFER_SIZE * 3.3f / 4096.0f;
}


/*****************************************************************
*
	*Function Name: static uint16_t Get_Adc_Average(uint32_t ch,uint8_t times)
	*Function ADC input channel be selected "which one channe"
	*Input Ref: which one ? AC_Channel_?
	*Return Ref: No
	*
	*
*****************************************************************/
//static uint16_t Get_Fan_Adc_Average(uint32_t ch,uint8_t times)
//{
//	uint32_t temp_val=0;
//	uint8_t t;
//  // temp_val=  Get_Fan_Adc_Channel_0(ch);   
//	for(t=0;t<times;t++)
//	{
//		temp_val+=Get_Fan_Adc_Channel_0(ch);  
//        vTaskDelay(pdMS_TO_TICKS(5));//WT.EDIT 2024.11.11 modifiy //HAL_Delay(10);
//		
//	}
//	return (uint16_t)temp_val/times;
//} 
//
//static uint16_t Get_Ptc_Adc_Average(uint32_t ch,uint8_t times)
//{
//	uint32_t temp_val=0;
//	uint8_t t;
//   // temp_val=Get_Ptc_Adc_Channel_1(ch);   
//	for(t=0;t<times;t++)
//	{
//		temp_val+=Get_Ptc_Adc_Channel_1(ch); 
//		 vTaskDelay(pdMS_TO_TICKS(5));//WT.EDIT 2024.11.11 modifiy//HAL_Delay(10);
//	}
//	return (uint16_t)temp_val/times ;
//}

/*****************************************************************
	*
	*Function Name: void Get_Fan_Adc_Fun(uint8_t channel,uint8_t times)
	*Function ADC input channel be selected "which one channe"
	*Input Ref: which one ? AC_Channel_?, hexadecimal of average
	*Return Ref: No
	*
*****************************************************************/
void Get_Fan_ADC_Fun(uint8_t channel,uint8_t times)
{
	
 

//    if( gpro_t.gTimer_detect_fan_error > 45   && recoder_error_times==1){

//         recoder_error_times=0;

//         if(detect_error_times >2){
//	   	
//		  detect_error_times= 0;
//		  //gctl_t.fan_warning = 1;
//		  gpro_t.fan_warning_flag = 1;
//		

//           buzzer_sound();//Buzzer_KeySound();
//		   osDelay(100);
//		   buzzer_sound();//Buzzer_KeySound();
//		   osDelay(100);
//		   buzzer_sound();//Buzzer_KeySound();
//			osDelay(100);
//		   buzzer_sound();//Buzzer_KeySound();
//		   osDelay(100);
//		   buzzer_sound();//Buzzer_KeySound();
//		   osDelay(100);
//          
//           SendWifiData_To_Cmd(0x09, 0x01);

//	       if(wifi_link_net_state()==1){

//               MqttData_Publis_SetFan(0);
//	            vTaskDelay(pdMS_TO_TICKS(200));//HAL_Delay(350);
//               Publish_Data_Warning(fan_warning,warning);
//    	        vTaskDelay(pdMS_TO_TICKS(200));//HAL_Delay(200);

//           }
//		}
//        else{

//            gpro_t.fan_warning_flag=0;//gctl_t.fan_warning = 0;
//         
//           


//        }

//    }
}


void fan_warning_sound(void)
{
   if(gpro_t.fan_warning_flag == 1 && gpro_t.gTimer_detect_fan_error > 9){
        gpro_t.gTimer_detect_fan_error =0;


         buzzer_sound();//Buzzer_KeySound();
		   osDelay(100);
		   buzzer_sound();//Buzzer_KeySound();
		   osDelay(100);
		   buzzer_sound();//Buzzer_KeySound();
			osDelay(100);
		   buzzer_sound();//Buzzer_KeySound();
		   osDelay(100);
		   buzzer_sound();//Buzzer_KeySound();
		   osDelay(100);
          
           SendWifiData_To_Cmd(0x09, 0x01);

	       if(wifi_link_net_state()==1){

               MqttData_Publis_SetFan(0);
	             vTaskDelay(pdMS_TO_TICKS(200));//HAL_Delay(350);

    		   Publish_Data_Warning(fan_warning,warning);
    	        vTaskDelay(pdMS_TO_TICKS(200));//HAL_Delay(200);

           }

    }



}
/*****************************************************************
	*
	*Function Name: void Get_Ptc_ADC_Fun(uint8_t channel,uint8_t times)
	*Function ADC input channel be selected "which one channe"
	*Input Ref: which one ? AC_Channel_?, hexadecimal of average
	*Return Ref: No
	*
*****************************************************************/
void Get_Ptc_ADC_Fun(uint8_t channel,uint8_t times)
{

//  uint16_t adcx;
//
//  if(gctl_t.ptc_warning ==0){
//	
//     adcx = Get_Ptc_Adc_Average(channel,times);
//
//     ptc_detect_voltage  =(uint16_t)((adcx * 3300)/4096); //amplification 100 ,3.11V -> 311
//   }
//
//     // run_t.ptc_temp_voltage= run_t.ptc_temp_voltage - MODIFICATION_VALUE ;
//	 

	
}


/*****************************************************************
	*
	*Function Name: void Judge_PTC_Temperature_Value(void)
	*Function: PTC adc read voltage
	*Input Ref: NO
	*Return Ref: No
	*
	*
*****************************************************************/
//static void Judge_PTC_Temperature_Value(void)
//{
//    #if FAN_OLDER_VERSION

//    if(ptc_detect_voltage < 331 || ptc_detect_voltage ==331){ //95 degree

//    #else 

//       if(ptc_detect_voltage <  261 ){ //105 degree WT.NEW FAN REF


//    #endif 

//        gctl_t.gDry = 0;
//     
//        PTC_SetLow(); //ptc turn off

//        buzzer_sound();//Buzzer_KeySound();
//        osDelay(50);
//        buzzer_sound();//Buzzer_KeySound();
//        osDelay(50);
//        buzzer_sound();//Buzzer_KeySound();
//        osDelay(50);
//        buzzer_sound();//Buzzer_KeySound();
//        osDelay(50);

//        SendWifiData_To_Cmd(0x08,0x01);

//        if(wifi_link_net_state()==1){

//        MqttData_Publish_SetPtc(0);
//         vTaskDelay(pdMS_TO_TICKS(200));//HAL_Delay(100);  

//        Publish_Data_Warning(ptc_temp_warning ,warning); //fan of default warning.
//        osDelay(100);

//     }

//     }
//}


