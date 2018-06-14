/*
 * File:		isr.c
 * Purpose:		KL46 testing of uart receive
 *                      interup service routines
 * I/O:                  Uart 0 UART0_RX(pin 11) and UART0_TX (pin 9) J2  
 *                      Terminal - on REVA KL46 FRDM Board UART1
 */
#include "common.h"
#include "uart_low_power_wu_dut.h"

extern unsigned char receive_char,error_flag;

void uart0_isr(void)
{
   volatile unsigned int dummyread;
//   if (((UART0_S2 & UART0_S2_RXEDGIF_MASK) >> UART0_S2_RXEDGIF_SHIFT) == 1) 
//       UART0_S2 |= UART0_S2_RXEDGIF_MASK;  // clear the flag

//   while(((UART0_S2 & UART0_S2_RAF_MASK) >> UART0_S2_RAF_SHIFT) == 1);
       
//   }
   if ((( UART0_S1 & UART0_S1_RDRF_MASK)>> UART0_S1_RDRF_SHIFT) == 1)
     receive_char =  UART0_D;
   
   if ((UART0_S1 & 0x0f) != 0) 
   {
       error_flag = 0;
       if ((UART0_S1 & 0x01) == 1)
         error_flag =  PARITY_ERROR_Flag;
       if ((UART0_S1 & 0x02) == 2)
         error_flag =  FRAMING_ERROR_Flag;
       if ((UART0_S1 & 0x04) == 4)
         error_flag =  NOISE_ERROR_Flag;
       if ((UART0_S1 & 0x08) == 8) {
         error_flag =  RX_OVERRUN_Flag;    
       }
       UART0_S1 |= UART0_S1_PF_MASK |
                   UART0_S1_FE_MASK |
                   UART0_S1_NF_MASK |
                   UART0_S1_OR_MASK  ;  // write 1 to clear error flags
   }
   dummyread = UART0_S1;
    
}

void porta_isr(void)
{
  PORTA_PCR15 |= PORT_PCR_ISF_MASK; // Falling edge
  /* clear the port interrupt */
  printf(" \n\rPortA15 falling edged detected and flag cleared\r\n");
}
  
  
  