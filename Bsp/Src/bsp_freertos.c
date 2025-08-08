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
typedef struct Msg
{
	uint8_t  ucMessageID;
	uint8_t usData[12];
	uint8_t link_wifi_net_flag;
}MSG_T;

MSG_T   gl_tMsg; /* ?????????????? */

uint8_t rx_data_counter,rx_end_flag;

uint8_t notify_counter;

uint8_t wifi_counter;
uint8_t state;


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

/**********************************************************************************************************
*	Function Name: vTaskStart
*	鍔熻兘璇存槑: 鍚姩浠诲姟锛屼篃灏辨槸鏈拷1锟?7楂樹紭鍏堢骇浠诲姟锛岃繖閲岀敤浣滄寜閿壂鎻忥拷1锟?7锟?1锟?7?1锟?1锟?7?7
*	锟?1锟?7?1锟?1锟?7?7    锟?1锟?7?1锟?1锟?7?7: pvParameters 鏄湪鍒涘缓璇ヤ换鍔℃椂浼狅拷1锟?7锟?1锟?7鐨勫舰鍙?
*	锟?1锟?7?1锟?1锟?7?7 锟?1锟?7?1锟?1锟?7?7 锟?1锟?7?1锟?1锟?7?7: 锟?1锟?7?1锟?1锟?7?7
*   锟?1锟?7?1锟?1锟?7?7 锟?1锟?7?1锟?1锟?7?7 锟?1锟?7?1锟?1锟?7?7: 4  (鏁帮拷1锟?7锟?1锟?7瓒婂皬浼樺厛绾ц秺浣庯紝杩欎釜璺焨COS鐩稿弽)
************************************ifi**********************************************************************/
// static void vTaskWifiPro(void *pvParameters)
// {
  
//      while(1)
//      {

//        if(gpro_t.wifi_led_fast_blink_flag==0 ){
// 		  	     wifi_counter++;
//              wifi_communication_tnecent_handler();//
//              getBeijingTime_cofirmLinkNetState_handler();
//              wifi_auto_detected_link_state();
//        }
//        vTaskDelay(1000);
//     }


// }

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
                gpro_t.disp_rx_cmd_done_flag = 0;
				rx_data_counter=0;
				state=0;

               
                 receive_data_from_display(gl_tMsg.usData);

				 	vTaskPrioritySet(xHandleTaskMsgPro, LOWEST_PRIORITY);  // ???????
	       taskYIELD();  // ??????
	    vTaskPrioritySet(xHandleTaskStart,HIGHEST_PRIORITY);  // ???????

                  
             }
			 
                
         }
//		 else{
//		 	
////		      xTaskNotify(xHandleTaskStart, /* 目标任务 */
////								BIT_1,             /* 设置目标任务事件标志位bit0  */
////				 				eSetBits);         /* 将目标任务的事件标志位与BIT_0进行或操作�* */
//		
//		}
				                                   
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
    static uint8_t power_on_sound_flag ;
	BaseType_t xResult;
	const TickType_t xMaxBlockTime = pdMS_TO_TICKS(100); /* 设置最大等待时间为500ms */
	uint32_t ulValue;
	while(1)
    {
  
       	if(power_on_sound_flag==0){
            power_on_sound_flag ++;
            FAN_Stop();  //WT.EDIT.2025.01.03
            buzzer_sound();//buzzer_sound();

        }


//		xResult = xTaskNotifyWait(0x00000000,      
//						          0xFFFFFFFF,      
//						          &ulValue,        /* 保存ulNotifiedValue到变量ulValue中 */
//						          xMaxBlockTime);  /* 最大允许延迟时间 */
//		
//		if( xResult == pdPASS )
//		{
//			/* 接收到消息，检测那个位被按下 */
//			if((ulValue & BIT_1) != 0)
//			{
//              notify_counter++; 
//			}
//		}
//    else{

          switch(gpro_t.gpower_on){ 

            case power_on:
            power_on_handler();
           
            link_wifi_to_tencent_handler(gpro_t.wifi_led_fast_blink_flag); //detected ADC of value 
        
		    if(gpro_t.answer_buzzer_flag == 1){ //WT.EDIT 2025.07.28 

				SendWifiData_Answer_Cmd(0x16,0x01); //WT.EDIT 2025.07.28
				vTaskDelay(pdMS_TO_TICKS(5));
		   	}

		  
          break;

            case power_off:

      
              gpro_t.process_run_step=0;
              gl_tMsg.link_wifi_net_flag=0;
              power_off_handler();
             break;
          }


          if(gpro_t.wifi_led_fast_blink_flag > 1){
		  	 

		  	   gpro_t.wifi_led_fast_blink_flag=0;
			  //vTaskDelay(pdMS_TO_TICKS(1000));//3000
			  

          }
		  else if(gpro_t.wifi_led_fast_blink_flag==0 ){
		  	    wifi_counter++;
             wifi_communication_tnecent_handler();//
             getBeijingTime_cofirmLinkNetState_handler();
             wifi_auto_detected_link_state();
           }
		  
		
			
          //send_cmd_ack_hanlder();
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
                 256,            		/* 浠诲姟鏍堝ぇ灏忥紝鍗曚綅word锛屼篃灏辨槸4瀛楄妭 */
                 NULL,           		/* 浠诲姟鍙傛暟  */
                 1,              		/* 浠诲姟浼樺厛锟?1锟?7?1锟?1锟?7?7 鏁帮拷1锟?7锟?1锟?7瓒婂皬浼樺厛绾ц秺浣庯紝杩欎釜璺焨COS鐩稿弽 */
                 &xHandleTaskStart );   /* 浠诲姟鍙ユ焺  */
}


/********************************************************************************
	**
	*Function Name:void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
	*Function :UART callback function  for UART interrupt for receive data
	*Input Ref: structure UART_HandleTypeDef pointer
	*Return Ref:NO
	*
*******************************************************************************/
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
     
     BaseType_t xHigherPriorityTaskWoken = pdFALSE;
   //  MSG_T *ptMsg;

    if(huart->Instance==USART2)
    {
	
   //  DISABLE_INT();
     if(net_t.linking_tencent_cloud_doing ==1){

			gpro_t.wifi_rx_data_array[gpro_t.wifi_rx_data_counter] =wifi_rx_inputBuf[0];
			gpro_t.wifi_rx_data_counter++;

			if(*wifi_rx_inputBuf==0x0A) // 0x0A = "\n"
			{
				
				Wifi_Rx_InputInfo_Handler();
				gpro_t.wifi_rx_data_counter=0;
			}

	 } 
     else{

		    if(wifi_t.get_rx_beijing_time_enable==1){
					gpro_t.wifi_rx_data_array[gpro_t.wifi_rx_data_counter] = wifi_rx_inputBuf[0];
					gpro_t.wifi_rx_data_counter++;
					
			}
			else
			    Subscribe_Rx_Interrupt_Handler();
	 }
	
     //  ENABLE_INT();
	  __HAL_UART_CLEAR_OREFLAG(&huart2);
      HAL_UART_Receive_IT(&huart2,wifi_rx_inputBuf,1);
	}
    else if(huart->Instance==USART1)//if(huart==&huart1) // Motor Board receive data (filter)
	{
      // DISABLE_INT();
		switch(state)
		{
		case 0:  //#0
			if(inputBuf[0] == 0xA5){  // 0xA5 --didplay command head
               rx_data_counter=0;
               gl_tMsg.usData[rx_data_counter] = inputBuf[0];
			   state=1; //=1

             }
            else
                state=0;
		break;

       
		case 1: //#1

            if(gpro_t.disp_rx_cmd_done_flag ==0){
              /* ???????? */
               rx_data_counter++;
		     
	          gl_tMsg.usData[rx_data_counter] = inputBuf[0];
              

               
              if(gl_tMsg.usData[rx_data_counter] ==0xFE && rx_end_flag == 0 &&  rx_data_counter > 4){
                     
				     gpro_t.disp_rx_cmd_done_flag = 0 ;
         

                state = 0;
            
                //uid = rx_data_counter;
                //rx_end_flag=0;

                rx_data_counter =0;

             
               // state=0;

                //bcc_check_code=inputBuf[0];

               // gpro_t.gTimer_rx_cmd_done =0;

                #if 1

                xTaskNotifyFromISR(xHandleTaskMsgPro,  /* ???? */
                DECODER_BIT_0,     /* ???????????bit0  */
                eSetBits,  /* ????????????BIT_0?????, ??????????? */
                &xHigherPriorityTaskWoken);

                /* ??xHigherPriorityTaskWoken = pdTRUE,???????????????????? */
                portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
                #endif 
                  
              }

              }


        break;


			
		}

      //  ENABLE_INT();
	    __HAL_UART_CLEAR_OREFLAG(&huart1);
		HAL_UART_Receive_IT(&huart1,inputBuf,1);//UART receive data interrupt 1 byte
		
	 }
    
    
  
 }


void freertos_set_prority(void)
{
	
	vTaskPrioritySet(xHandleTaskStart, LOWEST_PRIORITY);  // ???????
	taskYIELD();  // ??????
	vTaskPrioritySet(xHandleTaskMsgPro,HIGHEST_PRIORITY);  // ???????

   
} 


