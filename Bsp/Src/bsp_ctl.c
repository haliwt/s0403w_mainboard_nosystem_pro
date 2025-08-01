#include "bsp.h"


_run_t gctl_t; 
uint8_t powerOffFanRun_flag ;
uint8_t powerOffTunrOff_flag;
uint8_t gTimer_powerOffRunFan;
uint8_t stopHours_flag;







/**********************************************************************
	*
	*Functin Name: void Single_ReceiveCmd(uint8_t cmd)
	*Function : resolver is by usart port receive data  from display panle  
	*Input Ref:  usart receive data
	*Return Ref: NO
	*
**********************************************************************/
void SystemReset(void)
{
    
		
		__set_PRIMASK(1) ;
		HAL_NVIC_SystemReset();
		

}




/**********************************************************************
    *
    *Functin Name: void main_function_detected_handler(uint8_t cmd)
    *Function : 
    *Input Ref:  key of value
    *Return Ref: NO
    *
************************************************************************/
void main_function_detected_handler(uint8_t cmd)
{

if(gctl_t.gTimer_senddata_panel >1  &&  cmd ==0){ //300ms
         gctl_t.gTimer_senddata_panel=0;
         
           ActionEvent_Handler();
     }


}


  
