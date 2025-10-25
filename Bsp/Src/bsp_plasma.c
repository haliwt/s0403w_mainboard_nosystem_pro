#include "bsp.h"



void plasma_fun(uint8_t sel)
{
     if(sel ==0){
         PLASMA_SetLow();

     }
     else{

        PLASMA_SetHigh();
     }

}


void plasma_on(void)
{
	PLASMA_SetHigh();

}

void plasma_off(void)
{
	PLASMA_SetLow();


}



