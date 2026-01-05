#include "bsp.h"

#define S03_MAX_DATA_LEN   4
//#define S03_HEADER_MAIN    0x5A
#define S03_HEADER_DISPLAY 0xA5 //display board of head 
#define S03_TAIL           0xFE

typedef struct
{
    uint8_t header;      // 帧头 0x5A / 0xA5
    uint8_t dev_addr;    // 设备地址/类型
    uint8_t cmd_type;    // 命令类型：指令/应答/通知...
    uint8_t func_code;   // 功能码：0x01 开关机等
    uint8_t copy_type;   // 复制类型
    uint8_t data_len;    // 数据长度 N
    uint8_t data[S03_MAX_DATA_LEN]; // 数据区
    uint8_t tail;        // 帧尾 0xFE
    uint8_t bcc;         // BCC 校验
    uint8_t frame_length;
	uint8_t frame_one_frame;
} S03Frame_t;


typedef enum {
    S03_STATE_WAIT_HEADER = 0,
    S03_STATE_DEV_ADDR,
    S03_STATE_CMD_TYPE,
    S03_STATE_FUNC_CODE,
    S03_STATE_DATA_LEN,
    S03_STATE_DATA,
    S03_STATE_TAIL,
    S03_STATE_BCC
} S03_State_e;

S03Frame_t frame;

static S03_State_e s_state = S03_STATE_WAIT_HEADER;

static void receive_cmd_or_notice_handler(const S03Frame_t  pd);

static void parse_recieve_copy_data_handler(const S03Frame_t  pd);

static void S03_Frame_Dispatch(const S03Frame_t f);
static void S03_Protocol_ByteHandler(uint8_t *chdata,uint8_t len);


/*****************************************************************************
	**
	*Function Name
	*Function :  BCC 校验函数
	*Input Ref:NO
	*Return Ref:NO
	*
*******************************************************************************/
static uint8_t s03_calc_bcc(const uint8_t *buf, uint16_t len)
{
    uint8_t bcc = 0;
    for(uint16_t i = 0; i < len; i++)
    {
        bcc ^= buf[i];
    }
    return bcc;
}





/*****************************************************************************
	**
	*Function Name
	*Function :  逐字节状态机（核心）,每来一个字节就进一次
	*Input Ref:NO
	*Return Ref:NO
	*
*******************************************************************************/
void S03_Protocol_ByteHandler(uint8_t *pdch,uint8_t len)
{
     uint8_t recv_bcc,calc_bcc ;

 
	
   
            if(pdch[0+len] == S03_HEADER_DISPLAY && pdch[1+len]==0x01)
            {
               
                s_state = S03_STATE_CMD_TYPE;
            }
			else{

                return ;
			}
         

      

          if(s_state ==S03_STATE_CMD_TYPE){

		     frame.cmd_type = pdch[2+len]; 
             if(frame.cmd_type == 0xFF){
                 frame.copy_type = pdch[3+len];
                 frame.func_code = pdch[4+len];
			     if(frame.func_code == 0x0F){
                      frame.data_len = pdch[5+len];
					  if(frame.data_len==1){

                         frame.data[0]= pdch[6+len];
						 frame.tail = pdch[7+len];
					      recv_bcc = pdch[8+len];
						 frame.frame_length = 9;

					  }


				 }
				 else{
				 	frame.data_len = pdch[5+len];//frame.data_len=0;
				    frame.tail = pdch[6+len];//0xFE
					recv_bcc = pdch[7+len];
					frame.frame_length = 8;
			        S03_Frame_Dispatch(frame);
				 }
				 return ;
             }
			 else{

                frame.func_code = pdch[3+len]; 
		        if(frame.func_code ==0x0F){
					frame.data_len = pdch[4+len];//数据的长度
					if(frame.data_len >0){
		            if( frame.data_len== 1)  // 0~4头信息 + N 数据
		            {
		                frame.data[0] = pdch[5+len]; //第一个数据
						frame.tail  = pdch[6+len];
						 recv_bcc = pdch[7+len];
						 s_state = S03_STATE_BCC;
						 frame.frame_length = 8;
						 if(frame.tail == 0xFE){
						    S03_Frame_Dispatch(frame);
						 }
						 return;
		            }
					else if(frame.data_len == 2){
		                 frame.data[0] = pdch[5+len]; //第一个数据
		                 frame.data[1] = pdch[6+len]; //第二个数据
		                 frame.tail = pdch[7+len];
						 recv_bcc = pdch[8+len];
						 s_state = S03_STATE_BCC;
						 frame.frame_length = 9;
						 if(frame.tail == 0xFE){
						   S03_Frame_Dispatch(frame);
						 }
						 return;
		            }
					else if(frame.data_len == 3){
		                frame.data[0] = pdch[5+len]; //第一个数据
		                frame.data[1]=  pdch[6+len]; //第一个数据
		                frame.data[2] = pdch[7+len]; //第三个数据
		                frame.tail   = pdch[8+len] ;
						recv_bcc = pdch[9+len];
						 s_state = S03_STATE_BCC;
						 frame.frame_length = 10;
						 if(frame.tail == 0xFE){
						     S03_Frame_Dispatch(frame);
						 }
						return;
		            }

					}
 					else{
						return;

					}


				}
				 frame.data_len = pdch[4+len];
				 frame.tail = pdch[5+len] ;
				 recv_bcc = pdch[6+len];
				 frame.frame_length = 7;
				 if(frame.data_len == 0 && frame.tail == 0xFE){
				    S03_Frame_Dispatch(frame);
					return ;
				 }
				 else{
				    return ;
				 }

			  }
              
			}

      
    
}


/*****************************************************************************
	**
	*Function Name:void S03_Handle_Command(const S03Frame_t *f)
	*Function :  pase of display board function.
	*Input Ref:NO
	*Return Ref:NO
	*
*******************************************************************************/
#if 0
void S03_Handle_Command(const S03Frame_t *f)
{
    switch(f->func_code)
    {
        case 0x01: // 开关机
            // 数据长度一般是 1 byte: 0=关,1=开...
            break;

        case 0x02: // PTC 打开/关闭
            break;

        case 0x03: // PLASMA
            break;

        case 0x1A: // 读取传感器温湿度数据
            // f->data 里是实际温度/湿度/等
            break;

        default:
            break;
    }
}
#endif 
/*****************************************************************************
	**
	*Function Name:
	*Function :  
	*Input Ref:NO
	*Return Ref:NO
	*
*******************************************************************************/
void S03_Frame_Dispatch(const S03Frame_t f)
{
    // 先按命令类型分大类（指令/通知/应答）
  
       if(frame.cmd_type == 0xFF){// 举例：应答
               
          parse_recieve_copy_data_handler(f);
           // S03_Handle_Ack(f);
        }
        else{
          
          receive_cmd_or_notice_handler(f);
                 
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
static void receive_cmd_or_notice_handler(const S03Frame_t f)
{

   static uint8_t ptc_tx_default=0xff;
   static uint8_t plasma_tx_default =0xff, sonic_tx_default = 0xff;
   	switch(frame.cmd_type){

   

     case 0:


     break;

     case power_on_off: 

         
        if(frame.func_code == 0x01){ //open

	        buzzer_sound();//buzzer_sound_fun();
            SendWifiData_Answer_Cmd(0x01,0x01);
            vTaskDelay(pdMS_TO_TICKS(50));
          
            gpro_t.process_run_step=0;
           	gpro_t.gpower_on = power_on;
        }
        else if(frame.func_code == 0x0){ //close 

		
			  buzzer_sound();

              SendWifiData_Answer_Cmd(0x01,0x02); //power off .

              vTaskDelay(pdMS_TO_TICKS(50)); 
             
            
             gpro_t.power_off_run_step=1;
             gpro_t.gpower_on = power_off;
			 
		     
        }

     break;


	 case 0x30: //power on or off don't sound WT.EDIT 2026.01.04

        if(frame.func_code == 0x01){ //open

		     
	        SendWifiData_Answer_Cmd(0x01,0x01);
	        vTaskDelay(pdMS_TO_TICKS(50));
	            
	            gpro_t.process_run_step=0;
	           	gpro_t.gpower_on = power_on;

			
  

        }
        else if(frame.func_code == 0x0){ //close 

		
		
              SendWifiData_Answer_Cmd(0x01,0x02); //power off .

              vTaskDelay(pdMS_TO_TICKS(50)); 
             
             if(gpro_t.again_power_off_flag == 1)gpro_t.again_power_off_flag = 0;
			 else{
               gpro_t.power_off_run_step=1;
               gpro_t.gpower_on = power_off;

			 }
           
            
			 
		     
        }

	 break;

     case ptc_on_off: //PTC key of command .

     if(frame.func_code  == 0x01 && gpro_t.gpower_on == power_on){//phone_cmd_power

	      buzzer_sound();
		 if(gpro_t.stopTwoHours_flag==0){//two hours have a rest ten minutes .
         if(gpro_t.ptc_warning ==0 && gpro_t.fan_warning_flag ==0){ //PTC warning flag
              PTC_SetHigh();
              
		      gctl_t.gTimer_senddata_panel=7;//at once run ptc function.
          }
      
         }
          SendWifiData_Answer_Cmd(0x02,0x01); //
          vTaskDelay(pdMS_TO_TICKS(100)); 
		
          gctl_t.gDry = 1;
		  ptc_recoder_flag = 1;
		  if(ptc_tx_default != gpro_t.ptc_switch_flag){
		  	  gpro_t.ptc_switch_flag++;
		  	  ptc_tx_default = gpro_t.ptc_switch_flag;
	          

		  }
		  
		  if(gctl_t.app_timer_power_on_flag==1){
		  	gctl_t.app_timer_power_on_flag=0;
		    gctl_t.ptc_on_off_flag =0; //WT.EDIT 2025.12.19

		  }
  
      
	      
       }
       else if(frame.func_code== 0x0 && gpro_t.gpower_on == power_on){
	   
		 
          buzzer_sound();
          gctl_t.gDry =0;
	      PTC_SetLow();
		  ptc_recoder_flag = 0;
          SendWifiData_Answer_Cmd(0x02,0x0); //
          vTaskDelay(pdMS_TO_TICKS(50)); 
     
       
         gctl_t.gTimer_senddata_panel=7; //at once run ptc function.
		 gctl_t.app_timer_power_on_flag=0;
		 if(ptc_tx_default != gpro_t.ptc_switch_flag){
		  	  gpro_t.ptc_switch_flag++;
		  	  ptc_tx_default = gpro_t.ptc_switch_flag;
	          

		  }

       }
       

     break;

     case plasma_on_off: //PLASMA ACTIVE OPEN OR CLOSE

        if(frame.func_code == 0x01){
           
          buzzer_sound();
          SendWifiData_Answer_Cmd(0x03,0x01); //
          vTaskDelay(pdMS_TO_TICKS(50)); 
           
           gctl_t.gPlasma = 1;
		   if(gpro_t.stopTwoHours_flag==0){
               PLASMA_SetHigh() ;

		  }
		   
		   if(plasma_tx_default != gpro_t.plasma_switch_flag){
		   	    gpro_t.plasma_switch_flag++;
		      	plasma_tx_default = gpro_t.plasma_switch_flag;
		        
            }
           
          
        }
        else if( frame.func_code  == 0x0){
           buzzer_sound();
          SendWifiData_Answer_Cmd(0x03,0x0); //
          vTaskDelay(pdMS_TO_TICKS(50)); 
           
           gctl_t.gPlasma = 0;
		   PLASMA_SetLow();
		     if(plasma_tx_default != gpro_t.plasma_switch_flag){
		   	    gpro_t.plasma_switch_flag++;
		      	plasma_tx_default = gpro_t.plasma_switch_flag;
		        
            }
         
        
          PLASMA_SetLow();

        }


     break;


      case 0x04: //ultrasonic  ACTIVE OPEN OR CLOSE

        if(frame.func_code  == 0x01){  //open 
          
          gctl_t.gUlransonic =1;

		  if(gpro_t.stopTwoHours_flag==0){
               ultrasonic_open();

		  }
		  if(sonic_tx_default != gpro_t.ultrasonic_switch_flag){
                 gpro_t.ultrasonic_switch_flag++;
		         sonic_tx_default = gpro_t.ultrasonic_switch_flag;

		  }
          SendWifiData_Answer_Cmd(0x04,0x01); //
          vTaskDelay(pdMS_TO_TICKS(50)); 

        }
        else if(frame.func_code  == 0x0){ //close 

          gctl_t.gUlransonic = 0;

		  ultrasonic_close();
		  
          if(sonic_tx_default != gpro_t.ultrasonic_switch_flag){
                 gpro_t.ultrasonic_switch_flag++;
		         sonic_tx_default = gpro_t.ultrasonic_switch_flag;

		  }
          SendWifiData_Answer_Cmd(0x04,0x0); //
          vTaskDelay(pdMS_TO_TICKS(50)); 

        }


     break;

      case  wifi_link: // link wifi command

       if(frame.func_code == 0x01){  // link wifi 
        
        
          gpro_t.link_net_step =0;
	      net_t.wifi_link_net_success=0;
          gpro_t.wifi_led_fast_blink_flag =1;
          gctl_t.wifi_config_net_lable=wifi_set_restor;
		  wifi_t.runCommand_order_lable= wifi_link_tencent_cloud;//2 
		  
          gctl_t.gTimer_linkTencentCounter=0; //total times is 120s
          SendWifiData_Answer_Cmd(0x05,0x01); //WT.EDIT 2024.12.28
          vTaskDelay(pdMS_TO_TICKS(10));

      
        }


     break;

     case buzzer_sound_s: //buzzer sound command 
          
          buzzer_sound();
		  vTaskDelay(pdMS_TO_TICKS(5));
     break;

	 case 0x07: //AI command
	  if( frame.func_code == 0x02){
	       buzzer_sound();
		
          gctl_t.gModel=2;
          gctl_t.mode_ai_switch_flag =1;
          SendWifiData_Answer_Cmd(0x07,0x02); //
          vTaskDelay(pdMS_TO_TICKS(100)); 
        
          
       }
       else if(frame.func_code == 0x01){ //AI mode 
       
	
         buzzer_sound();
         gctl_t.gModel=1;
	     gctl_t.mode_ai_switch_flag =1;
         SendWifiData_Answer_Cmd(0x07,0x01); //
         vTaskDelay(pdMS_TO_TICKS(100)); 
		 
       }


	 break;

     case 0x16 : //buzzer sound command with answer .

        buzzer_sound();
        gpro_t.answer_buzzer_flag = 1;//WT.EDIT 2025.07.28 


        SendWifiData_Answer_Cmd(0x16,0x01); //WT.EDIT 2025.07.28

		vTaskDelay(pdMS_TO_TICKS(100));
		  
       break;


	  case 0x1A: //receive from display board set temperature value .

      case 0x19:
	   if(gpro_t.soft_version ==1){
	      // gctl_t.set_temperature_flag = 1; 
	     
	       gctl_t.set_temperature_value = frame.data[0]  ;
		   gctl_t.ptc_on_off_flag =0;
		   gctl_t.set_temp_first_closeptc=0;
		   gctl_t.rx_set_temp_flag =0;
		   
	
		   
	       if(wifi_link_net_state()==1){
	            MqttData_Publis_SetTemp(gctl_t.set_temperature_value);
			      vTaskDelay(pdMS_TO_TICKS(200));//osDelay(200);//HAL_Delay(350);
	        }
	   }
	   else{
	      gctl_t.set_temperature_value = frame.data[0] ;

	      if(wifi_link_net_state()==1){
	            MqttData_Publis_SetTemp(gctl_t.set_temperature_value);
			      vTaskDelay(pdMS_TO_TICKS(200));//osDelay(200);//HAL_Delay(350);
	        }


	   }

        
      break;

	  
	 case 0x1C: //display board to send time of value for two hours stop have a rest.
				//no sound is notice
			   if(frame.data[0]== 0x78 &&  gpro_t.soft_version ==1){ //2 hours 
	 
                  gpro_t.stopTwoHours_flag = 1;
				  SendWifiData_Answer_Cmd(0x1C,0x01); //WT.EDIT 2025.07.28
                  vTaskDelay(pdMS_TO_TICKS(100));
				  
				  #if DEBUG_FLAG

				  printf("rx_stopTwo_Hours_flag = 1 !!!!\r\n");

				  #endif 

                 
			 
			   }
			   else if(frame.data[0]== 0x0A && gpro_t.soft_version ==1){ //10mintues
                 gpro_t.stopTwoHours_flag =0;
				
			     SendWifiData_Answer_Cmd(0x1C,0x0); //WT.EDIT 2025.07.28
                 vTaskDelay(pdMS_TO_TICKS(100));
			      #if DEBUG_FLAG

				  printf("rx_stopTwo_Hours_flag = 0 @@@@@\r\n");

				  #endif 
				 ActionEvent_Handler();
			     
				
				 
			  }
	 
	 break;

     

     case 0x22: //PTC notice don't buzzer sound
        if(frame.func_code == 0x01){
        gctl_t.gDry = 1;
		ptc_recoder_flag =1 ;//WT.EDIT 2025.11.17
		gpro_t.ptc_switch_flag ++;
		
		
        if(gpro_t.stopTwoHours_flag ==0){
              PTC_SetHigh();
        }
		if(wifi_link_net_state()==1){ 
			MqttData_Publish_SetPtc(0x01);
			
		}

		 SendWifiData_Answer_Cmd(0x22,0x01); //WT.EDIT 2025.07.28
         vTaskDelay(pdMS_TO_TICKS(100));
		
   
			#if DEBUG_FLAG

			//  printf("disp_gDry = %d\r\n",gctl_t.gDry);

			#endif 
          
      }
      else if(frame.func_code == 0x0){

          gctl_t.gDry =0;
		  ptc_recoder_flag =0 ;
	
          PTC_SetLow();
          gpro_t.ptc_switch_flag++;
		  

		   SendWifiData_Answer_Cmd(0x22,0x0); //WT.EDIT 2025.07.28
           vTaskDelay(pdMS_TO_TICKS(100));

		  
		  if(wifi_link_net_state()==1){ 
			MqttData_Publish_SetPtc(0x0);
			
		  }
         
		  	
			#if DEBUG_FLAG

			//  printf("disp_gDry = %d\r\n",gctl_t.gDry);

			#endif 
			 
      }
   
     break;

     case 0x27: //AI command without buzzer sound

	    if(frame.func_code == 0x02){
		 
	          gctl_t.gModel=2;
			  gctl_t.mode_ai_switch_flag =1;
		}
	    else if(frame.func_code == 0x01){ //AI mode 
	        gctl_t.gModel=1;
		    gctl_t.mode_ai_switch_flag =1; 
		}
    break;

     case 0xF0: //software version difference older and new sotfware 
      
            gpro_t.soft_version = frame.data[0];
	 
				
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
static void parse_recieve_copy_data_handler(const S03Frame_t f)
{

    switch(frame.copy_type){
    
        case 0:
    
    
        break;
    
        case 0x10: //power on or off notice .--older version 
            
          if(frame.func_code == 0x01){
		  	 if(gpro_t.gpower_on == power_on){
               gpro_t.copy_cmd_notice_buff[1] =COPY_OK;
		  	 }
			 else{ 
             	gpro_t.copy_cmd_notice_buff[1] =COPY_NG;
				gpro_t.gTimer_timer_start_counter = 0;//start_timer(0);

			 }
            
          }
          else if(frame.func_code == 0x0){
             if(gpro_t.gpower_on == power_off)
                gpro_t.copy_cmd_notice_buff[1] =COPY_OK;
			 else 
             	 gpro_t.copy_cmd_notice_buff[1] =COPY_NG;
                 gpro_t.gTimer_timer_start_counter = 0;//start_timer(0);
          }
                    
         
        break;
    
        case 0x012 ://ptc open or close
    
          
          if(frame.func_code== 0x01){

		     if(ptc_recoder_flag ==1)//if(gctl_t.gDry ==1)
               gpro_t.copy_cmd_notice_buff[2] =COPY_OK;
			else if(ptc_recoder_flag ==0){
			  gpro_t.copy_cmd_notice_buff[2] =COPY_NG;
			  gpro_t.gTimer_timer_start_counter = 0;
			}
            
          }
          else if(frame.func_code == 0x0){
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
		  if(frame.func_code ==0x01){
           
             //gctl_t.ptc_warning =0;
			
	        ///gpro_t.fan_warning_flag =0;
	        //gpro_t.power_off_run_step=1;
	        //powerOffFanRun_flag = 1;
	
			//gpro_t.gpower_on = power_on;//gctl_t.rx_command_tag= POWER_ON;
			gpro_t.phone_power_on_flag = 0; //ack_app_power_on;
	     

		   }

		break;

		case 0x30: //phone power off 

		  if(frame.func_code  == 0x00){
	           //gpro_t.gpower_on = power_off;
			   //gpro_t.power_off_run_step=1; //WT.EDIT 2025.01.04
			   //powerOffFanRun_flag = 1;
			   gpro_t.phone_power_on_flag = 0; //ack_app_power_off;
		  }

		break;

        }
}

void disp_protocol_bytehandler(uint8_t *pdbuf)
{

   S03_Protocol_ByteHandler(pdbuf,0);
  
   S03_Protocol_ByteHandler(pdbuf,frame.frame_length);
   
   

}
