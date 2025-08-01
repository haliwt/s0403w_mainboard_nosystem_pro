#include "bsp.h"

process_t gpro_t;

uint8_t counter_flag;

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */
  static uint16_t tim17_counter;
  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM14)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */
  else if (htim->Instance == TIM17)
  {
    tim17_counter++;
	if(tim17_counter>999){
		tim17_counter=0;
		counter_flag++;
	}
  }

  /* USER CODE END Callback 1 */
}