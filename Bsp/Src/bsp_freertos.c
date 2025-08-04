#include "bsp.h"

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


#if 0
//LED��˸����1
void LED_Thread1(void const * argument)
{

  /* USER CODE BEGIN 5 */
  (void) argument;
  /* Infinite loop */
  for (;;)
  {
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);//PB4�����
  osDelay(100);//�ȴ�100ms
 
  }
  /* USER CODE END 5 */ 
}

//LED��˸����2
void LED_Thread2(void const * argument)
{
  /* USER CODE BEGIN LED_Thread2 */
  (void) argument;
  /* Infinite loop */
  for (;;)
  {
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);//PB4�����
  osDelay(250);//�ȴ�250ms
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

uint8_t test_counter;



uint8_t wifi_counter;

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
	  
	  /* 启动调度，开始执行任�?1�?7 */
	   vTaskStartScheduler();


}

/**********************************************************************************************************
*	�?1�?7 �?1�?7 �?1�?7: vTaskStart
*	功能说明: 启动任务，也就是朢�高优先级任务，这里用作按键扫描��?1�?7
*	�?1�?7    �?1�?7: pvParameters 是在创建该任务时传��的形参
*	�?1�?7 �?1�?7 �?1�?7: �?1�?7
*   �?1�?7 �?1�?7 �?1�?7: 4  (数��越小优先级越低，这个跟uCOS相反)
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
*	�?1�?7 �?1�?7 �?1�?7: vTaskStart
*	功能说明: 启动任务，也就是朢�高优先级任务，这里用作按键扫描��?1�?7
*	�?1�?7    �?1�?7: pvParameters 是在创建该任务时传��的形参
*	�?1�?7 �?1�?7 �?1�?7: �?1�?7
*   �?1�?7 �?1�?7 �?1�?7: 4  (数��越小优先级越低，这个跟uCOS相反)
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

             //   check_code =  bcc_check(gl_tMsg.usData,uid);

              // if(check_code == bcc_check_code ){
               
                  receive_data_fromm_display(gl_tMsg.usData);
               
         
			      vTaskPrioritySet(xHandleTaskMsgPro , LOWEST_PRIORITY);  // ???????
			      taskYIELD();  // ??????
			      vTaskPrioritySet(xHandleTaskStart, HIGHEST_PRIORITY);  // ???????
                  
                }
                
            }
        }
}
       

/**********************************************************************************************************
*	�?1�?7 �?1�?7 �?1�?7: vTaskStart
*	功能说明: 启动任务，也就是朢�高优先级任务，这里用作按键扫描��?1�?7
*	�?1�?7    �?1�?7: pvParameters 是在创建该任务时传��的形参
*	�?1�?7 �?1�?7 �?1�?7: �?1�?7
*   �?1�?7 �?1�?7 �?1�?7: 4  (数��越小优先级越低，这个跟uCOS相反)
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
            works_run_two_hours_state();
            link_wifi_to_tencent_handler(gpro_t.wifi_led_fast_blink_flag); //detected ADC of value 
            if(wifi_link_net_state() ==1 && gl_tMsg.link_wifi_net_flag ==0){
              gl_tMsg.link_wifi_net_flag ++;
              Update_Dht11_Totencent_Value();
             vTaskDelay(pdMS_TO_TICKS(10));//HAL_Delay(200) //WT.EDIT 2024.08.10
             }
		
          
            if(gpro_t.answer_buzzer_flag == 1){//WT.EDIT 2025.07.28
                gpro_t.answer_buzzer_flag ++;
			    SendWifiData_Answer_Cmd(0x16,0x01); //WT.EDIT 2025.07.28
			    vTaskDelay(pdMS_TO_TICKS(10));


            }

       
		    
			if(gpro_t.gTimer_update_todisplay > 1){
			 	gpro_t.gTimer_update_todisplay=0;
                updateDht11_sensorData_toDisp();
              
			 }
			test_counter++;
			
           break;

            case power_off:
      
              gpro_t.process_run_step=0;
              gl_tMsg.link_wifi_net_flag=0;
              power_off_handler();
             break;
          }


           if(gpro_t.wifi_led_fast_blink_flag==0 ){
		  	     wifi_counter++;
             wifi_communication_tnecent_handler();//
             getBeijingTime_cofirmLinkNetState_handler();
             wifi_auto_detected_link_state();
            }
		  
		  
          if(gpro_t.gTimer_update_todisplay > 1){
			 	    gpro_t.gTimer_update_todisplay=0;
                updateDht11_sensorData_toDisp();
              
		       }
			
          send_cmd_ack_hanlder();
		     vTaskDelay(20);//�ȴ�100ms


        }
       
		
	
 }
       

  
 /*
*********************************************************************************************************
*	�?1�?7 �?1�?7 �?1�?7: AppTaskCreate
*	功能说明: 创建应用任务
*	�?1�?7    参：�?1�?7
*	�?1�?7 �?1�?7 �?1�?7: �?1�?7
*********************************************************************************************************
*/
void AppTaskCreate (void)
{
    
 
  xTaskCreate( vTaskMsgPro,     		/* 任务函数  */
                 "vTaskMsgPro",   		/* 任务�?1�?7    */
                 128,            		/* 任务栈大小，单位word，也就是4字节 */
                 NULL,           		/* 任务参数  */
                 2,              		/* 任务优先�?1�?7 数��越小优先级越低，这个跟uCOS相反 */
                 &xHandleTaskMsgPro);   /* 任务句柄  */


   xTaskCreate( vTaskStart,     		/* 任务函数  */
                 "vTaskStart",   		/* 任务�?1�?7    */
                 128,            		/* 任务栈大小，单位word，也就是4字节 */
                 NULL,           		/* 任务参数  */
                 1,              		/* 任务优先�?1�?7 数��越小优先级越低，这个跟uCOS相反 */
                 &xHandleTaskStart );   /* 任务句柄  */
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
     static uint8_t state;
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
                     
				     gpro_t.disp_rx_cmd_done_flag = 1 ;
         

                state = 0;
            
                //uid = rx_data_counter;
                //rx_end_flag=0;

                rx_data_counter =0;

             
               // state=0;

                //bcc_check_code=inputBuf[0];

           

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


