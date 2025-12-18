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

#define UART1_RX_BUF_SIZE 20

typedef void (*Usart1RxCallback)(uint8_t data);

static Usart1RxCallback usart1_rx_cb = NULL;  //定义一个全局静态函数指针

static void usart1_isr_callback_handler(uint8_t data);

uint8_t rx_inputBuf[12];

//提供注册接口
void usart1_register_rx_callback(Usart1RxCallback cb)
{
   usart1_rx_cb = cb;

}

void usart1_invoke_callback(uint8_t data)
{
   if(usart1_rx_cb !=NULL){

       usart1_rx_cb(data);
   }


}


void callback_register_usart1_rx(void)
{

   usart1_register_rx_callback(usart1_isr_callback_handler);

}



volatile uint8_t uart1_rx_buf[UART1_RX_BUF_SIZE];
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

typedef enum{

    power_on_off=1,
    ptc_on_off=2,
    plasma_on_off=3,
    ultrasonic_on_off=4,
    wifi_link=5,
    buzzer_sound_s=6,
    ai_mode=7,
    temp_high_warning=8,
    fan_warning_s=9,
    fan_on_off = 0x0B,

     //notice no sound 
    ack_power_on_off = 0x10,
    ack_ptc_on_off = 0x12,
    ack_plasma_on_ff= 0x13,
    ack_ultrasonic_on_off = 0x14,
}signal_parase_t;


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

uint8_t inputBuf[1];
uint8_t wifi_rx_inputBuf[WIFI_RX_NUMBERS];

uint8_t rx_numbers;

static void receive_cmd_or_notice_handler(void);

static void parse_recieve_copy_data_handler(void);


volatile uint8_t rx_data_counter=0;


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
				  gl_tMsg.usData[0]=0;
				  gl_tMsg.usData[1]=0;
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
				gl_tMsg.usData[0]=0;
				gl_tMsg.usData[1]=0;
				
	           rx_state =0;
				  
	
	
			break;
	
			
	
	
		  }


	   #endif 
}
/********************************************************************************
	**
	*Function Name:void usart1_protocol_state_machine(void)
	*Function :  in process bsp_freertos.c xTaskMsgPro
	*Input Ref:NO
	*Return Ref:NO
	*
*******************************************************************************/
void usart1_protocol_state_machine(void)
{
  
   static uint8_t  parse_decoder_flag, parse_exit_flag;

   uint8_t i;
   memcpy(rx_inputBuf,gl_tMsg.usData,rx_numbers);

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
 
	  receive_cmd_or_notice_handler();
	   

   }
   else{

        parse_recieve_copy_data_handler();
	
    }

	parse_decoder_flag=0;

   }

   }

}

/**********************************************************************
    *
    *Function Name:void receive_data_from_display(uint8_t *pdata,uint8_t len)
    *Function: receive usart touchkey of command 
    *Input Ref:NO
    *Return Ref:NO
    *
**********************************************************************/
static void receive_cmd_or_notice_handler(void)
{

   static uint8_t wifi_link_counter;
   	switch(gl_tMsg.cmd_notice){

   

     case 0:


     break;

     case power_on_off: 

         
        if(gl_tMsg.execuite_cmd_notice  == 0x01){ //open

		  
			if(gpro_t.power_onoff_cp_counter == 0){ //recoder power on times .
				gpro_t.power_onoff_cp_counter ++ ;
	            buzzer_sound();//buzzer_sound_fun();
	            SendWifiData_Answer_Cmd(0x01,0x01);
	            vTaskDelay(pdMS_TO_TICKS(10));
	            wifi_link_counter=0;
	            gpro_t.process_run_step=0;
	           	gpro_t.gpower_on = power_on;

			}
			else if(gpro_t.power_onoff_cp_counter==1){
				gpro_t.gpower_on = power_on;
	            SendWifiData_Answer_Cmd(0x01,0x01);
		        vTaskDelay(pdMS_TO_TICKS(10));

			}
  

        }
        else if(gl_tMsg.execuite_cmd_notice  == 0x0){ //close 

		     if(gpro_t.power_onoff_cp_counter > 0){
			 	
			  gpro_t.power_onoff_cp_counter=0;

              buzzer_sound();

              SendWifiData_Answer_Cmd(0x01,0x02); //power off .

              vTaskDelay(pdMS_TO_TICKS(10)); 
              wifi_link_counter=0;
             
             
              
             gpro_t.power_off_run_step=1;
             gpro_t.gpower_on = power_off;
			 
		     }
			 else if(gpro_t.power_onoff_cp_counter==0){
			      SendWifiData_Answer_Cmd(0x01,0x02); //power off .
			      vTaskDelay(pdMS_TO_TICKS(10)); 
			    
                   gpro_t.gpower_on = power_off;


			 }
        }

     break;

     case ptc_on_off: //PTC key of command .

     if(gl_tMsg.execuite_cmd_notice  == 0x01 && gpro_t.gpower_on == power_on){//phone_cmd_power

	     if(gpro_t.ptc_onoff_cp_counter==0){
		  	gpro_t.ptc_onoff_cp_counter++;
          buzzer_sound();
          SendWifiData_Answer_Cmd(0x02,0x01); //
          vTaskDelay(pdMS_TO_TICKS(10)); 
		
          gctl_t.gDry = 1;
		  ptc_recoder_flag = 1;
	      gpro_t.ptc_switch_flag++;
		  gctl_t.app_timer_power_on_flag=0;

        
         if(gpro_t.stopTwoHours_flag==0){
         if(gpro_t.ptc_warning ==0 && gpro_t.fan_warning_flag ==0){ //PTC warning flag
              PTC_SetHigh();
              
		      gctl_t.gTimer_senddata_panel=7;//at once run ptc function.
          }
      
         }
	      }
       }
       else if(gl_tMsg.execuite_cmd_notice  == 0x0 && gpro_t.gpower_on == power_on){
	   	if(gpro_t.ptc_onoff_cp_counter==1){
			gpro_t.ptc_onoff_cp_counter=0;
          buzzer_sound();
          gctl_t.gDry =0;
		  ptc_recoder_flag = 0;
          SendWifiData_Answer_Cmd(0x02,0x0); //
          vTaskDelay(pdMS_TO_TICKS(10)); 
         
         PTC_SetLow();
         gpro_t.ptc_switch_flag ++;
         gctl_t.gTimer_senddata_panel=7; //at once run ptc function.
		 gctl_t.app_timer_power_on_flag=0;

       }
       }

     break;

     case plasma_on_off: //PLASMA ACTIVE OPEN OR CLOSE

        if(gl_tMsg.execuite_cmd_notice == 0x01){
           
          buzzer_sound();
          SendWifiData_Answer_Cmd(0x03,0x01); //
          vTaskDelay(pdMS_TO_TICKS(10)); 
           
           gctl_t.gPlasma = 1;
           gpro_t.plasma_switch_flag++;
          if(gpro_t.stopTwoHours_flag==0){
            PLASMA_SetHigh();
          }
        }
        else if(gl_tMsg.execuite_cmd_notice  == 0x0){
           buzzer_sound();
          SendWifiData_Answer_Cmd(0x03,0x0); //
          vTaskDelay(pdMS_TO_TICKS(10)); 
           
           gctl_t.gPlasma = 0;
           gpro_t.plasma_switch_flag++;
        
          PLASMA_SetLow();

        }


     break;


      case 0x04: //ultrasonic  ACTIVE OPEN OR CLOSE

        if(gl_tMsg.execuite_cmd_notice  == 0x01){  //open 
          
          gctl_t.gUlransonic =1;
          gpro_t.ultrasonic_switch_flag++;
          SendWifiData_Answer_Cmd(0x04,0x01); //
          vTaskDelay(pdMS_TO_TICKS(10)); 

        }
        else if(gl_tMsg.execuite_cmd_notice  == 0x0){ //close 

          gctl_t.gUlransonic = 0;
          gpro_t.ultrasonic_switch_flag++;
          SendWifiData_Answer_Cmd(0x04,0x0); //
          vTaskDelay(pdMS_TO_TICKS(10)); 

        }


     break;

      case  wifi_link: // link wifi command

       if(gl_tMsg.execuite_cmd_notice == 0x01){  // link wifi 
        
          if(wifi_link_counter==0){
		  	wifi_link_counter++;
          gpro_t.link_net_step =0;
	      net_t.wifi_link_net_success=0;
          gpro_t.wifi_led_fast_blink_flag =1;
          gctl_t.wifi_config_net_lable=wifi_set_restor;
		  wifi_t.runCommand_order_lable= wifi_link_tencent_cloud;//2 
		  
          gctl_t.gTimer_linkTencentCounter=0; //total times is 120s
          SendWifiData_Answer_Cmd(0x05,0x01); //WT.EDIT 2024.12.28
          vTaskDelay(pdMS_TO_TICKS(10));

          }

        }
        else if(wifi_link_counter == 0x01){ //don't link wifi 
		      SendWifiData_Answer_Cmd(0x05,0x01); //WT.EDIT 2024.12.28
			  vTaskDelay(pdMS_TO_TICKS(10));

        }


     break;

     case buzzer_sound_s: //buzzer sound command 

          buzzer_sound();
		  //vTaskDelay(pdMS_TO_TICKS(5));
     break;

	 case 0x07: //AI command
	  if(gl_tMsg.execuite_cmd_notice == 0x02){
	       buzzer_sound();
		
          gctl_t.gModel=2;
          gctl_t.mode_ai_switch_flag =1;
          SendWifiData_Answer_Cmd(0x07,0x02); //
          vTaskDelay(pdMS_TO_TICKS(5)); 
        
          
       }
       else if(gl_tMsg.execuite_cmd_notice == 0x01){ //AI mode 
       
	
         buzzer_sound();
         gctl_t.gModel=1;
	     gctl_t.mode_ai_switch_flag =1;
         SendWifiData_Answer_Cmd(0x07,0x01); //
         vTaskDelay(pdMS_TO_TICKS(5)); 
		 
       }


	 break;

     case 0x16 : //buzzer sound command with answer .

        buzzer_sound();
        

		  gpro_t.answer_buzzer_flag = 1;//WT.EDIT 2025.07.28 


          SendWifiData_Answer_Cmd(0x16,0x01); //WT.EDIT 2025.07.28

		  vTaskDelay(pdMS_TO_TICKS(10));
		  
       break;


	  case 0x1A: //receive from display board set temperature value .

	   if(gpro_t.soft_version ==1){
	      // gctl_t.set_temperature_flag = 1; 
	     
	       gctl_t.set_temperature_value = gl_tMsg.rx_data[0]  ;
		   gctl_t.ptc_on_off_flag =0;
		   gctl_t.set_temp_first_closeptc=0;
		   gctl_t.rx_set_temp_flag =0;
		   
	
		   
	       if(wifi_link_net_state()==1){
	            MqttData_Publis_SetTemp(gctl_t.set_temperature_value);
			         // vTaskDelay(pdMS_TO_TICKS(200));//osDelay(200);//HAL_Delay(350);
	        }
	   }
	   else{
	      gctl_t.set_temperature_value = gl_tMsg.rx_data[0]  ;

	      if(wifi_link_net_state()==1){
	            MqttData_Publis_SetTemp(gctl_t.set_temperature_value);
			         // vTaskDelay(pdMS_TO_TICKS(200));//osDelay(200);//HAL_Delay(350);
	        }


	   }

        
      break;

	  
	 case 0x1C: //display board to send time of value for two hours stop have a rest.
				//no sound is notice
			   if(gl_tMsg.rx_data[0]== 0x78 &&  gpro_t.soft_version ==1){ //2 hours 
	 
                  gpro_t.stopTwoHours_flag = 1;
				  SendWifiData_Answer_Cmd(0x1C,0x01); //WT.EDIT 2025.07.28
                  vTaskDelay(pdMS_TO_TICKS(10));
				  
				  #if DEBUG_FLAG

				  printf("rx_stopTwo_Hours_flag = 1 !!!!\r\n");

				  #endif 

                 
			 
			   }
			   else if(gl_tMsg.rx_data[0]== 0x0A && gpro_t.soft_version ==1){ //10mintues
                 gpro_t.stopTwoHours_flag =0;
				
			     SendWifiData_Answer_Cmd(0x1C,0x0); //WT.EDIT 2025.07.28
                 vTaskDelay(pdMS_TO_TICKS(10));
			      #if DEBUG_FLAG

				  printf("rx_stopTwo_Hours_flag = 0 @@@@@\r\n");

				  #endif 
				 ActionEvent_Handler();
			     
				
				 
			  }
	 
	 break;

     

     case 0x22: //PTC notice don't buzzer sound
        if(gl_tMsg.execuite_cmd_notice== 0x01){
        gctl_t.gDry = 1;
		ptc_recoder_flag =1 ;//WT.EDIT 2025.11.17
		gpro_t.ptc_switch_flag ++;
//		if(gpro_t.soft_version ==1){
//		 SendWifiData_Answer_Cmd(0x22,0x01); //WT.EDIT 2025.07.28
//         vTaskDelay(pdMS_TO_TICKS(5));
//		}
   
        if(gpro_t.stopTwoHours_flag ==0){
              PTC_SetHigh();
        }
		if(wifi_link_net_state()==1){ 
			MqttData_Publish_SetPtc(0x01);
			
		}
			#if DEBUG_FLAG

			//  printf("disp_gDry = %d\r\n",gctl_t.gDry);

			#endif 
          
      }
      else if(gl_tMsg.execuite_cmd_notice== 0x0){
        
          gctl_t.gDry =0;
		  ptc_recoder_flag =0 ;
          PTC_SetLow();
          gpro_t.ptc_switch_flag++;
		  
//		  if(gpro_t.soft_version==1){
//		   SendWifiData_Answer_Cmd(0x22,0x0); //WT.EDIT 2025.07.28
//            vTaskDelay(pdMS_TO_TICKS(5));

//		  }
		  if(wifi_link_net_state()==1){ 
			MqttData_Publish_SetPtc(0x0);
			
		  }
         
		  	
			#if DEBUG_FLAG

			//  printf("disp_gDry = %d\r\n",gctl_t.gDry);

			#endif 
			 
      }
   
     break;

     case 0x27: //AI command without buzzer sound

	    if(gl_tMsg.execuite_cmd_notice == 0x02){
		 
	          gctl_t.gModel=2;
			  gctl_t.mode_ai_switch_flag =1;
		}
	    else if(gl_tMsg.execuite_cmd_notice == 0x01){ //AI mode 
	        gctl_t.gModel=1;
		    gctl_t.mode_ai_switch_flag =1; 
		}
    break;

     case 0xF0: //software version difference older and new sotfware 
      
            gpro_t.soft_version = gl_tMsg.rx_data[0];
	 
				
			#if DEBUG_FLAG

			 // printf("gpro_t.soft_version = %d\r\n",gpro_t.soft_version);

			#endif 
			 
		

	 break;

	
    }

	
}
/**********************************************************************
	*
	*Function Name:static void parse_recieve_copy_data_handler(void)
	*Function: display board send to mainboard answer signal
	*Input Ref:NO
	*Return Ref:NO
	*
**********************************************************************/
static void parse_recieve_copy_data_handler(void)
{

    
     switch(gl_tMsg.cmd_notice){
    
        case ack_null:
    
    
        break;
    
        case 0x10: //power on or off notice .--older version 
            
          if(gl_tMsg.execuite_cmd_notice == 0x01){
		  	 if(gpro_t.gpower_on == power_on){
               gpro_t.copy_cmd_notice_buff[1] =COPY_OK;
		  	 }
			 else{ 
             	gpro_t.copy_cmd_notice_buff[1] =COPY_NG;
				gpro_t.gTimer_timer_start_counter = 0;//start_timer(0);

			 }
            
          }
          else if(gl_tMsg.execuite_cmd_notice == 0){
             if(gpro_t.gpower_on == power_off)
                gpro_t.copy_cmd_notice_buff[1] =COPY_OK;
			 else 
             	 gpro_t.copy_cmd_notice_buff[1] =COPY_NG;
                 gpro_t.gTimer_timer_start_counter = 0;//start_timer(0);
          }
                    
         
        break;
    
        case 0x012 ://ptc open or close
    
          
          if(gl_tMsg.execuite_cmd_notice == 0x01){

		     if(ptc_recoder_flag ==1)//if(gctl_t.gDry ==1)
               gpro_t.copy_cmd_notice_buff[2] =COPY_OK;
			else if(ptc_recoder_flag ==0){
			  gpro_t.copy_cmd_notice_buff[2] =COPY_NG;
			  gpro_t.gTimer_timer_start_counter = 0;
			}
            
          }
          else if(gl_tMsg.execuite_cmd_notice == 0){
            if(ptc_recoder_flag ==0){//if(gctl_t.gDry ==0){
                gpro_t.copy_cmd_notice_buff[2] =COPY_OK;
            }
			else if(ptc_recoder_flag == 1){
			  gpro_t.copy_cmd_notice_buff[2] =COPY_NG;
			  gpro_t.gTimer_timer_start_counter = 0;
			 }
          }
    
    
        break;

		case 0x31: //phone power on 
		  if(gl_tMsg.execuite_cmd_notice == 0x01){
           
             //gctl_t.ptc_warning =0;
			
	        ///gpro_t.fan_warning_flag =0;
	        //gpro_t.power_off_run_step=1;
	        //powerOffFanRun_flag = 1;
	
			//gpro_t.gpower_on = power_on;//gctl_t.rx_command_tag= POWER_ON;
			gpro_t.phone_power_on_flag = 0; //ack_app_power_on;
	     

		   }

		break;

		case 0x30: //phone power off 

		  if(gl_tMsg.execuite_cmd_notice == 0x00){
			           //gpro_t.gpower_on = power_off;
					   //gpro_t.power_off_run_step=1; //WT.EDIT 2025.01.04
					   //powerOffFanRun_flag = 1;
					   gpro_t.phone_power_on_flag = 0; //ack_app_power_off;
					

               

		    }

		break;

        case ack_app_timer_power_on:

          

        break;
    
        case ack_wifi_on:
    
         
    
    
        break;
    
        case ack_ptc_on:
    
    
        break;
    
        case ack_ptc_off:
    
        break;
    
    
        }
 

}

/**
  * @brief This function handles USART1 global interrupt / USART1 wake-up interrupt through EXTI line 25.
  */
void USART1_IRQHandler(void)
{
  /* USER CODE BEGIN USART1_IRQn 0 */
  volatile uint8_t data;
  // static uint8_t rx_flag;
   if(LL_USART_IsActiveFlag_RXNE_RXFNE(USART1)){
   
      //LL_USART_ClearFlag_RXNE(USART1);
      data = LL_USART_ReceiveData8(USART1);
      //usart1_isr_callback_handler(data);
       //  usart1_invoke_callback(data);
       usart1_isr_callback_handler(data);

     

   }
  /* USER CODE END USART1_IRQn 0 */

  
  /* USER CODE BEGIN USART1_IRQn 1 */
	 // 清除错误标志
    if (LL_USART_IsActiveFlag_ORE(USART1)) LL_USART_ClearFlag_ORE(USART1);
    if (LL_USART_IsActiveFlag_FE(USART1))  LL_USART_ClearFlag_FE(USART1);
    if (LL_USART_IsActiveFlag_NE(USART1))  LL_USART_ClearFlag_NE(USART1);
  /* USER CODE END USART1_IRQn 1 */
}


void decoder_handler(void)
{
if(gpro_t.decoder_success_flag==1){
	gpro_t.decoder_success_flag++;
		
	usart1_protocol_state_machine();
	

		
}
}

