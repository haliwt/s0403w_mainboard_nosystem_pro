#ifndef __BSP_ADC_H
#define  __BSP_ADC_H
#include "main.h"


#define ADC_CHANNEL_NUMBER    2


#define  FAN_OLDER_VERSION     0 //WT.EDIT 2025.02.20 fan works current is 500mA.




typedef enum{

    no_warning,
	warning,


}warning_state;


void Get_Fan_ADC_Fun(uint8_t  channel,uint8_t times);

void Get_Ptc_ADC_Fun(uint8_t channel,uint8_t times);

void fan_warning_sound(void);

void adc_detected_hundler(void);



#endif 

