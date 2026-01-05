#include "bsp.h"

// 为协议中的魔术字节定义常量，提高可读性
#define FRAME_HEADER        0xA5        //receive display board header  
#define FRAME_NUM           0x02          //main deviece number is 0x10 
#define FRAME_OLD_NUM       0x01          //older version device NUM
#define FRAME_ACK_NUM       0x80          //new version from main answer singnal 0x80 new version . 
#define FRAME_END_BYTE              0xFE
#define DATA_FRAME_TYPE_INDICATOR   0x0F
#define FRAME_COPY_NUM              0xFF   //this is older version .

#define ACK_SUCCESS 0x00U
#define ACK_FAILURE 0x01U

#define UART1_RING_SIZE  20

uint16_t dma_len;


/* 假设定义 */
//#define UART1_RX_BUF_SIZE  256
uint8_t rx_buf[2][UART1_RX_BUF_SIZE]; // 真正的双缓冲区
uint8_t active_buf = 0;               // 当前使用的缓冲区索引


//typedef void (*Usart1RxCallback)(uint8_t data);

//static Usart1RxCallback usart1_rx_cb = NULL;  //定义一个全局静态函数指针

//static void usart1_isr_callback_handler(uint8_t data);

uint8_t rx_inputBuf[20];
uint8_t rx_frame_tc;


typedef struct
{
    uint8_t *buffer;      // 缓冲区指针
    uint8_t size;        // 缓冲区大小
    volatile uint8_t head; // 写指针
    volatile uint8_t tail; // 读指针
} ring_buffer_t;

uint8_t uart1_ring_buffer[UART1_RING_SIZE]; 
ring_buffer_t uart1_rx_ring;

uint8_t uart1_rx_buf[UART1_RX_BUF_SIZE];
volatile uint8_t uart1_rx_head = 0;
volatile uint8_t uart1_rx_tail = 0;
volatile uint8_t rx_state;



typedef enum ack_sig{

  ack_null,
  ack_power_on=1 ,
  ack_power_off=2,
  ack_wifi_on=3,
  ack_ptc_on=4,
  ack_ptc_off=5,
  ack_plasma_on=6,
  ack_plasma_off=7,
  ack_ultra_on=8,
  ack_ultra_off=9,
  //wifi cmd

  ack_app_power_on=10,
  ack_app_power_off=11,
  ack_app_timer_power_on=12,
  
  

}ack_e;

typedef enum{

  open =1,
  close =2,
  no_change =0  

}atcion_state_e;



typedef enum {
    UART_STATE_WAIT_HEADER = 0,
    UART_STATE_NUM=1,
    UART_STATE_CMD_NOTICE=2,
    UART_STATE_EXEC_CMD_OR_LEN=3,
    UART_STATE_FRAME_END=4,
    UART_STATE_BCC_CHECK,
    UART_STATE_OLDER_BCC_CHECK,
    UART_STATE_DATA_LEN,
    UART_STATE_DATA,
    UART_STATE_DATA_END,
    UART_STATE_DATA_BCC
} uart_parse_state_t;




typedef struct Msg
{
    
	uint8_t   cmd_notice;
	uint8_t   execuite_cmd_notice;
	uint8_t   copy_cmd_flag;
	uint8_t   rx_data_flag;	
    uint8_t   bcc_check_code;
	uint8_t   check_code_hex;
    uint8_t   receive_data_length;
    uint8_t   data_length;
	uint8_t   rc_data_length;
	uint8_t   total_data_length;
	uint8_t   rx_data[4];
	uint8_t   usData[12];
	uint8_t   desData[12];

}MSG_T;

MSG_T   gl_tMsg; 


uint8_t wifi_rx_inputBuf[WIFI_RX_NUMBERS];

uint8_t rx_numbers;

//static void receive_cmd_or_notice_handler(void);

//static void parse_recieve_copy_data_handler(void);




//初始化函数
void ring_buffer_init(ring_buffer_t *rb, uint8_t *buf, uint8_t size)
{
    rb->buffer = buf;
    rb->size = size;
    rb->head = 0;
    rb->tail = 0;
}
/**
	*@brief
	*@note
	*@param
	*@rtrval 
**/
void ring_buffer_write(ring_buffer_t *rb, uint8_t *data, uint8_t len)
{

    uint8_t i, next;
	for(i = 0; i < len; i++)
    {
        next = (rb->head + 1) % rb->size;

        // 缓冲区满了（丢弃最旧数据）
        if(next == rb->tail)
        {
            rb->tail = (rb->tail + 1) % rb->size;
        }

        rb->buffer[rb->head] = data[i];
        rb->head = next;
    }
}

/**
	*@brief  读取环形缓冲器
	*@note
	*@param
	*@rtrval 
**/
uint8_t ring_buffer_read_byte(ring_buffer_t *rb)
{
    if(rb->head == rb->tail)
        return 0; // 空

    uint8_t ch = rb->buffer[rb->tail];
    rb->tail = (rb->tail + 1) % rb->size;

    return ch;
}

/**
	*@brief //提供注册接口
	*@note
	*@param
**/
//void usart1_register_rx_callback(Usart1RxCallback cb)
//{
//   usart1_rx_cb = cb;

//}
/**
	*@brief 判断是否有数据
	*@note
	*@param
**/
uint8_t ring_buffer_has_data(ring_buffer_t *rb)
{
    return (rb->head != rb->tail);
}

/**
	*@brief  回调函数
	*@note
	*@param
**/
//void usart1_invoke_callback(uint8_t data)
//{
//   if(usart1_rx_cb !=NULL){

//       usart1_rx_cb(data);
//   }


//}


void callback_register_usart1_rx(void)
{

   //usart1_register_rx_callback(usart1_isr_callback_handler);

}
volatile uint8_t rx_data_counter=0;
/**
  * @brief This function handles USART1 global interrupt 
  * @param
  * @retrval 
**/
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */
  volatile uint8_t data;
  // static uint8_t rx_flag;

   #if 0
   if(LL_USART_IsActiveFlag_RXNE_RXFNE(USART1)){
   
      //LL_USART_ClearFlag_RXNE(USART1);
      data = LL_USART_ReceiveData8(USART1);
       usart1_isr_callback_handler(data);

     

   }
   #else 
  
     if(LL_USART_IsActiveFlag_IDLE(USART1))
	 {
		  LL_USART_ClearFlag_IDLE(USART1);
  
		  dma_len = UART1_RX_BUF_SIZE - LL_DMA_GetDataLength(DMA1, LL_DMA_CHANNEL_2);
          //dma_len = LL_DMA_GetDataLength(DMA1, LL_DMA_CHANNEL_2);
		  // 写入环形缓冲区
		 // ring_buffer_write(&uart1_rx_ring, uart1_rx_buf, dma_len);
		  memcpy(rx_inputBuf,uart1_rx_buf,20);
     
		  
		  // 重启 DMA
		 /// LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_2);
		  ///LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_2, UART1_RX_BUF_SIZE);
		  //LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_2);
		  rx_frame_tc = 1;
		  semaphore_isr();
	  }
  
   #endif 
  /* USER CODE END USART1_IRQn 0 */

  
  /* USER CODE BEGIN USART1_IRQn 1 */
	 // 清除错误标志
   // if (LL_USART_IsActiveFlag_ORE(USART1)) LL_USART_ClearFlag_ORE(USART1);
   // if (LL_USART_IsActiveFlag_FE(USART1))  LL_USART_ClearFlag_FE(USART1);
   // if (LL_USART_IsActiveFlag_NE(USART1))  LL_USART_ClearFlag_NE(USART1);
  /* USER CODE END USART1_IRQn 1 */
}

/**
  * @brief This function handles USART1 global interrupt 
  * @param
  * @retrval 
**/
void decoder_handler(void)
{

   // static uint8_t  decoder_copy ;
	//while(rx_frame_tc==1)//while(ring_buffer_has_data(&uart1_rx_ring))
	
	
         
	       disp_protocol_bytehandler(rx_inputBuf);
		  // S03_Protocol_ByteHandler(rx_inputBuf,0); // 每个字节丢进状态机

		   memset(rx_inputBuf,0,20);
		  

            // 重启 DMA
		  LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_2);
		  LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_2, UART1_RX_BUF_SIZE);
		  LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_2);
		   rx_frame_tc=0;
		    // 重启 DMA
		 
	     

	

   if (LL_USART_IsActiveFlag_ORE(USART1)) LL_USART_ClearFlag_ORE(USART1);
  // if (LL_USART_IsActiveFlag_FE(USART1))  LL_USART_ClearFlag_FE(USART1);
  // if (LL_USART_IsActiveFlag_NE(USART1))  LL_USART_ClearFlag_NE(USART1);

}

