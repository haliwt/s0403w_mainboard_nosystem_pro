#include "bsp.h"
#include "adc.h"


/* USER CODE BEGIN 0 */

//#define ADC_CHANNEL_NUMBER               2

//static uint16_t Get_Adc_Channel_0(void) ;
//static uint16_t Get_Adc_Channel_1(void) ; 




uint16_t fan_detect_voltage;
uint16_t ptc_detect_voltage;

static void Judge_PTC_Temperature_Value(void);
static uint16_t Get_Fan_Adc_Average(uint32_t ch,uint8_t times);
static uint16_t Get_Ptc_Adc_Average(uint32_t ch,uint8_t times);



static uint16_t Get_Fan_Adc_Channel_0(uint32_t ch);
static uint16_t Get_Ptc_Adc_Channel_1(uint32_t ch);

uint8_t detect_error_times,recoder_error_times;


/*****************************************************************
*
	*Function Name: static uint16_t Get_Adc_Channel_0(uint32_t ch)   
	*Function: FAN of volatage be detected ADC 
	*Input Ref: which one ? AC_Channel_?
	*Return Ref: No
	*
	*
*****************************************************************/
static uint16_t Get_Fan_Adc_Channel_0(uint32_t ch)   
{
   // HAL_StatusTypeDef status;

    ADC_ChannelConfTypeDef ADC1_ChanConf;

	ADC1_ChanConf.Channel=ADC_CHANNEL_0;                                   //Í¨µÀ
    ADC1_ChanConf.Rank= ADC_REGULAR_RANK_1;                                    //第一个序�?
    ADC1_ChanConf.SamplingTime=ADC_SAMPLETIME_1CYCLE_5;//ADC_SAMPLETIME_239CYCLES_5;      //²ÉÑùÊ±¼ä               


	HAL_ADC_ConfigChannel(&hadc1,&ADC1_ChanConf);        //Í¨µÀÅäÖÃ
	
    HAL_ADC_Start(&hadc1);                               //start ADC transmit
	
     HAL_ADC_PollForConversion(&hadc1,10);                //轮询转换

    
 
	   return (uint16_t)HAL_ADC_GetValue(&hadc1);	        	//·µ»Ø×î½üÒ»´ÎADC1¹æÔò×éµÄ×ª»»½á¹û

  
}

/*****************************************************************
*
	*Function Name: static uint16_t Get_Adc_Channel_0(uint32_t ch)   
	*Function: FAN of volatage be detected ADC 
	*Input Ref: which one ? AC_Channel_?
	*Return Ref: No
	*
	*
*****************************************************************/
static uint16_t Get_Ptc_Adc_Channel_1(uint32_t ch)   
{
    ADC_ChannelConfTypeDef ADC1_ChanConf;

	ADC1_ChanConf.Channel=ADC_CHANNEL_1;                                   //Í¨µÀ
    ADC1_ChanConf.Rank= ADC_REGULAR_RANK_1 ;                               //第一个序�?
    ADC1_ChanConf.SamplingTime=ADC_SAMPLETIME_1CYCLE_5;//ADC_SAMPLETIME_239CYCLES_5;      //²ÉÑùÊ±¼ä               


	HAL_ADC_ConfigChannel(&hadc1,&ADC1_ChanConf);        //Í¨µÀÅäÖÃ
	
    HAL_ADC_Start(&hadc1);                               //start ADC transmit
	
    HAL_ADC_PollForConversion(&hadc1,10);                //轮询转换
 
	return (uint16_t)HAL_ADC_GetValue(&hadc1);	        	//·µ»Ø×î½üÒ»´ÎADC1¹æÔò×éµÄ×ª»»½á¹û
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
static uint16_t Get_Fan_Adc_Average(uint32_t ch,uint8_t times)
{
	uint32_t temp_val=0;
	uint8_t t;
  // temp_val=  Get_Fan_Adc_Channel_0(ch);   
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Fan_Adc_Channel_0(ch);  
        vTaskDelay(pdMS_TO_TICKS(5));//WT.EDIT 2024.11.11 modifiy //HAL_Delay(10);
		
	}
	return (uint16_t)temp_val/times;
} 

static uint16_t Get_Ptc_Adc_Average(uint32_t ch,uint8_t times)
{
	uint32_t temp_val=0;
	uint8_t t;
   // temp_val=Get_Ptc_Adc_Channel_1(ch);   
	for(t=0;t<times;t++)
	{
		temp_val+=Get_Ptc_Adc_Channel_1(ch); 
		 vTaskDelay(pdMS_TO_TICKS(5));//WT.EDIT 2024.11.11 modifiy//HAL_Delay(10);
	}
	return (uint16_t)temp_val/times ;
}

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
	
 //  static uint8_t detect_error_times,recoder_error_times;
   uint16_t adc_fan_hex;
   
 
   adc_fan_hex = Get_Fan_Adc_Average(channel,times);

   fan_detect_voltage  =(uint16_t)((adc_fan_hex * 3300)/4096); //amplification 1000 ,3.111V -> 3111


   
    if(fan_detect_voltage < 350 ){ //500  now and then is bug false alarm rate  .
       detect_error_times++;

       if(detect_error_times==1 && recoder_error_times == 0){
             
               recoder_error_times ++;
               gpro_t.gTimer_detect_fan_error = 0;

       }
       
      
    }

    if( gpro_t.gTimer_detect_fan_error > 45   && recoder_error_times==1){

         recoder_error_times=0;

         if(detect_error_times >2){
	   	
		  detect_error_times= 0;
		  //gctl_t.fan_warning = 1;
		  gpro_t.fan_warning_flag = 1;
		

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
        else{

            gpro_t.fan_warning_flag=0;//gctl_t.fan_warning = 0;
         
            detect_error_times=0;


        }

    }
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

  uint16_t adcx;

  if(gctl_t.ptc_warning ==0){
	
     adcx = Get_Ptc_Adc_Average(channel,times);

     ptc_detect_voltage  =(uint16_t)((adcx * 3300)/4096); //amplification 100 ,3.11V -> 311
   }

     // run_t.ptc_temp_voltage= run_t.ptc_temp_voltage - MODIFICATION_VALUE ;
	  Judge_PTC_Temperature_Value();

	
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
static void Judge_PTC_Temperature_Value(void)
{
    #if FAN_OLDER_VERSION

    if(ptc_detect_voltage < 331 || ptc_detect_voltage ==331){ //95 degree

    #else 

       if(ptc_detect_voltage <  261 ){ //105 degree WT.NEW FAN REF


    #endif 

        gctl_t.gDry = 0;
     
        PTC_SetLow(); //ptc turn off

        buzzer_sound();//Buzzer_KeySound();
        osDelay(50);
        buzzer_sound();//Buzzer_KeySound();
        osDelay(50);
        buzzer_sound();//Buzzer_KeySound();
        osDelay(50);
        buzzer_sound();//Buzzer_KeySound();
        osDelay(50);

        SendWifiData_To_Cmd(0x08,0x01);

        if(wifi_link_net_state()==1){

        MqttData_Publish_SetPtc(0);
         vTaskDelay(pdMS_TO_TICKS(200));//HAL_Delay(100);  

        Publish_Data_Warning(ptc_temp_warning ,warning); //fan of default warning.
        osDelay(100);

     }

     }
}


