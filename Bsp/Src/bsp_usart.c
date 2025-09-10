#include "bsp.h"

// 为协议中的魔术字节定义常量，提高可读性
#define FRAME_HEADER        0xA5        //receive display board header  
#define FRAME_NUM           0x02          //main deviece number is 0x10    
#define FRAME_ACK_NUM       0x80          //from main answer singnal 0x80 new version . 
#define FRAME_END_BYTE              0xFE
#define DATA_FRAME_TYPE_INDICATOR   0x0F
#define FRAME_COPY_NUM              0xFF   //this is older version .

#define ACK_SUCCESS 0x00U
#define ACK_FAILURE 0x01U

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
    uint8_t   tx_data_success;
	uint8_t   cmd_notice;
	uint8_t   copy_cmd_notice;
    uint8_t   execuite_cmd_notice;
	uint8_t   rx_data_flag;	
    uint8_t   bcc_check_code;
    uint8_t   receive_data_length;
    uint8_t   data_length;
	uint8_t   rc_data_length;
	uint8_t   rx_data[4];
	uint8_t   usData[12];

}MSG_T;

MSG_T   gl_tMsg; 

uint8_t inputBuf[1];
uint8_t wifi_rx_inputBuf[WIFI_RX_NUMBERS];



static void receive_cmd_or_data_handler(void);

static void receive_copy_cmd_or_data_handler(void);

volatile uint8_t rx_data_counter=0;


/********************************************************************************
	**
	*Function Name:void usart1_isr_callback_handler(void)
	*Function :  this is receive data from mainboard.
	*Input Ref:NO
	*Return Ref:NO
	*
*******************************************************************************/
#if 1
void usart1_isr_callback_handler(uint8_t data)
{
     static uint8_t state;
    // BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	 inputBuf[0] = data;
	 if(gl_tMsg.tx_data_success ==0){
     switch(state)
		{
		case UART_STATE_WAIT_HEADER:  //#0
			if(inputBuf[0] == FRAME_HEADER){  // 0x5A --main board singla
               rx_data_counter=0;
               gl_tMsg.usData[rx_data_counter] = inputBuf[0];
				state=UART_STATE_NUM; //=1
				gl_tMsg.copy_cmd_notice=0;
				 gl_tMsg.cmd_notice=0;

             }
            
		break;

        case UART_STATE_NUM:

             if(inputBuf[0] == FRAME_NUM ||inputBuf[0] == FRAME_ACK_NUM){  // 0x5A --main board singla or copy cmd
               rx_data_counter++;
               gl_tMsg.usData[rx_data_counter] = inputBuf[0];
			   if(inputBuf[0] == FRAME_ACK_NUM){
                  gl_tMsg.copy_cmd_notice  = 0x80; //new version protocol is copy cmd notice.
                   state=UART_STATE_CMD_NOTICE; //=1
			   }
			   else{ 
			   	  gl_tMsg.copy_cmd_notice = 0;
			      state=UART_STATE_CMD_NOTICE; //=1
			   	}

            }
            else{
                state=0;
                rx_data_counter=0;
            }

        break;

        case UART_STATE_CMD_NOTICE://2
               rx_data_counter++;
               gl_tMsg.usData[rx_data_counter] = inputBuf[0];
			   gl_tMsg.cmd_notice = inputBuf[0];
			   if(gl_tMsg.cmd_notice == 0xFF){//this is older version protocol 0x02 -> copy command.
				   gl_tMsg.copy_cmd_notice = 0xFF;
				   state=UART_STATE_EXEC_CMD_OR_LEN; //1
			   }
			   else{
                 state=UART_STATE_EXEC_CMD_OR_LEN; //1
			   }
    
        break;

        case UART_STATE_EXEC_CMD_OR_LEN:
            rx_data_counter++;
            gl_tMsg.usData[rx_data_counter] = inputBuf[0];
            if(gl_tMsg.usData[rx_data_counter] !=0x0F && gl_tMsg.copy_cmd_notice != 0xFF){
                gl_tMsg.execuite_cmd_notice =  gl_tMsg.usData[rx_data_counter];
                state = UART_STATE_FRAME_END;

            }
            else if(gl_tMsg.usData[rx_data_counter] ==0x0F){
               gl_tMsg.rx_data_flag =  0x0F;
               state = UART_STATE_DATA_LEN; //receive data.
           }
		   else if(gl_tMsg.copy_cmd_notice == 0xFF){ //this is older compatibility 
		        gl_tMsg.cmd_notice = gl_tMsg.usData[rx_data_counter];
                state = UART_STATE_DATA_LEN; //receive data.
            }


        break;


        case  UART_STATE_FRAME_END: //receive comd and notice frame  end
            rx_data_counter++;
            gl_tMsg.usData[rx_data_counter] = inputBuf[0];
            if(inputBuf[0] == 0xFE){  // frame is tail of end "0xFE"
             
			   state=UART_STATE_BCC_CHECK; //=1

             }
		     else if(inputBuf[0] == 0x0){ //this is older version cmd[3]= 0 -> is cmd or notice don't "data"
		     
			    state=UART_STATE_BCC_CHECK; //=1

		     }
			 else{
                state=0;
                rx_data_counter=0;
			 }
         

        break;


        case UART_STATE_BCC_CHECK: //frem
            rx_data_counter++;
            gl_tMsg.usData[rx_data_counter] = inputBuf[0];
		    if(gl_tMsg.usData[rx_data_counter]==0xFE){
                  state = UART_STATE_OLDER_BCC_CHECK;

			}
			else{
                gl_tMsg.bcc_check_code =  gl_tMsg.usData[rx_data_counter];
				gl_tMsg.data_length = rx_data_counter;
	            if(gl_tMsg.bcc_check_code == bcc_check(gl_tMsg.usData, gl_tMsg.data_length))
	            {
	                state=0;
	                rx_data_counter=0; 
					 gl_tMsg.tx_data_success = 1;
	                freertos_decoder_isr_handler();

	            }
	            else{
	                state=0;
	                rx_data_counter=0;
	            }
			}

        break;

		case UART_STATE_OLDER_BCC_CHECK:
			rx_data_counter++;
            gl_tMsg.usData[rx_data_counter] = inputBuf[0];
            gl_tMsg.bcc_check_code = inputBuf[0];
			gl_tMsg.data_length = rx_data_counter;
			if(gl_tMsg.bcc_check_code == bcc_check(gl_tMsg.usData, gl_tMsg.data_length))
			{
				state=0;
				rx_data_counter=0;  
				 gl_tMsg.tx_data_success = 1;
				freertos_decoder_isr_handler();

			}
			else{
				state=0;
				rx_data_counter=0;
			}


		break;

        //this is receive data 
		case UART_STATE_DATA_LEN: //receive is data ->"0x04"

             rx_data_counter++;
             gl_tMsg.usData[rx_data_counter] = inputBuf[0];

              gl_tMsg.receive_data_length = gl_tMsg.usData[rx_data_counter];
              gl_tMsg.data_length=0;
           // 根据数据长度判断是否需要接收载荷
            if(gl_tMsg.receive_data_length > 0 && gl_tMsg.copy_cmd_notice != 0xFF) {
				 gl_tMsg.rc_data_length=0;
                 state = UART_STATE_DATA;
            } 
            else if(gl_tMsg.copy_cmd_notice == 0xFF){ //this is older compatibility 
                 gl_tMsg.execuite_cmd_notice =  gl_tMsg.usData[rx_data_counter];
				 state = UART_STATE_FRAME_END; //receive data.
			}
			else {
                // 如果数据长度为0，直接跳到帧尾
                rx_data_counter=0;
                state = 0;
            }
        break;

        case UART_STATE_DATA:

        rx_data_counter++;
        gl_tMsg.data_length ++;
        gl_tMsg.usData[rx_data_counter] = inputBuf[0];
		gl_tMsg.rx_data[gl_tMsg.rc_data_length]=inputBuf[0];
		gl_tMsg.rc_data_length++;
         
        if(gl_tMsg.data_length == gl_tMsg.receive_data_length){
              
             state = UART_STATE_DATA_END;

        }

        break;

        case UART_STATE_DATA_END:

        rx_data_counter++;
        gl_tMsg.usData[rx_data_counter] = inputBuf[0];
        if(gl_tMsg.usData[rx_data_counter]==0xFE){
             
             state = UART_STATE_DATA_BCC;

        }
        else{
            state=0;
            rx_data_counter=0;

        }
       break;

       case UART_STATE_DATA_BCC:

            rx_data_counter++;
           // gl_tMsg.data_length = rx_data_counter;
            gl_tMsg.usData[rx_data_counter] = inputBuf[0];
            gl_tMsg.bcc_check_code = inputBuf[0];
			gl_tMsg.data_length = rx_data_counter;
			
            if(gl_tMsg.bcc_check_code == bcc_check(gl_tMsg.usData,gl_tMsg.data_length ))
            {
                state=0;
                rx_data_counter=0; 
				 gl_tMsg.tx_data_success = 1;
                freertos_decoder_isr_handler();
				
               

            }
            else{
                state=0;
                rx_data_counter=0;
            }


       break;
     	}

	}
}

#endif 
/********************************************************************************
	**
	*Function Name:void usart1_isr_callback_handler(void)
	*Function : parse this is receive data from mainboard.
	*Input Ref:NO
	*Return Ref:NO
	*
*******************************************************************************/
void parse_recieve_data_handler(void)
{
  
    
	switch(gl_tMsg.copy_cmd_notice){ //cmd or notice .

	case 0:
      
       receive_cmd_or_data_handler();
	   gl_tMsg.tx_data_success = 0;

   break;

   case 0x0FF: //copy cmd or notice,this is older version protocol.
		receive_copy_cmd_or_data_handler();
		 gl_tMsg.tx_data_success = 0;

   case 0x80:
       receive_copy_cmd_or_data_handler();
	    gl_tMsg.tx_data_success = 0;
   break;

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
void receive_cmd_or_data_handler(void)
{

 
   	switch(gl_tMsg.cmd_notice){

   

     case 0:


     break;

     case power_on_off: 

        if(gl_tMsg.execuite_cmd_notice  == 0x01){ //open
          
            buzzer_sound();//buzzer_sound_fun();
            SendWifiData_Answer_Cmd(0x01,0x01);
            vTaskDelay(pdMS_TO_TICKS(5));
            
           
           	gpro_t.gpower_on = power_on;
           
  

        }
        else if(gl_tMsg.execuite_cmd_notice  == 0x0){ //close 

              SendWifiData_Answer_Cmd(0x01,0x02); //power off .
              vTaskDelay(pdMS_TO_TICKS(10)); 
              buzzer_sound();
              freertos_set_prority();
             
              
             powerOffTunrOff_flag=1;
             gpro_t.gpower_on = power_off;
			 
			
        }

     break;

     case ptc_on_off: //PTC?????????

     if(gl_tMsg.execuite_cmd_notice  == 0x01){
          buzzer_sound();
          gctl_t.gDry = 1;
	      gpro_t.ptc_switch_flag =open;

        
         if(gpro_t.stopTwoHours_flag==0){
         if(gpro_t.ptc_warning ==0 && gpro_t.fan_warning_flag ==0){ //PTC warning flag
              PTC_SetHigh();
              gpro_t.ptc_switch_flag =open;
		     gctl_t.gTimer_senddata_panel=7;
          }
      
         }
       }
       else if(gl_tMsg.execuite_cmd_notice  == 0x0){
        buzzer_sound();
          gctl_t.gDry =0;
        
         PTC_SetLow();
         gpro_t.ptc_switch_flag =close;
         gctl_t.gTimer_senddata_panel=7;

       }

     break;

     case plasma_on_off: //PLASMA ACTIVE OPEN OR CLOSE

        if(gl_tMsg.execuite_cmd_notice == 0x01){
           
            buzzer_sound();
           
           gctl_t.gPlasma = 1;
           gpro_t.plasma_switch_flag =1;
          if(gpro_t.stopTwoHours_flag==0){
            PLASMA_SetHigh();
          }
        }
        else if(gl_tMsg.execuite_cmd_notice  == 0x0){
           buzzer_sound();
           
           gctl_t.gPlasma = 0;
           gpro_t.plasma_switch_flag =2;
        
          PLASMA_SetLow();

        }


     break;


      case 0x04: //ultrasonic  ACTIVE OPEN OR CLOSE

        if(gl_tMsg.execuite_cmd_notice  == 0x01){  //open 
          
          gctl_t.gUlransonic =1;
          gpro_t.ultrasonic_switch_flag =1;

        }
        else if(gl_tMsg.execuite_cmd_notice  == 0x0){ //close 

          gctl_t.gUlransonic = 0;
          gpro_t.ultrasonic_switch_flag =2;

        }


     break;

      case 0x05: // link wifi command

       if(gl_tMsg.execuite_cmd_notice == 0x01){  // link wifi 
        
         
          gpro_t.link_net_step =0;
	      net_t.wifi_link_net_success=0;
          gpro_t.wifi_led_fast_blink_flag =1;
          gctl_t.wifi_config_net_lable=wifi_set_restor;
		  wifi_t.runCommand_order_lable= wifi_link_tencent_cloud;//2 
		  
          gctl_t.gTimer_linkTencentCounter=0; //total times is 120s
          SendWifiData_Answer_Cmd(0x05,0x01); //WT.EDIT 2024.12.28
          vTaskDelay(pdMS_TO_TICKS(10));

        }
        else if(gl_tMsg.execuite_cmd_notice  == 0x0){ //don't link wifi 

        }


     break;

     case 0x06: //buzzer sound command 

          buzzer_sound();
		  //vTaskDelay(pdMS_TO_TICKS(5));
     break;

     case 0x16 : //buzzer sound command with answer .

        buzzer_sound();
        

		  gpro_t.answer_buzzer_flag = 1;//WT.EDIT 2025.07.28 
          SendWifiData_Answer_Cmd(0x16,0x01); //WT.EDIT 2025.07.28
		  vTaskDelay(pdMS_TO_TICKS(5));
		  
       break;


      case 0x1A: //??????

      

           gctl_t.set_temperature_value = gl_tMsg.rx_data[0]  ;
           if(wifi_link_net_state()==1){
             MqttData_Publis_SetTemp(gctl_t.set_temperature_value);
		     vTaskDelay(pdMS_TO_TICKS(200));//osDelay(200);//HAL_Delay(350);
            }

        
      break;

     

     case 0x22: //PTC notice 

      if(gl_tMsg.execuite_cmd_notice == 0x01){
        

        gctl_t.gDry = 1;
		gpro_t.ptc_switch_flag = open;
   
        if(gpro_t.stopTwoHours_flag ==0){
              PTC_SetHigh();
              gpro_t.ptc_switch_flag =open;
              gctl_t.gTimer_senddata_panel=7;
          }
          
      }
      else if(gl_tMsg.execuite_cmd_notice == 0x0){
        
         gctl_t.gDry =0;
         PTC_SetLow();
          gpro_t.ptc_switch_flag =close;
          gctl_t.gTimer_senddata_panel=7;
      }

     break;

     case 0x27:

      if(gl_tMsg.execuite_cmd_notice == 0x02){
       
         gctl_t.gModel=2;
         //SendWifiData_Answer_Cmd(0x27,0x02); //don't AI mode,   WT.EDIT 2025.01.06
          if(wifi_link_net_state()==1){
            MqttData_Publish_SetState(2);
	        vTaskDelay(pdMS_TO_TICKS(200));//osDelay(100);//HAL_Delay(350);
           }
        
          
       }
       else if(gl_tMsg.execuite_cmd_notice == 0x01){ //AI mode 
       
         gctl_t.gModel=1;
        // SendWifiData_Answer_Cmd(0x27,0x01); //AI mode,WT.EDIT 2025.01.06
         if(wifi_link_net_state()==1){
             MqttData_Publish_SetState(1);
    	     vTaskDelay(pdMS_TO_TICKS(200));//osDelay(100);//HAL_Delay(350);
         }
       }


     break;
     	}
}


#if 0
     case 0xFF: //copy send cmd acknowlege
     //power on or power off 
        if(pdata[3]==0x31){ //smart phone normal :power on
            if(pdata[4]==1){ //power on

                gpro_t.receive_copy_cmd = ack_app_power_on;

            }
            else if(pdata[4]==2){ //smart phone normal :power off
               gpro_t.receive_copy_cmd = ack_app_power_off;
            }

        }
        else if(pdata[3]==0x21){ //smart phone of App timer power on .

            if(pdata[4]==1){ //power on

                gpro_t.receive_copy_cmd = ack_app_timer_power_on;

            }
            else if(pdata[4]==2){ //smart phone normal :power off
               gpro_t.receive_copy_cmd = ack_app_power_off;
            }


        }
        else if(pdata[3] == 0x05){ //link wifi command copy command..

        if(pdata[4]==1){

            gpro_t.receive_copy_cmd = ack_wifi_on;

        }


        }

      break;
        
#endif 

/**********************************************************************
*
*Function Name:void send_cmd_ack_hanlder(void);
*Function: 
*Input Ref:NO
*Return Ref:NO
*
**********************************************************************/
void receive_copy_cmd_or_data_handler(void)
{

    
     switch(gl_tMsg.cmd_notice){
    
        case ack_null:
    
    
        break;
    
        case power_on_off:
            
          if(gpro_t.receive_copy_cmd == ack_app_power_on){
             gpro_t.receive_copy_cmd =0;
             gpro_t.send_ack_cmd = 0;
            
          }
          else if(gpro_t.receive_copy_cmd != 0 && gpro_t.gTimer_again_send_power_on_off >1){
             gpro_t.gTimer_again_send_power_on_off =0;
              SendWifiData_To_Cmd(0x31,0x01); //smart phone is power on
              vTaskDelay(pdMS_TO_TICKS(10));
          }
                    
         
        break;
    
        case ack_app_power_off :
    
         if(gpro_t.receive_copy_cmd == ack_app_power_off){
            gpro_t.receive_copy_cmd =0;
             gpro_t.send_ack_cmd = 0;
             
          }
          else if(gpro_t.receive_copy_cmd != 0 && gpro_t.gTimer_again_send_power_on_off >1){
              gpro_t.gTimer_again_send_power_on_off =0;
               SendWifiData_To_Cmd(0x31,0x0); //smart phone is power off
               vTaskDelay(pdMS_TO_TICKS(10));
          }
    
    
        break;

        case ack_app_timer_power_on:

           if(gpro_t.receive_copy_cmd == ack_app_timer_power_on){
             gpro_t.receive_copy_cmd =0;
             gpro_t.send_ack_cmd = 0;
            
          }
          else if(gpro_t.receive_copy_cmd != 0 && gpro_t.gTimer_again_send_power_on_off >1){
             gpro_t.gTimer_again_send_power_on_off =0;
              SendWifiData_To_Cmd(0x21,0x01); //smart phone is power on
              vTaskDelay(pdMS_TO_TICKS(10));
          }

        break;
    
        case ack_wifi_on:
    
          if(gpro_t.receive_copy_cmd == ack_wifi_on){
             gpro_t.receive_copy_cmd =0;
             gpro_t.send_ack_cmd = 0;
            
          }
          else if(gpro_t.receive_copy_cmd != 0 && gpro_t.gTimer_again_send_power_on_off >1){
             gpro_t.gTimer_again_send_power_on_off =0;
             SendWifiData_To_Data(0x1F,0x0);
		     vTaskDelay(pdMS_TO_TICKS(10));

			 
          }
    
    
        break;
    
        case ack_ptc_on:
    
    
        break;
    
        case ack_ptc_off:
    
        break;
    
    
        }
 

}


