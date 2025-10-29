#include "bsp.h"


#define BIT_1                (1<<1)

#define DECODER_BIT_0        (1<< 0)

/***********************************************************************************************************
											函数声明
***********************************************************************************************************/
//static void vTaskWifiPro(void *pvParameters);
static void vTaskMsgPro(void *pvParameters);
static void vTaskStart(void *pvParameters);
static void AppTaskCreate (void);


/* 创建任务通信机制 */
//static void AppObjCreate(void);


/***********************************************************************************************************
											变量声明
***********************************************************************************************************/
//static TaskHandle_t xHandleTaskWifiPro = NULL;
static TaskHandle_t xHandleTaskMsgPro = NULL;
static TaskHandle_t xHandleTaskStart = NULL;




#define LOWEST_PRIORITY   1  // ???????
#define HIGHEST_PRIORITY  2


uint8_t power_on_sound_flag ;

/**********************************************************************************************************
*
*	Function Name:void freeRTOS_Handler(void)
*   Function :
*	
*  
**********************************************************************************************************/
void freeRTOS_Handler(void)
{
     /* 创建任务 */
	   AppTaskCreate();
	  
	  /* 创建任务通信机制 */
//	   AppObjCreate();
	  
	  /* 启动调度，开始执行任�?1�?7?1�?1�?7?7 */
	   vTaskStartScheduler();


}
#if 0
/**
 * @brief  :  
 * @note    
 * @param   None
 * @retval  None
 */
static void vTaskWifiPro(void * pvParameters)
{
	while(1)
	{

         if(gpro_t.wifi_led_fast_blink_flag==0 ){
             wifi_communication_tnecent_handler();//
             getBeijingTime_cofirmLinkNetState_handler();
             wifi_auto_detected_link_state();
          }


      vTaskDelay(pdMS_TO_TICKS(100));
	}

}
#endif 
/**
 * @brief  :  static void vTaskStart(void *pvParameters)�������ݴ����������ȼ�Ϊ�е�
 * @note    �����ڲ�ʹ�ö��н������ݣ����ȳ�ʼ������
 * @param   None
 * @retval  None
 */
static void vTaskMsgPro(void *pvParameters)
{
  
	BaseType_t xResult;
	const TickType_t xMaxBlockTime = pdMS_TO_TICKS(9000); /* 1.?????-?????????50ms */
    uint32_t ulValue;
    
	
    while(1)
    {
       xResult = xTaskNotifyWait(0x00000000,      
						           0xFFFFFFFF,      
						          &ulValue,        /* ??ulNotifiedValue???ulValue? */
						          xMaxBlockTime);  /* ????????,????-block portMAX_DELAY */
        if(xResult == pdPASS){
             if((ulValue & DECODER_BIT_0 ) != 0)
             {
                  // parse_recieve_data_handler();//receive_data_from_display(gl_tMsg.usData);
             
				 usart1_protocol_state_machine();
				 	//vTaskPrioritySet(xHandleTaskMsgPro, LOWEST_PRIORITY);  // ???????
	       			///taskYIELD();  // ??????
	    			//vTaskPrioritySet(xHandleTaskStart,HIGHEST_PRIORITY);  // ???????

                  
             }
			 
                
         }

				                                   
 	}
}	

/**
 * @brief  :  static void vTaskStart(void *pvParameters)�������ݴ����������ȼ�Ϊ�е�
 * @note    �����ڲ�ʹ�ö��н������ݣ����ȳ�ʼ������
 * @param   None
 * @retval  None
 */
static void vTaskStart(void *pvParameters)
{
    

	while(1)
    {
  
       	if(power_on_sound_flag==0){
            power_on_sound_flag ++;
            FAN_Stop();  //WT.EDIT.2025.01.03
            buzzer_sound();//buzzer_sound();
			//printf("buzzer_sound !!!\r\n");

        }


          switch(gpro_t.gpower_on){ 

            case power_on:
		 
			gpro_t.power_off_run_step=0;
            power_on_handler();
           
            link_wifi_to_tencent_handler(); //detected ADC of value 

		    ai_mode_display_fun();

			

			if(gpro_t.process_run_step > 10)gpro_t.process_run_step=6; //WT.EDIT 2025.10.07
		   

			if(gpro_t.answer_buzzer_flag > 1 || gpro_t.two_hours_state > 2 ){
				if(gpro_t.answer_buzzer_flag == 1)gpro_t.answer_buzzer_flag =0;
				if(gpro_t.two_hours_state >2)gpro_t.two_hours_state=0; //WT.EDIT 2025.10.28
				
            }
			else if(gpro_t.answer_buzzer_flag == 1){ //WT.EDIT 2025.07.28 
				gpro_t.answer_buzzer_flag =0;

				SendWifiData_Answer_Cmd(0x16,0x01); //WT.EDIT 2025.07.28
				vTaskDelay(pdMS_TO_TICKS(10));
		   	}
		  break;

		  

          case power_off:
              gpro_t.process_run_step=0;
			  counter_two_hours=0;
		     power_off_handler();
             break;
          }

          if(gpro_t.process_run_step > 10){

		      gpro_t.process_run_step=6;
			  freertos_set_prority();

		  }
          else if(gpro_t.wifi_led_fast_blink_flag > 1){
		  	 gpro_t.wifi_led_fast_blink_flag=0;
			
		  }
		  else if(gpro_t.wifi_led_fast_blink_flag==0 ){
             wifi_communication_tnecent_handler();//
             getBeijingTime_cofirmLinkNetState_handler();
             wifi_auto_detected_link_state();
           }
		  
		
		  vTaskDelay(pdMS_TO_TICKS(50));//�?1�?7?0


        }
       
    }
 /**
 * @brief  :  void AppTaskCreate (void)�����ݴ����������ȼ�Ϊ�е�
 * @note    �����ڲ�ʹ�ö��н������ݣ����ȳ�ʼ������
 * @param   None
 * @retval  None
 */
void AppTaskCreate (void)
{

//  xTaskCreate( vTaskWifiPro,     		/* 任务函数  */
//                 "vTaskWifiPro",   		/* 任务�?1�?7?1�?1�?7?7    */
//                 128,            		/* 任务栈大小，单位word，也就是4字节 */
//                 NULL,           		/* 任务参数  */
//                 2,              		/* 任务优先�?1�?7?1�?1�?7?7 数�1�?7�?1�?7越小优先级越低，这个跟uCOS相反 */
//                 &xHandleTaskWifiPro);   /* 任务句柄  */



  xTaskCreate( vTaskMsgPro,     		/* 任务函数  */
                 "vTaskMsgPro",   		/* 任务�?1�?7?1�?1�?7?7    */
                 128,            		/* 任务栈大小，单位word，也就是4字节 */
                 NULL,           		/* 任务参数  */
                 2,              		/* 任务优先�?1�?7?1�?1�?7?7 数�1�?7�?1�?7越小优先级越低，这个跟uCOS相反 */
                 &xHandleTaskMsgPro);   /* 任务句柄  */


   xTaskCreate( vTaskStart,     		/* 任务函数  */
                 "vTaskStart",   		/* 任务�?1�?7?1�?1�?7?7    */
                 256,            		/* 任务栈大小，单位word，也就是4字节 */
                 NULL,           		/* 任务参数  */
                 1,              		/* 任务优先�?1�?7?1�?1�?7?7 数�1�?7�?1�?7越小优先级越低，这个跟uCOS相反 */
                 &xHandleTaskStart );   /* 任务句柄  */
}


/********************************************************************************
	**
	*Function Name:void freertos_set_prority(void)
	*Function :
	*Input Ref: 
	*Return Ref:NO
	*
*******************************************************************************/
void freertos_set_prority(void)
{
	
	vTaskPrioritySet(xHandleTaskStart, LOWEST_PRIORITY);  // ???????
	taskYIELD();  // ??????
	vTaskPrioritySet(xHandleTaskMsgPro,HIGHEST_PRIORITY);  // ???????

   
} 

void freertos_decoder_isr_handler(void)
{
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	xTaskNotifyFromISR(xHandleTaskMsgPro,  /* ???? */
						DECODER_BIT_0,	   /* ???????????bit0  */
						eSetBits,  /* ????????????BIT_0?????, ??????????? */
						&xHigherPriorityTaskWoken);

	/* ??xHigherPriorityTaskWoken = pdTRUE,???????????????????? */
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

}
