#include "bsp.h"

#define DECODER_BIT_0        (1<< 0)

#define BIT_1                (1<<1)

#define BIT_2                (1<<2)

/***********************************************************************************************************
											函数声明
***********************************************************************************************************/
#if 0
//static void vTaskWifiPro(void *pvParameters);
static void vTaskMsgPro(void *pvParameters);
static void vTaskStart(void *pvParameters);
#else 
/*------------------ 静态任务内存定义 ------------------*/

/* vTaskMsgPro 任务 */
static StaticTask_t xTaskMsgProTCB;
static StackType_t xTaskMsgProStack[256];

/* vTaskStart 任务 */
static StaticTask_t xTaskStartTCB;
static StackType_t xTaskStartStack[128];


#endif 


static void AppTaskCreate (void);


/* 创建任务通信机制 */
//static void AppObjCreate(void);

static void power_handler(void);
static void wifi_handler(void);

/***********************************************************************************************************
											变量声明
***********************************************************************************************************/
//static TaskHandle_t xHandleTaskWifiPro = NULL;
static TaskHandle_t xHandleTaskMsgPro = NULL;
static TaskHandle_t xHandleTaskStart = NULL;

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
/**
 * @brief  :  static void vTaskStart(void *pvParameters
 * @note    
 * @param   None
 * @retval  None
 */
static void vTaskMsgPro(void *pvParameters)
{
  
	while(1)
    {

       	if(power_on_sound_flag==0){
            power_on_sound_flag ++;
            FAN_Stop();  //WT.EDIT.2025.01.03
            buzzer_sound();//buzzer_sound();
			//printf("buzzer_sound !!!\r\n");

        }

	    power_handler();
		wifi_handler();


	    vTaskDelay(pdMS_TO_TICKS(1000));//

				                                   
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
  
       
		
		 if(gpro_t.decoder_success_flag==1){
		   	   gpro_t.decoder_success_flag++; 
              usart1_protocol_state_machine();

          }
		 
	       vTaskDelay(pdMS_TO_TICKS(10));//
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
   	xTaskCreate( vTaskMsgPro,     		/* 任务函数  */
                 "vTaskMsgPro",   		/* 任务�?1�?7?1�?1�?7?7    */
                 128,            		/* 任务栈大小，单位word，也就是4字节 */
                 NULL,           		/* 任务参数  */
                 2,              		/* 任务优先�?1�?7?1�?1�?7?7 数�1�?7�?1�?7越小优先级越低，这个跟uCOS相反 */
                 &xHandleTaskMsgPro);   /* 任务句柄  */


	xTaskCreate( vTaskMsgPro,     		/* 任务函数  */
                 "vTaskMsgPro",   		/* 任务�?1�?7?1�?1�?7?7    */
                 128,            		/* 任务栈大小，单位word，也就是4字节 */
                 NULL,           		/* 任务参数  */
                 1,              		/* 任务优先�?1�?7?1�?1�?7?7 数�1�?7�?1�?7越小优先级越低，这个跟uCOS相反 */
                 &xHandleTaskMsgPro);   /* 任务句柄  */


   xTaskCreate( vTaskStart,     		/* 任务函数  */
                 "vTaskStart",   		/* 任务�?1�?7?1�?1�?7?7    */
                 256,            		/* 任务栈大小，单位word，也就是4字节 */
                 NULL,           		/* 任务参数  */
                 2,              		/* 任务优先�?1�?7?1�?1�?7?7 数�1�?7�?1�?7越小优先级越低，这个跟uCOS相反 */
                 &xHandleTaskStart );   /* 任务句柄  */
   #else 
	/*------------------ 静态任务创建 ------------------*/
	
	xHandleTaskMsgPro = xTaskCreateStatic(
			vTaskMsgPro,			/* 任务函数 */
			"vTaskMsgPro",			/* 任务名 */
			256,					/* 栈大小（word） */
			NULL,					/* 参数 */
			2,						/* 优先级 */
			xTaskMsgProStack,		/* 栈数组 */
			&xTaskMsgProTCB 		/* TCB */
	);
	
	xHandleTaskStart = xTaskCreateStatic(
			vTaskStart, 			/* 任务函数 */
			"vTaskStart",			/* 任务名 */
			128,					/* 栈大小（word） */
			NULL,					/* 参数 */
			1,						/* 优先级 */
			xTaskStartStack,		/* 栈数组 */
			&xTaskStartTCB			/* TCB */
	);
 

   #endif 
}

/**************************************************************************
	**
	*Function Name:void freertos_set_prority(void)
	*Function :
	*Input Ref: 
	*Return Ref:NO
	*
****************************************************************************/
static void power_handler(void)
{
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
				vTaskDelay(pdMS_TO_TICKS(100));
		   	}
		  break;

		  

          case power_off:
              gpro_t.process_run_step=0;
	          gpro_t.soft_version =0; //WT.EDIT 2025.10.31
		     power_off_handler();
             break;
          }

}

/**************************************************************************
 **
 *Function Name:void freertos_set_prority(void)
 *Function :
 *Input Ref: 
 *Return Ref:NO
 *
****************************************************************************/
static void wifi_handler(void)
{
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

}


/**************************************************************************
	**
	*Function Name:void freertos_set_prority(void)
	*Function :
	*Input Ref: 
	*Return Ref:NO
	*
****************************************************************************/
void freertos_set_prority(void)
{
	
	

   
} 
/**
 * @brief  : 
 * @note    
 * @param   None
 * @retval  None
 */

void freertos_decoder_isr_handler(void)
{

    #if 0
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	xTaskNotifyFromISR(xHandleTaskMsgPro,  /* ???? */
						DECODER_BIT_0,	   /* ???????????bit0  */
						eSetBits,  /* ????????????BIT_0?????, ??????????? */
						&xHigherPriorityTaskWoken);

	/* ??xHigherPriorityTaskWoken = pdTRUE,???????????????????? */
	portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	#else

	/* 发送任务通知给处理任务 */
        BaseType_t xHigherPriorityTaskWoken = pdFALSE;
        // 假设我们要通知 vTaskStart 任务
        vTaskNotifyGiveFromISR(xHandleTaskMsgPro, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);


	#endif 

}
