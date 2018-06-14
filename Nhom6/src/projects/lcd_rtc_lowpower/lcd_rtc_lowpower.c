/*
 * File:		lcd_rtc_lowpower.c
 * Purpose:	        RTC and sLCD are clocked from RTC_CLKIN
 *                      configure RTC to interrupt ever X seconds,
 *                      LLWU configured to wake up form RTC alarm
 *                      sLCD display RTC_TSR register and enter in low power mode
 *                                configure  MODE_SELECT to desired low power mode to test
 *
 */

#include "common.h"
#include "lcd.h"
#include "rtc.h" 
#include "smc.h"
#include "llwu.h"
#include "GPIO.h"

#define MODE_WAIT  0
#define MODE_STOP  1
#define MODE_LLS   2
#define MODE_VLLS3 3
//#define MODE_VLLS2   Not supported on KL46\
   
#define MODE_VLLS1 5
#define MODE_RUN 6
#define  MODE_SELECT  MODE_VLLS1


#ifdef CMSIS
#include "start.h"
#endif

char gu8USB_Buffer[10];
extern int mcg_clk_hz;
extern int mcg_clk_khz;
extern int core_clk_khz;
extern int periph_clk_khz;
unsigned char op_mode; 

__no_init char last_gpio_status;  //__no_init is an IAR compiler specific
__no_init char rtc_isrv_counter;  //__no_init is an IAR compiler specific
int llwu_isrv_counter; 


void print_llwu_status(void)
{

 printf("\n\r LLWU_PEx %x, %x, %x, %x", LLWU_PE1,LLWU_PE2,LLWU_PE3,LLWU_PE4);
 printf("\n\r LLWU_ME %x,",LLWU_ME);
 printf("\n\r LLWU_Fx %x, %x, %x, ", LLWU_F1,LLWU_F2,LLWU_F3);

 

}

/********************************************************************/
int main (void)
{
    	char ch;
        char low_power_mode = MODE_SELECT;
        
         
        SMC_PMPROT = SMC_PMPROT_ALLS_MASK | SMC_PMPROT_AVLLS_MASK | SMC_PMPROT_AVLP_MASK ;  
        
#ifdef CMSIS  // If we are conforming to CMSIS, we need to call start here
    start();
#endif
        print_llwu_status();
        gpio_init();
               
        rtc_init();  // driven by 32kHz external oscillator //

        vfnLCD_Init();
        vfnLCD_Write_Msg("1234");      
             
        
    
        PORTC_PCR3 = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
        enable_irq(INT_LLW-16);
        llwu_configure(0x0080,0x02,LLWU_ME_WUME5_MASK); //M5= RTC Alarm,  M7 RTC seconds
        
        
   //check if it comes from VLLx mode
        if (PMC_REGSC % PMC_REGSC_ACKISO_MASK)
        {
          if (last_gpio_status&0x01) LED1_ON; else LED1_OFF;  //configure GPIO pin with latest state
          PMC_REGSC |= PMC_REGSC_ACKISO_MASK; //write to release hold on I/O
          last_gpio_status ^= 0x01;  //update state for next work
        }

        
  	printf("\n\rRunning the lcd_rtc_lowpower project.\n\r");
        
        // wait 3 seconds before start entering in low power mode
        while  (rtc_seconds_isrv_count<3){};
        

	while(1)
	{
           LED2_TOGGLE;
           
           ch=  (char)RTC_TSR;
	   sprintf(gu8USB_Buffer,"%4i",ch );
           vfnLCD_Write_Msg(gu8USB_Buffer);	
           printf("\n\r llwu_isrv_counter = %i, rtc_isrv_counter=%i ",llwu_isrv_counter, rtc_isrv_counter );
           printf("                       "); //delay 
          
        
           switch (low_power_mode)
           { 
             case MODE_WAIT:
             enter_wait();
             printf("\n\r exit from WAIT"); 
             break;
             
           case MODE_STOP:
           /* 0x00 = STOP - Normal Stop Mode
              0x40 = PSTOP1 - Partial Stop with both system and bus clocks disabled
              0x80 = PSTOP2 - Partial Stop with system clock disabled and bus clock enabled
           */
             enter_stop(0x00);

             printf("\n\r exit from STOP"); 
           break;
             
           case MODE_LLS:  
            MCG_C6 &= ~MCG_C6_CME0_MASK;  //Disable Clock Monitor before entering in low power mode LLS, VLSSx
            enter_lls();
            op_mode = what_mcg_mode();
            if(op_mode==PBE)
            {
              mcg_clk_hz = pbe_pee(CLK0_FREQ_HZ);
              MCG_C6 |= MCG_C6_CME0_MASK;
            }
            printf("\n\r exit from LLS");  
            break;
            
            case MODE_VLLS3:  
            MCG_C6 &= ~MCG_C6_CME0_MASK;
            enter_vlls3();
            //Note:  exit from VLLSx mode is through RESET PMC_REGSC_ACKISO is set
            break;
            
          // VLLS2 mode not supported on KL46       
            
            case MODE_VLLS1:  
            MCG_C6 &= ~MCG_C6_CME0_MASK;
            enter_vlls1();  //In VLLS1 and VLLS0 no SRAM is retained.
            //Note:  exit from VLLSx mode is through RESET PMC_REGSC_ACKISO is set            
            break;            
            
           default:
           }          
	} 
}
/********************************************************************/
