#ifndef __BSP_CMD_LINK_H_
#define __BSP_CMD_LINK_H_
#include "main.h"
typedef enum ack_sig{

  ack_null,
  ack_power_on ,
  ack_power_off,
  ack_wifi_on,
  ack_ptc_on,
  ack_ptc_off,
  ack_plasma_on,
  ack_plasma_off,
  ack_ultra_on,
  ack_ultra_off,
  //wifi cmd

  ack_app_power_on,
  ack_app_power_off,
  ack_app_timer_power_on,
  
  

}ack_e;

extern uint8_t rx_wifi_data[7];

//extern uint8_t inputBuf[4];

extern uint8_t  inputCmd[30];
extern uint8_t wifiInputBuf[1];



extern void (*EUSART_TxDefaultInterruptHandler)(void);
extern void (*EUSART_RxDefaultInterruptHandler)(void);

void EUSART_SetRxInterruptHandler(void (* interruptHandler)(void));



void sendData_Real_TimeHum(uint8_t hum,uint8_t temp);


void SendWifiData_To_PanelWindSpeed(uint8_t dat1);


void SendWifiData_To_PanelTime(uint8_t hours,uint8_t minutes,uint8_t seconds);


//void SendData_Set_Command(uint8_t cmd,uint8_t data);

void SendWifiData_To_Cmd(uint8_t cmd,uint8_t data);


void SendWifiData_To_Data(uint8_t cmd,uint8_t data);
void SendWifiData_Answer_Cmd(uint8_t cmd ,uint8_t data);



void USART1_Cmd_Error_Handler(UART_HandleTypeDef *huart);
void USART2_Cmd_Error_Handler(UART_HandleTypeDef *huart);


#endif 
