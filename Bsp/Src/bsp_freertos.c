#include "bsp.h"



/***********************************************************************************************************
											函数声明
***********************************************************************************************************/
//static void vTaskRunPro(void *pvParameters);
static void vTaskMsgPro(void *pvParameters);
static void vTaskStart(void *pvParameters);
static void AppTaskCreate (void);


/* 创建任务通信机制 */
//static void AppObjCreate(void);


/***********************************************************************************************************
											变量声明
***********************************************************************************************************/
//static TaskHandle_t xHandleTaskRunPro = NULL;
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
**********************************************************************************************************/
static void vTaskMsgPro(void *pvParameters)
{
  
	// BaseType_t xResult;
	// const TickType_t xMaxBlockTime = pdMS_TO_TICKS(1000); /* 1.测试设定�?-设置�?大等待时间为50ms */
    // uint32_t ulValue;
    // static uint8_t power_on_sound_flag ;
	
    while(1)
    {
    for (;;)
    {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);//PB4�����
        osDelay(250);//�ȴ�250ms
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
  
	// BaseType_t xResult;
	// const TickType_t xMaxBlockTime = pdMS_TO_TICKS(1000); /* 1.测试设定�?-设置�?大等待时间为50ms */
    // uint32_t ulValue;
    // static uint8_t power_on_sound_flag ;
	
    while(1)
    {
    for (;;)
        {
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);//PB4�����
        osDelay(100);//�ȴ�100ms

        }

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

