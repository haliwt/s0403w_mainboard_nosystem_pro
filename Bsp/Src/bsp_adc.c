#include "bsp.h"
#include "adc.h"



// ADC相关变量定义
#define ADC_BUFFER_SIZE 2
volatile uint16_t adc_buffer[ADC_BUFFER_SIZE]; // DMA传输缓冲区
volatile uint8_t adc_conversion_complete = 0;


/* USER CODE BEGIN 0 */




uint16_t fan_detect_voltage;
uint16_t ptc_detect_voltage;


static uint16_t compute_voltage(uint16_t raw_value) ;
static uint16_t ADC_FAN_ReadVoltage(void);
static uint16_t ADC_PTC_ReadVoltage(void);
static uint8_t ADC_StartConversion(void);
static void ADC_GetValues(void);


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
   // static uint8_t switch_flag;
    if(gctl_t.gTimer_fan_adc_times > 3 && gpro_t.stopTwoHours_flag ==0){ //detected 3 times is 60s 
        gctl_t.gTimer_fan_adc_times =0;

	   //switch_flag = switch_flag ^ 0x01;
	   if(ADC_StartConversion()){
	   ADC_GetValues();
	   vTaskDelay(pdMS_TO_TICKS(200));
	   	}
        
    }
	
   fan_warning_sound();
	

}


// 启动ADC转换
static uint8_t ADC_StartConversion(void)
{
    if(LL_ADC_IsEnabled(ADC1) == 0) {
        return 0; // ADC未使能
    }
    
    if(LL_DMA_IsEnabledChannel(DMA1, LL_DMA_CHANNEL_3)) {
        LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_3);
    }
    
    // 配置DMA传输
    LL_DMA_ConfigAddresses(DMA1, LL_DMA_CHANNEL_3,
                          LL_ADC_DMA_GetRegAddr(ADC1, LL_ADC_DMA_REG_REGULAR_DATA),
                          (uint32_t)adc_buffer,
                          LL_DMA_DIRECTION_PERIPH_TO_MEMORY);
    
    LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_3, ADC_BUFFER_SIZE);
    
    adc_conversion_complete = 0;
    
    // 使能DMA通道
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_3);
    
    // 启动ADC转换
    LL_ADC_REG_StartConversion(ADC1);
    
    return 1;
}

// 获取ADC转换结果
void ADC_GetValues(void)
{
    //if(adc_conversion_complete) {
        fan_detect_voltage = (adc_buffer[0] * 3300 )/4095; // PA0 - FAN
        ptc_detect_voltage = (adc_buffer[1] * 3300)/4095; // PA1 - PTC
      //  adc_conversion_complete = 0;
       // return 1;
    //}
    //return 0;
}


/*****************************************************************
	*
	*Function Name: 
	*Function :ADC input channel be selected "which one channe"
	*Input Ref: which one ? AC_Channel_?, hexadecimal of average
	*Return Ref: No
	*
*****************************************************************/
static uint16_t ADC_FAN_ReadVoltage(void)
{
    uint16_t raw_value;
	LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_0);
    LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_0, LL_ADC_SAMPLINGTIME_COMMON_1);

	LL_ADC_REG_StartConversion(ADC1);

	while(!LL_ADC_IsActiveFlag_EOC(ADC1));

	raw_value= LL_ADC_REG_ReadConversionData12(ADC1);
	
	LL_ADC_ClearFlag_EOC(ADC1);

	return compute_voltage(raw_value);

}
/*****************************************************************
	*
	*Function Name: static uint16_t ADC_PTC_ReadVoltage(void)
	*Function :
	*Input Ref:
	*Return Ref: No
	*
*****************************************************************/
static uint16_t ADC_PTC_ReadVoltage(void)
{
    uint16_t raw_value;
    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_2, LL_ADC_CHANNEL_1);
    LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_1, LL_ADC_SAMPLINGTIME_COMMON_1);

	LL_ADC_REG_StartConversion(ADC1);

	while(!LL_ADC_IsActiveFlag_EOC(ADC1));

	raw_value= LL_ADC_REG_ReadConversionData12(ADC1);
	
	LL_ADC_ClearFlag_EOC(ADC1);

	return compute_voltage(raw_value);

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
	            // vTaskDelay(pdMS_TO_TICKS(200));//HAL_Delay(350);

    		   Publish_Data_Warning(fan_warning,warning);
    	       // vTaskDelay(pdMS_TO_TICKS(200));//HAL_Delay(200);

           }

    }



}
/*****************************************************************
	*
	*Function Name: 
	*Function ADC input channel be selected "which one channe"
	*Input Ref: which one ? AC_Channel_?, hexadecimal of average
	*Return Ref: No
	*
*****************************************************************/
static uint16_t compute_voltage(uint16_t raw_value) 
{
    uint64_t temp ;
	const uint32_t multiplier = 3462835200U;
   temp = (uint64_t)raw_value * multiplier;
    return (uint16_t)(temp >> 32);  // ��λ��mV
}





