#include "bsp.h"

#define DECODER_BIT_0        (1<< 0)

/***********************************************************************************************************
											å‡½æ•°å£°æ˜Ž
***********************************************************************************************************/
//static void vTaskWifiPro(void *pvParameters);
static void vTaskMsgPro(void *pvParameters);
static void vTaskStart(void *pvParameters);
static void AppTaskCreate (void);


/* åˆ›å»ºä»»åŠ¡é€šä¿¡æœºåˆ¶ */
//static void AppObjCreate(void);


/***********************************************************************************************************
											å˜é‡å£°æ˜Ž
***********************************************************************************************************/
//static TaskHandle_t xHandleTaskWifiPro = NULL;
static TaskHandle_t xHandleTaskMsgPro = NULL;
static TaskHandle_t xHandleTaskStart = NULL;


#if 0
//LEDï¿?1ï¿?7ï¿?1ï¿?7ï¿?0ï¿?8ï¿?1ï¿?7ï¿?1ï¿?7ï¿?1ï¿?7ï¿?1ï¿?71
void LED_Thread1(void const * argument)
{

  /* USER CODE BEGIN 5 */
  (void) argument;
  /* Infinite loop */
  for (;;)
  {
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);//PB4ï¿?1ï¿?7ï¿?1ï¿?7ï¿?1ï¿?7ï¿?1ï¿?7ï¿?1ï¿?7
  osDelay(100);//ï¿?1ï¿?7ï¿?0ï¿?9ï¿?1ï¿?7100ms
 
  }
  /* USER CODE END 5 */ 
}

//LEDï¿?1ï¿?7ï¿?1ï¿?7ï¿?0ï¿?8ï¿?1ï¿?7ï¿?1ï¿?7ï¿?1ï¿?7ï¿?1ï¿?72
void LED_Thread2(void const * argument)
{
  /* USER CODE BEGIN LED_Thread2 */
  (void) argument;
  /* Infinite loop */
  for (;;)
  {
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);//PB4ï¿?1ï¿?7ï¿?1ï¿?7ï¿?1ï¿?7ï¿?1ï¿?7ï¿?1ï¿?7
  osDelay(250);//ï¿?1ï¿?7ï¿?0ï¿?9ï¿?1ï¿?7250ms
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

//uint8_t test_counter;

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
     /* åˆ›å»ºä»»åŠ¡ */
	  AppTaskCreate();
	  
	  /* åˆ›å»ºä»»åŠ¡é€šä¿¡æœºåˆ¶ */
//	   AppObjCreate();
	  
	  /* å¯åŠ¨è°ƒåº¦ï¼Œå¼€å§‹æ‰§è¡Œä»»ï¿?1ï¿?7?1ï¿?1ï¿?7?7 */
	   vTaskStartScheduler();


}

/**********************************************************************************************************
*	Function Name: vTaskStart
*	åŠŸèƒ½è¯´æ˜Ž: å¯åŠ¨ä»»åŠ¡ï¼Œä¹Ÿå°±æ˜¯æœ¢ï¿½1ï¿?7é«˜ä¼˜å…ˆçº§ä»»åŠ¡ï¼Œè¿™é‡Œç”¨ä½œæŒ‰é”®æ‰«æï¿½1ï¿?7ï¿?1ï¿?7?1ï¿?1ï¿?7?7
*	ï¿?1ï¿?7?1ï¿?1ï¿?7?7    ï¿?1ï¿?7?1ï¿?1ï¿?7?7: pvParameters æ˜¯åœ¨åˆ›å»ºè¯¥ä»»åŠ¡æ—¶ä¼ ï¿½1ï¿?7ï¿?1ï¿?7çš„å½¢å?
*	ï¿?1ï¿?7?1ï¿?1ï¿?7?7 ï¿?1ï¿?7?1ï¿?1ï¿?7?7 ï¿?1ï¿?7?1ï¿?1ï¿?7?7: ï¿?1ï¿?7?1ï¿?1ï¿?7?7
*   ï¿?1ï¿?7?1ï¿?1ï¿?7?7 ï¿?1ï¿?7?1ï¿?1ï¿?7?7 ï¿?1ï¿?7?1ï¿?1ï¿?7?7: 4  (æ•°ï¿½1ï¿?7ï¿?1ï¿?7è¶Šå°ä¼˜å…ˆçº§è¶Šä½Žï¼Œè¿™ä¸ªè·ŸuCOSç›¸å)
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
/**********************************************************************************************************
*	Function Name: vTaskStart
*	åŠŸèƒ½è¯´æ˜Ž: å¯åŠ¨ä»»åŠ¡ï¼Œä¹Ÿå°±æ˜¯æœ¢ï¿½1ï¿?7é«˜ä¼˜å…ˆçº§ä»»åŠ¡ï¼Œè¿™é‡Œç”¨ä½œæŒ‰é”®æ‰«æï¿½1ï¿?7ï¿?1ï¿?7?1ï¿?1ï¿?7?7
*	ï¿?1ï¿?7?1ï¿?1ï¿?7?7    ï¿?1ï¿?7?1ï¿?1ï¿?7?7: pvParameters æ˜¯åœ¨åˆ›å»ºè¯¥ä»»åŠ¡æ—¶ä¼ ï¿½1ï¿?7ï¿?1ï¿?7çš„å½¢å?
*	ï¿?1ï¿?7?1ï¿?1ï¿?7?7 ï¿?1ï¿?7?1ï¿?1ï¿?7?7 ï¿?1ï¿?7?1ï¿?1ï¿?7?7: ï¿?1ï¿?7?1ï¿?1ï¿?7?7
*   ï¿?1ï¿?7?1ï¿?1ï¿?7?7 ï¿?1ï¿?7?1ï¿?1ï¿?7?7 ï¿?1ï¿?7?1ï¿?1ï¿?7?7: 4  (æ•°ï¿½1ï¿?7ï¿?1ï¿?7è¶Šå°ä¼˜å…ˆçº§è¶Šä½Žï¼Œè¿™ä¸ªè·ŸuCOSç›¸å)
**********************************************************************************************************/
static void vTaskMsgPro(void *pvParameters)
{
  
	BaseType_t xResult;
	//const TickType_t xMaxBlockTime = pdMS_TO_TICKS(1000); /* 1.?????-?????????50ms */
    uint32_t ulValue;
    
	
    while(1)
    {
       xResult = xTaskNotifyWait(0x00000000,      
						           0xFFFFFFFF,      
						          &ulValue,        /* ??ulNotifiedValue???ulValue? */
						          portMAX_DELAY);  /* ????????,????-block portMAX_DELAY */
        if(xResult == pdPASS){
             if((ulValue & DECODER_BIT_0 ) != 0)
             {
                gpro_t.disp_rx_cmd_done_flag = 0;
				rx_data_counter=0;
				state=0;
				//gpro_t.gTimer_rx_cmd_done =0;
              
             //   check_code =  bcc_check(gl_tMsg.usData,uid);

              // if(check_code == bcc_check_code ){
               
                  receive_data_from_display(gl_tMsg.usData);
               
                  
			      vTaskPrioritySet(xHandleTaskMsgPro , LOWEST_PRIORITY);  // ???????
			      taskYIELD();  // ??????
			      vTaskPrioritySet(xHandleTaskStart, HIGHEST_PRIORITY);  // ???????
			     // vTaskDelay(pdMS_TO_TICKS(10));
                  
                
                }
                
         }


	} 
}
       

/**********************************************************************************************************
*	ï¿?1ï¿?7?1ï¿?1ï¿?7?7 ï¿?1ï¿?7?1ï¿?1ï¿?7?7 ï¿?1ï¿?7?1ï¿?1ï¿?7?7: vTaskStart
*	åŠŸèƒ½è¯´æ˜Ž: å¯åŠ¨ä»»åŠ¡ï¼Œä¹Ÿå°±æ˜¯æœ¢ï¿½1ï¿?7é«˜ä¼˜å…ˆçº§ä»»åŠ¡ï¼Œè¿™é‡Œç”¨ä½œæŒ‰é”®æ‰«æï¿½1ï¿?7ï¿?1ï¿?7?1ï¿?1ï¿?7?7
*	ï¿?1ï¿?7?1ï¿?1ï¿?7?7    ï¿?1ï¿?7?1ï¿?1ï¿?7?7: pvParameters æ˜¯åœ¨åˆ›å»ºè¯¥ä»»åŠ¡æ—¶ä¼ ï¿½1ï¿?7ï¿?1ï¿?7çš„å½¢å?
*	ï¿?1ï¿?7?1ï¿?1ï¿?7?7 ï¿?1ï¿?7?1ï¿?1ï¿?7?7 ï¿?1ï¿?7?1ï¿?1ï¿?7?7: ï¿?1ï¿?7?1ï¿?1ï¿?7?7
*   ï¿?1ï¿?7?1ï¿?1ï¿?7?7 ï¿?1ï¿?7?1ï¿?1ï¿?7?7 ï¿?1ï¿?7?1ï¿?1ï¿?7?7: 4  (æ•°ï¿½1ï¿?7ï¿?1ï¿?7è¶Šå°ä¼˜å…ˆçº§è¶Šä½Žï¼Œè¿™ä¸ªè·ŸuCOSç›¸å)
**********************************************************************************************************/
static void vTaskStart(void *pvParameters)
{
    static uint8_t power_on_sound_flag ;
	while(1)
    {
  
       	if(power_on_sound_flag==0){
            power_on_sound_flag ++;
            FAN_Stop();  //WT.EDIT.2025.01.03
            buzzer_sound();//buzzer_sound();

        };

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
			  if(gpro_t.power_on_prority_flag ==1){
                  gpro_t.power_on_prority_flag ++;
			   vTaskPrioritySet(xHandleTaskStart, LOWEST_PRIORITY);  // ???????
			   taskYIELD();  // ??????
			    vTaskPrioritySet(xHandleTaskMsgPro,HIGHEST_PRIORITY);  // ???????
           
			  }
      
              gpro_t.process_run_step=0;
              gl_tMsg.link_wifi_net_flag=0;
              power_off_handler();
             break;
          }


          if(gpro_t.wifi_led_fast_blink_flag > 1){
		  	 
			  vTaskPrioritySet(xHandleTaskStart , LOWEST_PRIORITY);  // ???????
			   taskYIELD();  // ??????
			  vTaskPrioritySet(xHandleTaskMsgPro,HIGHEST_PRIORITY);  // ???????
		  	   gpro_t.wifi_led_fast_blink_flag=0;
			  vTaskDelay(pdMS_TO_TICKS(1000));//3000
			  

          }
		  else if(gpro_t.wifi_led_fast_blink_flag==0 ){
		  	    wifi_counter++;
             wifi_communication_tnecent_handler();//
             getBeijingTime_cofirmLinkNetState_handler();
             wifi_auto_detected_link_state();
           }
		  
		
			
          send_cmd_ack_hanlder();
		  vTaskDelay(pdMS_TO_TICKS(100));//ï¿?1ï¿?7?0


        }
       
		
	
 }
       

  
 /*
*********************************************************************************************************
*	ï¿?1ï¿?7?1ï¿?1ï¿?7?7 ï¿?1ï¿?7?1ï¿?1ï¿?7?7 ï¿?1ï¿?7?1ï¿?1ï¿?7?7: AppTaskCreate
*	åŠŸèƒ½è¯´æ˜Ž: åˆ›å»ºåº”ç”¨ä»»åŠ¡
*	ï¿?1ï¿?7?1ï¿?1ï¿?7?7    å‚ï¼šï¿?1ï¿?7?1ï¿?1ï¿?7?7
*	ï¿?1ï¿?7?1ï¿?1ï¿?7?7 ï¿?1ï¿?7?1ï¿?1ï¿?7?7 ï¿?1ï¿?7?1ï¿?1ï¿?7?7: ï¿?1ï¿?7?1ï¿?1ï¿?7?7
*********************************************************************************************************
*/
void AppTaskCreate (void)
{
    
 
  xTaskCreate( vTaskMsgPro,     		/* ä»»åŠ¡å‡½æ•°  */
                 "vTaskMsgPro",   		/* ä»»åŠ¡ï¿?1ï¿?7?1ï¿?1ï¿?7?7    */
                 128,            		/* ä»»åŠ¡æ ˆå¤§å°ï¼Œå•ä½wordï¼Œä¹Ÿå°±æ˜¯4å­—èŠ‚ */
                 NULL,           		/* ä»»åŠ¡å‚æ•°  */
                 2,              		/* ä»»åŠ¡ä¼˜å…ˆï¿?1ï¿?7?1ï¿?1ï¿?7?7 æ•°ï¿½1ï¿?7ï¿?1ï¿?7è¶Šå°ä¼˜å…ˆçº§è¶Šä½Žï¼Œè¿™ä¸ªè·ŸuCOSç›¸å */
                 &xHandleTaskMsgPro);   /* ä»»åŠ¡å¥æŸ„  */


   xTaskCreate( vTaskStart,     		/* ä»»åŠ¡å‡½æ•°  */
                 "vTaskStart",   		/* ä»»åŠ¡ï¿?1ï¿?7?1ï¿?1ï¿?7?7    */
                 128,            		/* ä»»åŠ¡æ ˆå¤§å°ï¼Œå•ä½wordï¼Œä¹Ÿå°±æ˜¯4å­—èŠ‚ */
                 NULL,           		/* ä»»åŠ¡å‚æ•°  */
                 1,              		/* ä»»åŠ¡ä¼˜å…ˆï¿?1ï¿?7?1ï¿?1ï¿?7?7 æ•°ï¿½1ï¿?7ï¿?1ï¿?7è¶Šå°ä¼˜å…ˆçº§è¶Šä½Žï¼Œè¿™ä¸ªè·ŸuCOSç›¸å */
                 &xHandleTaskStart );   /* ä»»åŠ¡å¥æŸ„  */
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


