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

#define UART1_RING_SIZE 20

uint16_t dma_len;




typedef void (*Usart1RxCallback)(uint8_t data);

static Usart1RxCallback usart1_rx_cb = NULL;  //定义一个全局静态函数指针

//static void usart1_isr_callback_handler(uint8_t data);

uint8_t rx_inputBuf[12];


typedef struct
{
    uint8_t *buffer;      // 缓冲区指针
    uint16_t size;        // 缓冲区大小
    volatile uint16_t head; // 写指针
    volatile uint16_t tail; // 读指针
} ring_buffer_t;

uint8_t uart1_ring_buffer[UART1_RING_SIZE]; 
ring_buffer_t uart1_rx_ring;


//初始化函数
void ring_buffer_init(ring_buffer_t *rb, uint8_t *buf, uint16_t size)
{
    rb->buffer = buf;
    rb->size = size;
    rb->head = 0;
    rb->tail = 0;
}

void ring_buffer_write(ring_buffer_t *rb, uint8_t *data, uint16_t len)
{
    for(uint16_t i = 0; i < len; i++)
    {
        uint16_t next = (rb->head + 1) % rb->size;

        // 缓冲区满了（丢弃最旧数据）
        if(next == rb->tail)
        {
            rb->tail = (rb->tail + 1) % rb->size;
        }

        rb->buffer[rb->head] = data[i];
        rb->head = next;
    }
}


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
void usart1_register_rx_callback(Usart1RxCallback cb)
{
   usart1_rx_cb = cb;

}
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
void usart1_invoke_callback(uint8_t data)
{
   if(usart1_rx_cb !=NULL){

       usart1_rx_cb(data);
   }


}


void callback_register_usart1_rx(void)
{

   //usart1_register_rx_callback(usart1_isr_callback_handler);

}



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

uint8_t inputBuf[12];
uint8_t wifi_rx_inputBuf[WIFI_RX_NUMBERS];

uint8_t rx_numbers;

//static void receive_cmd_or_notice_handler(void);

//static void parse_recieve_copy_data_handler(void);


volatile uint8_t rx_data_counter=0;

#if 0
/********************************************************************************
	**
	*Function Name:void usart1_isr_callback_handler(void)
	*Function :  this is receive data from mainboard.
	*Input Ref:NO
	*Return Ref:NO
	*
*******************************************************************************/
static void usart1_isr_callback_handler(uint8_t data)
{
       
	#if 0    
       switch(rx_state){
	
         case 0:
	      if(data == FRAME_HEADER){
	   	   gl_tMsg.usData[rx_data_counter]=data;
		    rx_data_counter++;
		    rx_state =1;

	      }
		  break;

		  case 1:
		    if(data == FRAME_NUM || data == FRAME_ACK_NUM || data==FRAME_OLD_NUM){
	   	       gl_tMsg.usData[rx_data_counter]=data;
		       
			   if(gl_tMsg.usData[rx_data_counter]==0x80){ //new version is copy command or notice 0x80
			   	 rx_data_counter++;
			   	 gl_tMsg.copy_cmd_flag=0x80;
			   	}
			   else{
			   	 rx_data_counter++;
			   	 gl_tMsg.copy_cmd_flag=0;
		         rx_state =2;
			   	}

	         }
			 else{
			    rx_state =0;
			    rx_data_counter=0;
			    gl_tMsg.usData[0]=0;
			    gl_tMsg.usData[1]=0;
            }

		  break;

		  case 2: //rx command or notice or oxFF --> copy command or notice .
		      gl_tMsg.usData[rx_data_counter]=data;
			
		      if(gl_tMsg.usData[rx_data_counter]==0xFF){ //older version is copy command or notice "0xFF"
			  	gl_tMsg.copy_cmd_flag=0xFF;
			    rx_data_counter++;

		      }
			  else{
			  	gl_tMsg.copy_cmd_flag=0;
			  	gl_tMsg.cmd_notice= gl_tMsg.usData[rx_data_counter];
			    rx_data_counter++;
			  }
			  rx_state =3;


		  break;

		  case 3: //rx excuite command and notice or data 
		     
		      gl_tMsg.usData[rx_data_counter]=data;
			
		      if(gl_tMsg.usData[rx_data_counter]==0x0F){ //0x0F -> is receive data .
				 rx_data_counter++;
			
			  	rx_state =7;

			  }
			  else if(gl_tMsg.copy_cmd_flag==0xFF){
			      gl_tMsg.cmd_notice= gl_tMsg.usData[rx_data_counter];
				  rx_data_counter++;
			      rx_state =4;


			  }
			  else{
			  	 gl_tMsg.execuite_cmd_notice=gl_tMsg.usData[rx_data_counter];
				  rx_data_counter++;
				  rx_state =4;
				 

			  	}


		  break;

		   case 4: //rx is cmmand and notice (new version is frame end "0xFE")
			  gl_tMsg.usData[rx_data_counter]=data;
			 
			  if(gl_tMsg.usData[rx_data_counter]==0){//older version is frame command "0x00"
			         rx_data_counter++;
					 gl_tMsg.rx_data_flag = 0;
					 rx_state =5; //older version 

              }
			  else if(gl_tMsg.copy_cmd_flag==0xFF){ //copy command or notice execuite 
                  gl_tMsg.execuite_cmd_notice=gl_tMsg.usData[rx_data_counter];
				  rx_data_counter++;
			      rx_state =5; //older version 


			  }
			  else if(gl_tMsg.usData[rx_data_counter]==0xFE){ //new verson protocol is frame end "0xFE"

			          rx_data_counter++;
					  gl_tMsg.rx_data_flag = 0;
					  rx_state =6; //new version 


			  }
			  else{
			  	rx_state =0;
			    rx_data_counter=0;
                 gl_tMsg.usData[0]=0;
				 gl_tMsg.usData[1]=0;
				 gl_tMsg.usData[2]=0;
				 gl_tMsg.usData[3]=0;
				 gl_tMsg.usData[4]=0;
			  }

		  break;


		  
		case 5: //old version is frame end "0xFE"
			  gl_tMsg.usData[rx_data_counter]=data;
			 
			  if(gl_tMsg.usData[rx_data_counter]==0xFE){//new version id frame end 
			         rx_data_counter++;
					 rx_state =6; //new version 

              }
			  else{
			  	rx_state =0;
			    rx_data_counter=0;
			     gl_tMsg.usData[0]=0;
				 gl_tMsg.usData[1]=0;
				 gl_tMsg.usData[2]=0;
				 gl_tMsg.usData[3]=0;
				 gl_tMsg.usData[4]=0;
			  
			  }

		  break;
	

		  case 6: //BCC CHECK CODE ,receive success 
			  
		  	 gl_tMsg.usData[rx_data_counter]=data;
			 gl_tMsg.bcc_check_code=gl_tMsg.usData[rx_data_counter];
		     gl_tMsg.total_data_length = rx_data_counter+1;
			// memcpy(gl_tMsg.desData,gl_tMsg.usData,(gl_tMsg.total_data_length+1));
          
			
			
			//gl_tMsg.check_code_hex = bcc_check(gl_tMsg.usData, (gl_tMsg.total_data_length-1));
	        //if(gl_tMsg.check_code_hex == gl_tMsg.bcc_check_code){
			 rx_data_counter=0;
		     rx_state = 0;
			gl_tMsg.usData[0]=0;
			gl_tMsg.usData[1]=0;
	         gl_tMsg.usData[6]=0;
			 gpro_t.decoder_success_flag=1;
		     //continue; // 使用 continue 立即跳过下面所有代码，回到 while(1) 顶部
			 //freertos_decoder_isr_handler();
			



		  break;

		  case 7://calculate receive data length.

		      gl_tMsg.usData[rx_data_counter]=data;
			  gl_tMsg.data_length = gl_tMsg.usData[rx_data_counter];
			  rx_data_counter++;
			
		      if(gl_tMsg.data_length > 0){ //0x0F -> is receive data .
		         gl_tMsg.rc_data_length =0;
			  	rx_state =8;

			  }
			  else{
			  	
				 rx_data_counter =0;
		         rx_state =0;
				 gl_tMsg.usData[0]=0;
				 gl_tMsg.usData[1]=0;
				 gl_tMsg.usData[2]=0;
				 gl_tMsg.usData[3]=0;
				 gl_tMsg.usData[4]=0;

			  	}



		  break;


		  

		  case 8: //receive is data of length.
		  	 
			 gl_tMsg.usData[rx_data_counter]=data;
			
			 gl_tMsg.rx_data[gl_tMsg.rc_data_length++]= gl_tMsg.usData[rx_data_counter];
		      rx_data_counter++;
			 
			 if(gl_tMsg.rc_data_length >=gl_tMsg.data_length){
                
			      rx_state =5;
             }
			 else rx_state = 8;
		  	
		  break;

		}
	   #else 
	   switch(rx_state){
	  
		   case 0:
			if(data == FRAME_HEADER){
			 gl_tMsg.usData[rx_data_counter]=data;
			  rx_data_counter++;
			  rx_state =1;
	
			}
			break;
	
			case 1:
			  if(data == FRAME_NUM || data == FRAME_ACK_NUM || data==FRAME_OLD_NUM){
				 gl_tMsg.usData[rx_data_counter]=data;
				  rx_data_counter++;
				
				  rx_state =2;
			  }
			  else{
				  rx_state =0;
				  rx_data_counter=0;
		 
			  }
	
			break;
	
			case 2: //rx command or notice or oxFF --> copy command or notice .
				gl_tMsg.usData[rx_data_counter]=data;
			    rx_data_counter++;
//			    if(data == 0xA5){
//				   rx_state =0;

//				   rx_data_counter=0;
//				   gl_tMsg.usData[0]=0;
//				   gl_tMsg.usData[1]=0;


//				}
//				else 

				if(data==0xFE && rx_data_counter>2){ //older version is copy command or notice "0xFF"
				   
				    rx_state =3;
	
	              
				}
				
				
	
			break;
	
			case 3: //rx excuite command and notice or data 
			   
				gl_tMsg.usData[rx_data_counter]=data;
				rx_data_counter++;

			    rx_numbers =rx_data_counter;
			
				 
			
				gpro_t.decoder_success_flag=1;
				rx_data_counter=0;
			 
				
	           rx_state =0;
				  
	
	
			break;
	
			
	
	
		  }


	   #endif 
}
#endif 
/*****************************************************************************
	**
	*Function Name:void usart1_protocol_state_machine(void)
	*Function :  in process bsp_freertos.c xTaskMsgPro
	*Input Ref:NO
	*Return Ref:NO
	*
*******************************************************************************/
uint8_t parse_exit_flag,parse_decoder_flag;

void usart1_protocol_state_machine(void)
{
  


   uint8_t i;
   //memcpy(rx_inputBuf,gl_tMsg.usData,rx_numbers);
   memcpy(rx_inputBuf,uart1_rx_buf, dma_len);
   parse_decoder_flag=1;

   while(parse_decoder_flag==1){
	
        if(rx_inputBuf[2]==0xFF){ //copy command 

		     gl_tMsg.copy_cmd_flag = 0xFF;
			  
		     gl_tMsg.cmd_notice = rx_inputBuf[3];
		
		
		     gl_tMsg.execuite_cmd_notice = rx_inputBuf[4];
			
		  
			 parse_exit_flag =1;

			 rx_data_counter=0;
			
			 
		 }
		 else{
		 	gl_tMsg.copy_cmd_flag = 0;
			gl_tMsg.cmd_notice = rx_inputBuf[2];
            //gl_tMsg.usData[rx_data_counter] = inputBuf[3];
          
           if(inputBuf[3]==0x0F){ //is data frame ,don't is command 

               gl_tMsg.data_length =rx_inputBuf[4]; //receive data of length
               gl_tMsg.execuite_cmd_notice=0;
               for(i=0;i<gl_tMsg.data_length;i++){
		          rx_data_counter++;
               
			      gl_tMsg.rx_data[i] = rx_inputBuf[4+rx_data_counter];
         
                 
               }
			   rx_data_counter=0;
   
			    parse_exit_flag=1;
			
		 
           }
		   else if(inputBuf[3]!=0x0F){
                gl_tMsg.execuite_cmd_notice =  rx_inputBuf[3];
				 rx_data_counter=0;
				
                parse_exit_flag=1;
		
		  
			 

            }
		  

		 }

   if(parse_exit_flag==1){
   	
   
   if(gl_tMsg.copy_cmd_flag == 0){
 
	 // receive_cmd_or_notice_handler();
	   

   }
   else{

       // parse_recieve_copy_data_handler();
	
   

   }
     parse_exit_flag++;
	parse_decoder_flag=0;

   }
   }

}




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
  
		  // 写入环形缓冲区
		  ring_buffer_write(&uart1_rx_ring, uart1_rx_buf, dma_len);
  
		  // 重启 DMA
		  LL_DMA_DisableChannel(DMA1, LL_DMA_CHANNEL_2);
		  LL_DMA_SetDataLength(DMA1, LL_DMA_CHANNEL_2, UART1_RX_BUF_SIZE);
		  LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_2);
	  }
  
   #endif 
  /* USER CODE END USART1_IRQn 0 */

  
  /* USER CODE BEGIN USART1_IRQn 1 */
	 // 清除错误标志
    if (LL_USART_IsActiveFlag_ORE(USART1)) LL_USART_ClearFlag_ORE(USART1);
    if (LL_USART_IsActiveFlag_FE(USART1))  LL_USART_ClearFlag_FE(USART1);
    if (LL_USART_IsActiveFlag_NE(USART1))  LL_USART_ClearFlag_NE(USART1);
  /* USER CODE END USART1_IRQn 1 */
}

/**
  * @brief This function handles USART1 global interrupt 
  * @param
  * @retrval 
**/
void decoder_handler(void)
{

    static uint8_t  decoder_copy ;
	while(ring_buffer_has_data(&uart1_rx_ring))
	{
	
	       memcpy(rx_inputBuf,uart1_rx_buf,dma_len);
	       //uint8_t ch = ring_buffer_read_byte(&uart1_rx_ring); 
	       decoder_copy =1;
		   //S03_Protocol_ByteHandler(rx_inputBuf); // 每个字节丢进状态机
           memset(&uart1_rx_ring,0,12);
		   dma_len = 0;
	       //usart1_protocol_state_machine();

	}

	while(decoder_copy ==1){

	S03_Protocol_ByteHandler(rx_inputBuf); // 每个字节丢进状态机

	decoder_copy++;

	}
		
				


}

