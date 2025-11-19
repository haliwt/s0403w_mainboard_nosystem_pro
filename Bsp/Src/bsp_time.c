#include "bsp.h"


// ????????
#define TWOH_RUN_DURATION_MIN   119 //(2U * 60U * 60U)  // 2 hours
#define TWOH_FAN_DURATION_SEC   (60U)             // fan run 1 minute
#define TWOH_PAUSE_DURATION_SEC 10       // rest 10 minutes

uint8_t counter_two_hours;


// ??????????????????????? new_works_run_two_hours_state()
typedef enum {
    TWOH_STATE_RUNNING,
    TWOH_STATE_FAN_COOLING,
    TWOH_STATE_PAUSED
} TwoHoursState_t;

uint8_t  beijing_step;

uint8_t real_hours,real_minutes,real_seconds;

uint8_t auto_link_net_flag;
uint8_t timer_fan_flag;//times_flag;
uint8_t twoHours_stop_flag;

/**********************************************************************
    *
    *Functin Name: void new_works_run_two_hours_state(void)
    *Function :  
    *Input Ref: NO
    *Return Ref: NO
    *
************************************************************************/
void new_works_run_two_hours_state(void)
{
   switch(gpro_t.two_hours_state){

    case TWOH_STATE_RUNNING:
        if(gpro_t.stopTwoHours_flag == 1){ // 119 seconds = 2 hours
            //close main board fun
            PLASMA_SetLow(); // 
            PTC_SetLow();
            ultrasonic_close();
            gctl_t.gTimer_fan_run_one_minute = 0;
         
            gpro_t.two_hours_state = TWOH_STATE_FAN_COOLING; // Transition to FAN_COOLING state
            #if DEBUG_FLAG

		     printf("gpro_t.stopTwoHours_flag = 1 \r\n");

			#endif 
        }
		else if(gctl_t.gTimer_senddata_panel >6){ //300ms
				 gctl_t.gTimer_senddata_panel=0;
				 
				 ActionEvent_Handler();
		 }
		  
		
        break;

    case TWOH_STATE_FAN_COOLING:
        if(gctl_t.gTimer_fan_run_one_minute < 61){
            fan_run_fun(); // SetLevel_Fan_PWMA(10);//Fan_RunSpeed_Fun();// FAN_CCW_RUN();
             PLASMA_SetLow(); // 
             PTC_SetLow();
            ultrasonic_close();
			counter_two_hours =0;//WT.EDIT 2025.11.05
			gpro_t.gTimer_check_twohours =0;
			gpro_t.gTimer_twohours_seconds_counter = 0; //WT.EDIT 2025.11.17
        } 
		else {
    
            FAN_Stop();
            gpro_t.two_hours_state = TWOH_STATE_PAUSED; // Transition to PAUSED state
            counter_two_hours =0;//WT.EDIT 2025.11.05
			gpro_t.gTimer_check_twohours = 0;
			gpro_t.gTimer_twohours_seconds_counter = 0; //WT.EDIT 2025.11.17
           #if DEBUG_FLAG
            
		     printf("two hours state fan one minutes \r\n");
	

		   #endif 
        }
		
        break;

    case TWOH_STATE_PAUSED:

            FAN_Stop();
            PLASMA_SetLow(); //
            PTC_SetLow();
            ultrasonic_close();
			gpro_t.ptc_switch_flag++;
			counter_two_hours =0;//WT.EDIT 2025.11.05
			gpro_t.gTimer_check_twohours = 0;
			gpro_t.gTimer_twohours_seconds_counter = 0; //WT.EDIT 2025.11.17
            gpro_t.two_hours_state = TWOH_STATE_RUNNING;
			#if DEBUG_FLAG

		     printf("two hours state is over\r\n");
			 printf("gpro_t.stopTwoHours_flag = 0 \r\n");

		   #endif 
        
        break;

        default:
           	
        break;

  }

  
}
/**********************************************************************
    *
    *Functin Name: void older_works_run_two_hours_state(void)
    *Function :  
    *Input Ref: NO
    *Return Ref: NO
    *
************************************************************************/
void older_works_run_two_hours_state(void)
{
  // static uint8_t twoHours_stop_flag;
   if(gpro_t.two_hours_state >TWOH_STATE_PAUSED){
       if(twoHours_stop_flag==0)gpro_t.two_hours_state = TWOH_STATE_RUNNING;
       else if(twoHours_stop_flag==1 || twoHours_stop_flag==2)gpro_t.two_hours_state = TWOH_STATE_FAN_COOLING;
	   else if(twoHours_stop_flag==3)gpro_t.two_hours_state = TWOH_STATE_PAUSED;
   }

	switch(gpro_t.two_hours_state){

    case TWOH_STATE_RUNNING:
        if(counter_two_hours > TWOH_RUN_DURATION_MIN){ // 119 seconds = 2 hours
            counter_two_hours = 0;
            gpro_t.gTimer_check_twohours = 0;
		    gpro_t.gTimer_twohours_seconds_counter = 0; 
			twoHours_stop_flag=0;

            PLASMA_SetLow(); //
            PTC_SetLow();
            ultrasonic_close();
            gctl_t.gTimer_fan_run_one_minute = 0;
            gpro_t.stopTwoHours_flag = 1;
            gpro_t.two_hours_state = TWOH_STATE_FAN_COOLING; // Transition to FAN_COOLING state
        }
		else if(gctl_t.gTimer_senddata_panel >6 &&  gpro_t.stopTwoHours_flag ==0){ //300ms
            gctl_t.gTimer_senddata_panel=0;
            
            ActionEvent_Handler();
        }
        break;

    case TWOH_STATE_FAN_COOLING:
        if(gctl_t.gTimer_fan_run_one_minute < 61){
			twoHours_stop_flag=1;
            fan_run_fun(); // SetLevel_Fan_PWMA(10);//Fan_RunSpeed_Fun();// FAN_CCW_RUN();
        } else {
			twoHours_stop_flag=2;
            FAN_Stop();
            gpro_t.two_hours_state = TWOH_STATE_PAUSED; // Transition to PAUSED state
        }
        break;

    case TWOH_STATE_PAUSED:
    //??(??)10??
        if(counter_two_hours > TWOH_PAUSE_DURATION_SEC){ // 10  minutes = 600 seconds
            counter_two_hours = 0;
			twoHours_stop_flag=0;
            gpro_t.gTimer_check_twohours = 0;
            gctl_t.gTimer_fan_adc_times = 0; // ADC be detected must be run 60s,after be detected ADC
            gpro_t.stopTwoHours_flag = 0;
            gpro_t.two_hours_state = TWOH_STATE_RUNNING; // Transition back to RUNNING state
            gpro_t.gTimer_twohours_seconds_counter = 0; //WT.EDIT 2025.11.17
            ActionEvent_Handler();
        }
        else{
            twoHours_stop_flag=3;
            FAN_Stop();
            PLASMA_SetLow(); //
            PTC_SetLow();
            ultrasonic_close();
        }
        break;

       default:
       
        break;

  }

}

/********************************************************************************
	*
	*Functin Name:void void getBeijingTime_cofirmLinkNetState_handler(void)
	*Functin :
	*Input Ref: NO
	*Return Ref:NO
	*
********************************************************************************/
void getBeijingTime_cofirmLinkNetState_handler(void)
{
   static uint8_t flag_switch;
    switch(gpro_t.get_beijing_flag){

     case 0: //WT.EDIT .2024.08.10

      if(net_t.wifi_link_net_success ==1 && gpro_t.gTimer_get_data_from_tencent_data > 9){ ////9
       
          gpro_t.gTimer_get_data_from_tencent_data =0;
           flag_switch++;

       

        if(flag_switch > 4 && gpro_t.gpower_on == power_on){
             flag_switch=0;
            wifi_t.get_rx_beijing_time_enable=0;
            Subscriber_Data_FromCloud_Handler();
            vTaskDelay(pdMS_TO_TICKS(100));//HAL_Delay(200)
            gpro_t.get_beijing_flag = 1;

            
        }
        else if(gpro_t.gpower_on == power_off && flag_switch > 4){
            flag_switch=0;
            wifi_t.get_rx_beijing_time_enable=0;
            Update_Dht11_Totencent_Value();
            gpro_t.get_beijing_flag = 1;

         }
         else{ //WT.EDIT 2024.08.10 ADD ITEM
              if(flag_switch > 5){
                 flag_switch=0;
              }
              wifi_t.get_rx_beijing_time_enable=0; 
              gpro_t.get_beijing_flag = 1;
             

         }
       
    }
    else{
	 if(net_t.wifi_link_net_success ==0){
	    SendWifiData_To_Data(0x1F,0x0); //WT.EDIT 2025.04.02 0x1F: wifi link net is succes 
	    vTaskDelay(pdMS_TO_TICKS(5));
	 }
     gpro_t.get_beijing_flag = 1;

    }

   break;


   case 1:

        if(wifi_t.gTimer_get_beijing_time > 65){//WT.EDIT 2025.01.06 //100

         wifi_t.gTimer_get_beijing_time=0;
        
          if(wifi_link_net_state()==1){

     
    		    gpro_t.get_beijing_flag = 2;
           
      
                net_t.linking_tencent_cloud_doing  =0; //receive from tencent command state .
                SendWifiData_To_Data(0x1F,0x01);
                vTaskDelay(pdMS_TO_TICKS(10));

            }
            else{
              
               SendWifiData_To_Data(0x1F,0x0); //0x1F: 0x1=wifi link net is succes ,0x0 = wifi link net is fail
               vTaskDelay(pdMS_TO_TICKS(10));
               gpro_t.get_beijing_flag = 10;
               net_t.linking_tencent_cloud_doing  =1; //receive from tencent command state .
               gpro_t.send_ack_cmd = 1; //ack_wifi_on;
                gpro_t.gTimer_timer_start_counter=0;
             }
	
       }
       else{

            gpro_t.get_beijing_flag = 0;
       }

      break;


      case 2:


	    if(gpro_t.get_beijing_time_success == 0){
		

         gpro_t.get_beijing_flag = 3;
         beijing_step =0; //WT.EDIT 2024.08.10
	      
		wifi_t.gTimer_auto_detected_net_state_times =0;
	
		wifi_t.get_rx_beijing_time_enable=0;//disenable get beijing timing
		
	
       }
	   else{

          wifi_t.get_rx_beijing_time_enable=0;//disenable get beijing timing
		
		    gpro_t.get_beijing_flag = 3;
			beijing_step =0; //WT.EDIT 2024.08.10

        }


     
    break;


     case 3:

         switch(beijing_step){

         case 0:
            //disable publish data to tencent cloud.
            gpro_t.gTimer_get_data_from_tencent_data=0;

    		
    		Get_BeiJing_Time_Cmd();
    	    vTaskDelay(pdMS_TO_TICKS(100));//osDelay(100);//HAL_Delay(20); //WT.EDIT .2024.08.10//HAL_Delay(20);
    	    beijing_step =1;

         break;

         case 1:
                //disable publish data to tencent cloud.
                gpro_t.gTimer_get_data_from_tencent_data=0;
                wifi_t.get_rx_beijing_time_enable=1;
        		gpro_t.wifi_rx_data_counter =0;
        		Get_Beijing_Time();
              
        	    vTaskDelay(pdMS_TO_TICKS(100));//osDelay(100);//HAL_Delay(20); //WT.EDIT .2024.08.10
                
        	
                beijing_step =2;
          
         

          break; 


          case 2:

            if(gpro_t.wifi_rx_data_array[50] > 0x31 ){
           
                real_hours = (gpro_t.wifi_rx_data_array[41]-0x30)*10 + gpro_t.wifi_rx_data_array[42]-0x30;
                real_minutes =(gpro_t.wifi_rx_data_array[44]-0x30)*10 + gpro_t.wifi_rx_data_array[45]-0x30;
                real_seconds = (gpro_t.wifi_rx_data_array[47]-0x30)*10 + gpro_t.wifi_rx_data_array[48]-0x30;

                //memcpy(time_str,gpro_t.wifi_rx_data_array,sizeof(gpro_t.wifi_rx_data_array));
                ///net_parse_beijint_time();

                wifi_t.get_rx_beijing_time_enable=0; //enable beijing times
                gpro_t.disp_works_hours = real_hours;    
                    gpro_t.disp_works_minutes = real_minutes;

                    gpro_t.gTimer_works_time_seconds = real_seconds;
                    gpro_t.get_beijing_time_success = 1;

                    SendWifiData_To_PanelTime(gpro_t.disp_works_hours,gpro_t.disp_works_minutes,gpro_t.gTimer_works_time_seconds);
                    vTaskDelay(pdMS_TO_TICKS(10));//osDelay(50);

                   gpro_t.get_beijing_flag = 6; //WT.EDIT 2025.01.06
                    
                    
                
                }
                else if(gpro_t.wifi_rx_data_array[50] == 0x31){  //"0x31" ASCII = '1'

                   wifi_t.get_rx_beijing_time_enable=0; //enable beijing times
                   if(wifi_link_net_state()==1){
                       
                       gpro_t.gTimer_get_data_from_tencent_data=0;
                       wifi_t.gTimer_get_beijing_time = 50;
                       gpro_t.get_beijing_flag = 6;//WT.EDIT 2025.01.06
                      

                    }
                    else 
                       gpro_t.get_beijing_flag = 6;//WT.EDIT 2025.01.06
            }
            else{

              gpro_t.get_beijing_flag = 6;//WT.EDIT 2025.01.06
              

            }
            break;
         }
    break;


    case 6:

  
       confirm_wifi_link_net_state();
    

       wifi_t.gTimer_auto_detected_net_state_times=0;  

       gpro_t.get_beijing_flag = 0;
    
    break;

    //auto link net 

    case 10:

     if(wifi_t.gTimer_auto_detected_net_state_times > 100){

		      wifi_t.gTimer_auto_detected_net_state_times=0;

    

         if(net_t.wifi_link_net_success ==0){
            gpro_t.get_beijing_flag = 11;
            net_t.linking_tencent_cloud_doing  =1; //receive from tencent command state .
            gpro_t.wifi_rx_data_counter=0;
            SendWifiData_To_Data(0x1F,0x0); //WT.EDIT 2025.04.02 0x1F: wifi link net is succes 
            vTaskDelay(pdMS_TO_TICKS(10));
           
          }
          else{
             wifi_t.soft_ap_config_flag =1; //WE.EIDT 
             net_t.linking_tencent_cloud_doing  =0; //receive from tencent command state .
             gpro_t.get_beijing_flag = 0;
		     SendWifiData_To_Data(0x1F,0x01); //WT.EDIT 2025.04.02 0x1F: wifi link net is succes 
			 vTaskDelay(pdMS_TO_TICKS(10));
          }
        
       }

     
     if(wifi_link_net_state()==1){
          gpro_t.get_beijing_flag = 0;

     }
       
     break;


     case 11:
         if(wifi_link_net_state()==0 && gpro_t.wifi_led_fast_blink_flag==0){

           net_t.linking_tencent_cloud_doing =1;
        

          //  WIFI_IC_ENABLE();
       
    		at_send_data("AT+RST\r\n", strlen("AT+RST\r\n"));
            vTaskDelay(pdMS_TO_TICKS(1000));//HAL_Delay(1000);
            wifi_t.gTimer_auto_link_net_time =0;
            auto_link_net_flag=1;

            gpro_t.get_beijing_flag = 12;

         }
         else {

             gpro_t.get_beijing_flag = 10;


         }
        
               

     break;


     case 12:

        if(wifi_t.gTimer_auto_link_net_time > 2 && auto_link_net_flag==1){


            wifi_t.gTimer_auto_link_net_time=0;
            gpro_t.wifi_rx_data_counter=0;
	        wifi_t.soft_ap_config_flag =1;
             auto_link_net_flag =2;
   
//	        HAL_UART_Transmit(&huart2, "AT+TCMQTTCONN=1,5000,240,0,1\r\n", strlen("AT+TCMQTTCONN=1,5000,240,0,1\r\n"), 0xffff);//瀵�1�?7?婵绻涢幒?
            vTaskDelay(pdMS_TO_TICKS(1000));//HAL_Delay(1000);
         
           
		
	    }

        if(wifi_t.gTimer_auto_link_net_time > 4 && auto_link_net_flag==2){

            auto_link_net_flag=0 ;
            gpro_t.get_beijing_flag = 13;
           
        }


     break;


     case 13:
       if(wifi_link_net_state()==1){
       
         

        net_t.linking_tencent_cloud_doing  =0; //receive from tencent command state .
	    gpro_t.wifi_rx_data_counter=0; //clear USART2 counter is zero
		wifi_t.soft_ap_config_flag =0; 
	

          if(gpro_t.gpower_on == power_on){
                MqttData_Publish_Update_Data();//Publish_Data_ToTencent_Initial_Data();
                vTaskDelay(pdMS_TO_TICKS(200));//osDelay(200);//HAL_Delay(200);

            }
            else if(gpro_t.gpower_on == power_off){

               MqttData_Publish_PowerOff_Ref();
               vTaskDelay(pdMS_TO_TICKS(200));//osDelay(200);//HAL_Delay(200);


            }
            Subscriber_Data_FromCloud_Handler();
            osDelay(200);//HAL_Delay(200);

            SendWifiData_To_Data(0x1F,0x01); //0x1F: wifi link net is succes 
			vTaskDelay(pdMS_TO_TICKS(10));
             gpro_t.get_beijing_flag = 0;
		
         }
         else{
			 SendWifiData_To_Data(0x1F,0x0); //WT.EDIT 2025.04.02 0x1F: wifi link net is succes 
			 vTaskDelay(pdMS_TO_TICKS(10));
             gpro_t.get_beijing_flag = 10;
         }

       break;
	

	
        }

}




