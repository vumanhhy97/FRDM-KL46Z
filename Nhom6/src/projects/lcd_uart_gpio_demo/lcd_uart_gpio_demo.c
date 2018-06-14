/*
 * File:		lcd_uart_gpio_demo.c
 * Purpose:		Main process
 *
 */

#include "common.h"
#include "uart.h"
#ifdef CMSIS
#include "start.h"
#endif

#define USB_PLL_PRDIV           4       //Divide 8MHz XTAL by 4 = 2MHz
#define USB_PLL_VDIV            48      //Multiply 2MHz ref with 48 = 96MHz

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
  
#if TERM_PORT_NUM==0
  uart0_init (UART0_BASE_PTR, 48000, TERMINAL_BAUD);
#elif TERM_PORT_NUM==1
  uart_init (UART1_BASE_PTR, 24000, TERMINAL_BAUD);
#else
  uart_init (UART2_BASE_PTR, 24000, TERMINAL_BAUD);
#endif
   
}

/*configures TPM0_CH3   green LED to be PWM controlled*/
void tpm_init(void)
{
   SIM_SCGC6 |= SIM_SCGC6_TPM0_MASK;   //enable TPM clock gate
   SIM_SOPT2 |= SIM_SOPT2_TPMSRC(1) | SIM_SOPT2_PLLFLLSEL_MASK;  //select PLL as clock source of 
   
   TPM0_MOD = 1000;  
   
#ifdef FRDM_REVA   
   TPM0_C3SC = TPM_CnSC_MSB_MASK |TPM_CnSC_ELSB_MASK;  //PWM edge aligned
   TPM0_C3V = 500;
#else
   TPM0_C5SC = TPM_CnSC_MSB_MASK |TPM_CnSC_ELSB_MASK;  //PWM edge aligned
   TPM0_C5V = 500;
  
#endif
   
    TPM0_SC |= TPM_SC_PS(0) | TPM_SC_CMOD(1);
  
}

void task_test_slider(void)
{
     static int touch_slider_last_valid_value=50;
     
     
     if (AbsolutePercentegePosition>0) touch_slider_last_valid_value=AbsolutePercentegePosition;
     t_flash = (touch_slider_last_valid_value*2 + 50);
     sprintf((char *)gu8USB_Buffer,"%4i", touch_slider_last_valid_value);
     
#ifdef FRDM_REVA     
     PORTA_PCR6 = PORT_PCR_MUX(3); //enable as TPM0_CH3
     TPM0_C3V=touch_slider_last_valid_value*10;
#else
     PORTD_PCR5 = PORT_PCR_MUX(4); //enable as TPM0_CH5
     TPM0_C5V=touch_slider_last_valid_value*10; 
#endif
         
     
     
     
     vfnLCD_Write_Msg(gu8USB_Buffer);
     printf("\r tsi %%= %03i  ", AbsolutePercentegePosition);
     
     
     
  if (!ti_task)
   {
     ti_task = t_flash;    
     LED1_TOGGLE;
     LED2_TOGGLE;
   }     
 
    if (input_rise(SW1_ON, &sw1_aux1)) 
   {
#ifdef FRDM_REVA
                 PORTA_PCR6 = PORT_PCR_MUX(1); //PTA6 as GPIO LED3
#else
                 PORTD_PCR5 = PORT_PCR_MUX(1); //PTA6 as GPIO LED3
                 
#endif  
     
     
       next_task(task_ecompass);
       printf("\n\r eCompass,  Magenetomer + accelerometer test ");  
   }
  
}

/********************************************************************/
int main (void)
{
    	char ch;
        
#ifdef CMSIS  // If we are conforming to CMSIS, we need to call start here
    start();
#endif
    vfnMcuConfig();
		tpm_init();
  	printf("\n\rRunning the lcd_uart_gpio_demo project.\n\r");

	while(1)
	{
		ch = in_char();
		out_char(ch);
	} 
}
/********************************************************************/
