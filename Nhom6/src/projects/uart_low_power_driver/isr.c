/*
 * File:		isr.c
 * Purpose:		KL46 testing of uart receive
 *                      interup service routines
 * I/O:                  Uart 0 UART0_RX(pin 11) and UART0_TX (pin 9) J2  
 *                      Terminal - on REVA KL46 FRDM Board UART1
 */
#include "common.h"
#include "uart_low_power_driver.h"

extern unsigned char receive_char;

void uart0_isr(void)
{
   volatile unsigned int dummyread;
   if ((UART0_S1 & 0x0f) == 0)
        receive_char =  UART0_D;
   else 
   {
       if ((UART0_S1 & 0x01) == 1)
         receive_char =  PARITY_ERROR_Flag;
       if ((UART0_S1 & 0x02) == 2)
         receive_char =  FRAMING_ERROR_Flag;
       if ((UART0_S1 & 0x04) == 4)
         receive_char =  NOISE_ERROR_Flag;
       if ((UART0_S1 & 0x08) == 8)
         receive_char =  RX_OVERRUN_Flag;     
   }
   UART0_S1 = (UART0_S1 & 0x0f) ;  // write 1 to clear
   dummyread = UART0_S1;
   
}
