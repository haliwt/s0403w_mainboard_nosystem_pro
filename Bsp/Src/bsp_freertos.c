#include "bsp.h"

#define DECODER_BIT_0        (1<< 0)

#define BIT_1                (1<<1)

#define BIT_2                (1<<2)



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
//static QueueHandle_t xQueue1 = NULL;




#define LOWEST_PRIORITY   1  // ???????
#define HIGHEST_PRIORITY  3




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
	 //  AppObjCreate();
	  
	  /* 启动调度，开始执行任�?1�?7?1�?1�?7?7 */
	   vTaskStartScheduler();


}
 
/**
 * @brief  :  static void vTaskStart(void *pvParameters
 * @note    
 * @param   None
 * @retval  None
 */

static void vTaskMsgPro(void *pvParameters)
{
  
	BaseType_t xResult;
	const TickType_t xMaxBlockTime = pdMS_TO_TICKS(1000); /* 1.?????-?????????50ms */
   uint32_t ulValue;
    
	
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

			//ack_handler();

		
	

			if(gpro_t.process_run_step > 10)gpro_t.process_run_step=6; //WT.EDIT 2025.10.07
		   
		

			if(gpro_t.answer_buzzer_flag > 1){
				if(gpro_t.answer_buzzer_flag == 1)gpro_t.answer_buzzer_flag =0;
				//if(gpro_t.stopTwoHours_flag > 1)gpro_t.stopTwoHours_flag=0;//WT.EDIT 2025.10.29
				
            }
			else if(gpro_t.answer_buzzer_flag == 1){ //WT.EDIT 2025.07.28 
				gpro_t.answer_buzzer_flag =0;

				SendWifiData_Answer_Cmd(0x16,0x01); //WT.EDIT 2025.07.28
				vTaskDelay(pdMS_TO_TICKS(10));
		   	}
		  break;

		  

          case power_off:
		  
              gpro_t.process_run_step=0;
	          gpro_t.soft_version =0; //WT.EDIT 2025.10.31
		     power_off_handler();
             break;
          }

           if(gpro_t.process_run_step > 10){

		      gpro_t.process_run_step=6;
			  

		  }
          else if(gpro_t.wifi_led_fast_blink_flag > 1){
		  	 gpro_t.wifi_led_fast_blink_flag=0;
			
		  }
		  
		  if(gpro_t.wifi_led_fast_blink_flag==0 ){
             wifi_communication_tnecent_handler();//
        
             getBeijingTime_cofirmLinkNetState_handler();
	
             wifi_auto_detected_link_state();
		
           }
		  


		//  waiting_ack_handler();
     
       vTaskDelay(10);
		
		}
      
 }

 /**
  * @brief	:  static void vTaskStart(void *pvParameters
  * @note	 
  * @param	 None
  * @retval  None
  */
 
 static void vTaskStart(void *pvParameters)
 {
   
	 while(1)
	 {
	    decoder_handler();
		vTaskDelay(20);

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
   
  #if 0
	xTaskCreate( vTaskWifiPro,		   /* 任务函数	*/
					"vTaskWifiPro",		   /* 任务�?1�?7?1�?1�?7?7	*/
					128,				   /* 任务栈大小，单位word，也就是4字节 */
					NULL,				   /* 任务参数	*/
					2,					   /* 任务优先�?1�?7?1�?1�?7?7 数�1�?7�?1�?7越小优先级越低，这个跟uCOS相反 */
					&xHandleTaskWifiPro );   /* 任务句柄	*/

   #endif 

	xTaskCreate( vTaskMsgPro,     		/* 任务函数  */
                 "vTaskMsgPro",   		/* 任务�?1�?7?1�?1�?7?7    */
                 128,            		/* 任务栈大小，单位word，也就是4字节 */
                 NULL,           		/* 任务参数  */
                 1,              		/* 任务优先�?1�?7?1�?1�?7?7 数�1�?7�?1�?7越小优先级越低，这个跟uCOS相反 */
                 &xHandleTaskMsgPro);   /* 任务句柄  */

 
  #if 1
   xTaskCreate( vTaskStart,     		/* 任务函数  */
                 "vTaskStart",   		/* 任务�?1�?7?1�?1�?7?7    */
                 128,            		/* 任务栈大小，单位word，也就是4字节 */
                 NULL,           		/* 任务参数  */
                 2,              		/* 任务优先�?1�?7?1�?1�?7?7 数�1�?7�?1�?7越小优先级越低，这个跟uCOS相反 */
                 &xHandleTaskStart );   /* 任务句柄  */
   #endif 
 
}
/*
*********************************************************************************************************
*	函 数 名: AppObjCreate
*	功能说明: 创建任务通信机制
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
//static void AppObjCreate (void)
//{
//	/* 创建10个uint8_t型消息队列 */
//	xQueue1 = xQueueCreate(10, sizeof(uint8_t));
//    if( xQueue1 == 0 )
//    {
//        /* 没有创建成功，用户可以在这里加入创建失败的处理机制 */
//    }
	
////	/* 创建10个存储指针变量的消息队列，由于CM3/CM4内核是32位机，一个指针变量占用4个字节 */
////	xQueue2 = xQueueCreate(10, sizeof(struct Msg *));
////    if( xQueue2 == 0 )
////    {
////        /* 没有创建成功，用户可以在这里加入创建失败的处理机制 */
////    }
//}


/********************************************************************************
	**
	*Function Name:void freertos_set_prority(void)
	*Function :
	*Input Ref: 
	*Return Ref:NO
	*
*******************************************************************************/


