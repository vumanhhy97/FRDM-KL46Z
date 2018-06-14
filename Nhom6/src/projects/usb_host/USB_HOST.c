/*
 * File:		usb_host.c
 * Purpose:		Main process
 *
 */

#include "common.h"
#include "uart.h"
#include "hidmouse.h"

/********************************************************************
* Functions
********************************************************************/
void vfnMcuConfig(void);

/********************************************************************
* User definitions
********************************************************************/
#define CLKOUT

#define USB_PLL_PRDIV           4       //Divide 8MHz XTAL by 4 = 2MHz
#define USB_PLL_VDIV            48      //Multiply 2MHz ref with 48 = 96MHz

/* USB Reset stress test */
//#define USB_RESET_STRESS_TEST

/********************************************************************/
int main (void)
{
  vfnMcuConfig();               //Initialize Mcu for use with USB
  
  usb_host_main();              //Execute USB host routine

  for(;;)
  {
    //Loop forever
  }
}
/********************************************************************/

void vfnMcuConfig (void)
{ 
#if defined(CLKOUT)
  SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK;            //Enable clock on PTC3
  PORTC_PCR3 = PORT_PCR_MUX(0x05);              //PTC3 as CLKOUT
  SIM_SOPT2 &= ~SIM_SOPT2_CLKOUTSEL_MASK;       //Clear CLKOUTSEL register
  SIM_SOPT2 |= SIM_SOPT2_CLKOUTSEL(0x02);       //Select BUSCLK as CLKOUT output
#endif
  
  /* Actual PLL frequency is 48MHz --> 96MHz needed for USB to work due freq divider by 2 */
  
  /* Set dividers for new PLL frequency */
  SIM_CLKDIV1 = ( 0
                        | SIM_CLKDIV1_OUTDIV1(1)
                        | SIM_CLKDIV1_OUTDIV4(1) );
  
  /* Return MCG to PBE */
  MCG_C6 |= MCG_C6_PLLS_MASK;
  MCG_C2 &= ~MCG_C2_LP_MASK;
  MCG_C1 &= ~MCG_C1_IREFS_MASK;
  MCG_C1 |= MCG_C1_CLKS(2);
  
  /* Move MCG to FBE */
  MCG_C6 &= ~MCG_C6_PLLS_MASK;
  MCG_C5 &= ~MCG_C5_PLLCLKEN0_MASK;
  
  /* Configure PLL to run @96MHz */
  MCG_C6 &= ~MCG_C6_VDIV0_MASK;
  MCG_C5 &= ~MCG_C5_PRDIV0_MASK;
  MCG_C6 |= (USB_PLL_VDIV - 24);
  MCG_C5 |= (USB_PLL_PRDIV - 1);
  MCG_C5 |= MCG_C5_PLLCLKEN0_MASK;              //Enable PLL
  while(!(MCG_S & MCG_S_LOCK0_MASK));           //Wait for PLL to lock
  
  /* Go to PBE */
  MCG_C6 |= MCG_C6_PLLS_MASK;
  
  /* Go to PEE */
  MCG_C1 &= ~MCG_C1_CLKS_MASK;
  
  /* MCG is now configured */
  
  /* Reconfigure UART0 with new frequency */
#if TERM_PORT_NUM==1
    uart_init (UART1_BASE_PTR, 24000, TERMINAL_BAUD); 
#elif TERM_PORT_NUM==2
     uart_init (UART2_BASE_PTR, 24000, TERMINAL_BAUD);     
#else
    uart0_init (UART0_BASE_PTR, 48000, TERMINAL_BAUD);
#endif
  
  
}