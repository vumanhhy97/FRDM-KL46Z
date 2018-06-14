/*
 * File:		TWRPI_sLCD.c
 * Purpose:		Main process
 *
 */

#include "common.h"
#include "LCD.h"

#ifdef CMSIS
#include "start.h"
#endif

/*****************Global Variables*********************************************/
/*LCD Buffer*/
int8 s8DispBuffer[10];   // Buffer used to store formatted output from sprintf function

/********************************************************************/
int main (void)
{
    	char ch;
        uint16 u16cnt=0;
        
#ifdef CMSIS  // If we are conforming to CMSIS, we need to call start here
    start();
#endif
        /*LCD Init*/
        vfnLCD_Init();
        
        /*Turn On All LCD segments*/
        vfnLCD_Set_Display(); 
        
        /*Turn Off All LCD segments*/
        vfnLCD_Clear_Display();
        
        /*Turn On FREESCALE segment*/
        _FREESCALE_ON();      
  	
        printf("\n\rRunning the TWRPI_sLCD project.\n\r");
        
        /* Assign the counter value to the buffer*/ 
        sprintf(s8DispBuffer,"%04d",u16cnt);                             
        vfnLCD_Write_Msg_GoTo(s8DispBuffer,4,0);      //Display formatted buffer
        
	while(1)
	{
		
                ch = in_char();
		out_char(ch);
                u16cnt++;
                if(u16cnt>1999) u16cnt=0;
                /* Assign the counter value to the buffer*/ 
                sprintf(s8DispBuffer,"%04d",u16cnt);                             
                vfnLCD_Write_Msg_GoTo(s8DispBuffer,4,0);      //Display formatted buffer
                
	} 
}
/********************************************************************/