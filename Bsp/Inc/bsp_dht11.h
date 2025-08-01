#ifndef __BSP_DHT11_H__
#define __BSP_DHT11_H__
#include "main.h"

/******************************************************************************************/
/* DHT11 引脚 定义 */

//TEMP_SENSOR_GPIO_Port 



#define DHT11_DQ_GPIO_PORT                  GPIOB
#define DHT11_DQ_GPIO_PIN                   GPIO_PIN_13
#define DHT11_DQ_GPIO_CLK_ENABLE()          do{ __HAL_RCC_GPIOB_CLK_ENABLE(); }while(0)   /* GPIOA口时钟使能 */

/******************************************************************************************/

/* IO操作函数 */
#define DHT11_DQ_OUT(x)     do{ x ? \
                                HAL_GPIO_WritePin(DHT11_DQ_GPIO_PORT, DHT11_DQ_GPIO_PIN, GPIO_PIN_SET) : \
                                HAL_GPIO_WritePin(DHT11_DQ_GPIO_PORT, DHT11_DQ_GPIO_PIN, GPIO_PIN_RESET); \
                            }while(0)                                                /* 数据端口输出 */
#define DHT11_DQ_IN         HAL_GPIO_ReadPin(DHT11_DQ_GPIO_PORT, DHT11_DQ_GPIO_PIN)  /* 数据端口输入 */


uint8_t dht11_init(void);   /* 初始化DHT11 */
uint8_t dht11_check(void);  /* 检测是否存在DHT11 */
uint8_t dht11_read_data(uint8_t *temp,uint8_t *humi);   /* 读取温湿度 */

void Update_DHT11_Value(void);

void Update_Dht11_Totencent_Value(void);    
                            

#endif 







