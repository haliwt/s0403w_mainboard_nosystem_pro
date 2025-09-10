#include "bsp.h"


#define BIT_1                (1<<1)

#define DECODER_BIT_0        (1<< 0)

/***********************************************************************************************************
											鍑芥暟澹版槑
***********************************************************************************************************/
//static void vTaskWifiPro(void *pvParameters);
static void vTaskMsgPro(void *pvParameters);
static void vTaskStart(void *pvParameters);
static void AppTaskCreate (void);


/* 鍒涘缓浠诲姟閫氫俊鏈哄埗 */
//static void AppObjCreate(void);


/***********************************************************************************************************
											鍙橀噺澹版槑
***********************************************************************************************************/
//static TaskHandle_t xHandleTaskWifiPro = NULL;
static TaskHandle_t xHandleTaskMsgPro = NULL;
static TaskHandle_t xHandleTaskStart = NULL;


#if 0
//LED锟?1锟?7锟?1锟?7锟?0锟?8锟?1锟?7锟?1锟?7锟?1锟?7锟?1锟?71
void LED_Thread1(void const * argument)
{

  /* USER CODE BEGIN 5 */
  (void) argument;
  /* Infinite loop */
  for (;;)
  {
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);//PB4锟?1锟?7锟?1锟?7锟?1锟?7锟?1锟?7锟?1锟?7
  osDelay(100);//锟?1锟?7锟?0锟?9锟?1锟?7100ms
 
  }
  /* USER CODE END 5 */ 
}

//LED锟?1锟?7锟?1锟?7锟?0锟?8锟?1锟?7锟?1锟?7锟?1锟?7锟?1锟?72
void LED_Thread2(void const * argument)
{
  /* USER CODE BEGIN LED_Thread2 */
  (void) argument;
  /* Infinite loop */
  for (;;)
  {
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);//PB4锟?1锟?7锟?1锟?7锟?1锟?7锟?1锟?7锟?1锟?7
  osDelay(250);//锟?1锟?7锟?0锟?9锟?1锟?7250ms
  }
  /* USER CODE END LED_Thread2 */
}
#endif

#define LOWEST_PRIORITY   1  // ???????
#define HIGHEST_PRIORITY  2


//uint8_t rx_data_counter,rx_end_flag;
//
//uint8_t notify_counter;
//
//uint8_t wifi_counter;
//uint8_t state;
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
     /* 鍒涘缓浠诲姟 */
	   AppTaskCreate();
	  
	  /* 鍒涘缓浠诲姟閫氫俊鏈哄埗 */
//	   AppObjCreate();
	  
	  /* 鍚姩璋冨害锛屽紑濮嬫墽琛屼换锟?1锟?7?1锟?1锟?7?7 */
	   vTaskStartScheduler();


}
/**
 * @brief  :  static void vTaskStart(void *pvParameters)创建数据处理任务，优先级为中等
 * @note    任务内部使用队列接收数据，需先初始化队列
 * @param   None
 * @retval  None
 */
static void vTaskMsgPro(void *pvParameters)
{
  
	BaseType_t xResult;
	const TickType_t xMaxBlockTime = pdMS_TO_TICKS(3000); /* 1.?????-?????????50ms */
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
                   parse_recieve_data_handler();//receive_data_from_display(gl_tMsg.usData);

				 	vTaskPrioritySet(xHandleTaskMsgPro, LOWEST_PRIORITY);  // ???????
	       			taskYIELD();  // ??????
	    			vTaskPrioritySet(xHandleTaskStart,HIGHEST_PRIORITY);  // ???????

                  
             }
			 
                
         }
//		 else{
//		 /  printf("vTaskMsgPro run2 !!!\r\n");

//		 }
				                                   
 	}
}	

/**
 * @brief  :  static void vTaskStart(void *pvParameters)创建数据处理任务，优先级为中等
 * @note    任务内部使用队列接收数据，需先初始化队列
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
            power_on_handler();
           
            link_wifi_to_tencent_handler(gpro_t.wifi_led_fast_blink_flag); //detected ADC of value 
        
		    if(gpro_t.answer_buzzer_flag == 1){ //WT.EDIT 2025.07.28 

				SendWifiData_Answer_Cmd(0x16,0x01); //WT.EDIT 2025.07.28
				vTaskDelay(pdMS_TO_TICKS(10));
		   	}

	

		  
          break;

            case power_off:

      
              gpro_t.process_run_step=0;
              //gl_tMsg.link_wifi_net_flag=0;
              power_off_handler();
             break;
          }

          if(gpro_t.process_run_step > 9){

		      gpro_t.process_run_step=6;

		  }
          else if(gpro_t.wifi_led_fast_blink_flag > 1){
		  	 gpro_t.wifi_led_fast_blink_flag=0;
			
		  }
		  else if(gpro_t.wifi_led_fast_blink_flag==0 ){
		  	  //  wifi_counter++;
             wifi_communication_tnecent_handler();//
             getBeijingTime_cofirmLinkNetState_handler();
             wifi_auto_detected_link_state();
           }
		  
		// printf("vTaskStart run !!!\r\n");
		
		  vTaskDelay(pdMS_TO_TICKS(100));//锟?1锟?7?0


        }
       
    }
 
       

  
/**
 * @brief  :  void AppTaskCreate (void)建数据处理任务，优先级为中等
 * @note    任务内部使用队列接收数据，需先初始化队列
 * @param   None
 * @retval  None
 */
void AppTaskCreate (void)
{
    
 
  xTaskCreate( vTaskMsgPro,     		/* 浠诲姟鍑芥暟  */
                 "vTaskMsgPro",   		/* 浠诲姟锟?1锟?7?1锟?1锟?7?7    */
                 128,            		/* 浠诲姟鏍堝ぇ灏忥紝鍗曚綅word锛屼篃灏辨槸4瀛楄妭 */
                 NULL,           		/* 浠诲姟鍙傛暟  */
                 2,              		/* 浠诲姟浼樺厛锟?1锟?7?1锟?1锟?7?7 鏁帮拷1锟?7锟?1锟?7瓒婂皬浼樺厛绾ц秺浣庯紝杩欎釜璺焨COS鐩稿弽 */
                 &xHandleTaskMsgPro);   /* 浠诲姟鍙ユ焺  */


   xTaskCreate( vTaskStart,     		/* 浠诲姟鍑芥暟  */
                 "vTaskStart",   		/* 浠诲姟锟?1锟?7?1锟?1锟?7?7    */
                 128,            		/* 浠诲姟鏍堝ぇ灏忥紝鍗曚綅word锛屼篃灏辨槸4瀛楄妭 */
                 NULL,           		/* 浠诲姟鍙傛暟  */
                 1,              		/* 浠诲姟浼樺厛锟?1锟?7?1锟?1锟?7?7 鏁帮拷1锟?7锟?1锟?7瓒婂皬浼樺厛绾ц秺浣庯紝杩欎釜璺焨COS鐩稿弽 */
                 &xHandleTaskStart );   /* 浠诲姟鍙ユ焺  */
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
