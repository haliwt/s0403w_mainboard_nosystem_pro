#include "bsp.h"

#define DECODER_BIT_0        (1<< 0)

#define BIT_1                (1<<1)

#define BIT_2                (1<<2)

#define LOWEST_PRIORITY   1  // ???????
#define HIGHEST_PRIORITY  3


/***********************************************************************************************************
											函数声明
***********************************************************************************************************/
#if 0  //动态创建 内存
//static void vTaskWifiPro(void *pvParameters);
static void vTaskMsgPro(void *pvParameters);
static void vTaskStart(void *pvParameters);
#else
/*------------------ 静态任务内存定义 ------------------*/

/* vTaskMsgPro 任务 */
//static StaticTask_t xTaskMsgProTCB;
//static StackType_t xTaskMsgProStack[640];//640

/* vTaskStart 任务 */
static StaticTask_t xTaskStartTCB;
static StackType_t xTaskStartStack[896];//896



#endif 
static void AppTaskCreate (void);
//xTaskCreateStatic()


/* 创建任务通信机制 */
//static void AppObjCreate(void);
static void power_run_handler(void);
static void wifi_run_handler(void);


/***********************************************************************************************************
											变量声明
***********************************************************************************************************/
//static TaskHandle_t xHandleTaskWifiPro = NULL;
static TaskHandle_t xHandleTaskMsgPro = NULL;
static TaskHandle_t xHandleTaskStart = NULL;
//static QueueHandle_t xQueue1 = NULL;

/* 定义静态内存块 */
static StaticTask_t xIdleTaskTCB;
static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];


/* 内核会自动调用这个回调函数来获取 Idle 任务的内存 */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer,
                                    StackType_t **ppxIdleTaskStackBuffer,
                                    uint32_t *pulIdleTaskStackSize )
{
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}




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
#if 0
static void vTaskMsgPro(void *pvParameters)
{
  
	while(1)
    {

//		if(power_on_sound_flag==0){
//            power_on_sound_flag ++;
//            FAN_Stop();  //WT.EDIT.2025.01.03
//            buzzer_sound();//buzzer_sound();
		

//        }


		
      
	     power_run_handler();
       
         wifi_run_handler();

        
		 vTaskDelay(2000);//100
		
	}
      
 }
#endif 
 /**
  * @brief	:  static void vTaskStart(void *pvParameters
  * @note	 
  * @param	 None
  * @retval  None
  */
 
 static void vTaskStart(void *pvParameters)
 {
    BaseType_t xResult;
	const TickType_t xMaxBlockTime = pdMS_TO_TICKS(200); /* 设置�?大等待时间为100ms */
	uint32_t ulValue;
	 while(1)
	 {


       	if(power_on_sound_flag==0){
            power_on_sound_flag ++;
            FAN_Stop();  //WT.EDIT.2025.01.03
            buzzer_sound();//buzzer_sound();
		

        }
	 
		xResult = xTaskNotifyWait(0x00000000,	   
								  0xFFFFFFFF,   
								  &ulValue,		  /* 保存ulNotifiedValue到变量ulValue�? */
								  xMaxBlockTime);  /*  portMAX_DELAY */

		if( xResult == pdPASS ){
			
			if((ulValue & DECODER_BIT_0) != 0){
			   decoder_handler();
			}
		}
		else{
         power_run_handler();
       
         wifi_run_handler();

		}
		
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
   
 /*------------------ 静态任务创建 ------------------*/
	
//	xHandleTaskMsgPro = xTaskCreateStatic(
//			vTaskMsgPro,			/* 任务函数 */
//			"vTaskMsgPro",			/* 任务名 */
//			640,					/* 栈大小（word） */
//			NULL,					/* 参数 */
//			2,						/* 优先级 */
//			xTaskMsgProStack,		/* 栈数组 */
//			&xTaskMsgProTCB 		/* TCB */
//	);
	
	xHandleTaskStart = xTaskCreateStatic(
			vTaskStart, 			/* 任务函数 */
			"vTaskStart",			/* 任务名 */
			896,					/* 栈大小（word） */
			NULL,					/* 参数 */
			1,						/* 优先级 */
			xTaskStartStack,		/* 栈数组 */
			&xTaskStartTCB			/* TCB */
	);
 
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
	*Function Name:static void power_run_handler(void)
	*Function :
	*Input Ref: 
	*Return Ref:NO
	*
*******************************************************************************/
static void power_run_handler(void)
{
    switch(gpro_t.gpower_on){ 

            case power_on:
		 
			
            power_on_handler();
            link_wifi_to_tencent_handler(); //detected ADC of value 
            ai_mode_display_fun();
			if(gpro_t.stopTwoHours_flag ==0){
			   Fan_RunSpeed_Fun();
		    }

			//ack_handler();
            if(gpro_t.process_run_step > 13 || gpro_t.stopTwoHours_flag > 1 || gpro_t.soft_version > 2){
				 if(gpro_t.process_run_step > 13 )gpro_t.process_run_step=6; //WT.EDIT 2025.10.07
				 if(gpro_t.stopTwoHours_flag > 1 )gpro_t.stopTwoHours_flag =0;
				 if(gpro_t.soft_version > 2)gpro_t.soft_version =0;
            }
		   
			
		  break;

		  

          case power_off:
		  
          
		      power_off_handler();
             break;
          }


}

/********************************************************************************
	**
	*Function Name:static void wifi_run_handler(void)
	*Function :
	*Input Ref: 
	*Return Ref:NO
	*
*******************************************************************************/
static void wifi_run_handler(void)
{
      if(gpro_t.process_run_step > 10){

		      gpro_t.process_run_step=6;
			  

		  }
          //else if(gpro_t.wifi_led_fast_blink_flag > 1){
		  	 //gpro_t.wifi_led_fast_blink_flag=0;
			
		  //}
		  
		  if(gpro_t.wifi_led_fast_blink_flag==0 ){
             wifi_communication_tnecent_handler();//
        
             getBeijingTime_cofirmLinkNetState_handler();
	
             wifi_auto_detected_link_state();
		
           }

}


void display_board_xtask_notice(void)
{

        BaseType_t xHigherPriorityTaskWoken = pdFALSE;

        xTaskNotifyFromISR(xHandleTaskStart,  /* 目标任务 */
					         DECODER_BIT_0,     /* 设置目标任务事件标志位bit0  */
					         eSetBits,  /* 将目标任务的事件标志位与BIT_0进行或操作， 将结果赋值给事件标志�? */
					         &xHigherPriorityTaskWoken);

         /* 如果xHigherPriorityTaskWoken = pdTRUE，那么�??出中断后切到当前�?高优先级任务执行 */
         portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

}


