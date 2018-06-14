/*
 * File:		TWRKL46_Demo.c
 * Purpose:		Main process
 * PTC3 shared with sLCD
 * Electrodes are shared sLCD
*/

#include "common.h"
#include "lcd.h"
#include "gpio.h"
#include "TSIDrv.h"
#include "global.h"
#include "pit.h"
#include "task_mgr.h"
#include "usb_cdc.h"
#include "usb_reg.h"
#include "adc16.h"
#include "adc_test.h"
#include "median.h"   
#include "hal_i2c.h"
#include "hal_dev_mma8451.h"
#include "angle_cal.h"
#include "uart.h"
#include "rtc.h" 
#include "adc_test.h"
#include "adc16.h"


#ifdef CMSIS
#include "start.h"
#endif

#define CLKOUT
#define USB_PLL_PRDIV           4       //Divide 8MHz XTAL by 4 = 2MHz
#define USB_PLL_VDIV            48      //Multiply 2MHz ref with 48 = 96MHz



/*****************************************************************************************************
* Definition of module wide VARIABLEs - NOT for use in other modules
*****************************************************************************************************/
signed short resultx, resulty, resultz;

struct mediana_type arr_medianas[4];   

int adc_pot;


/* Testing Suite */
#define USB_DEVICE_XCVR_TEST

   
   
   
uint8 gu8USB_Buffer[64];
int t_flash=300;



   
extern uint8 gu8USB_State;
extern uint8 gu8USB_Flags;
extern uint8 gu8EP3_OUT_ODD_Buffer[];
extern tBDT tBDTtable[];




const uint8 RAIN_SEQUENCE[] = 
  {
    0x01,
    0x02,
    0x04,   
    0x08,
    0x00,
  };


#define TIME_LED_SEQ   25 // base time for sequence falling

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
#elif  TERM_PORT_NUM==2
  uart_init (UART2_BASE_PTR, 24000, TERMINAL_BAUD);
#endif
   
}



void task_accel_led_test(void)
 {
   static uint16 period_led;
   static uint16 sw1_count;
   static uint16 sw2_count;
    
   static int rain_sequence_off;
   static int note,notez;
   static char aux_1,aux_SW1, aux_SW2;
   
   note  = xz_angle/100;  //9-9
   if (!ti_led)
        {
         if (note <-9) note=-9;
         if (note >9)  note =9;
         if (notez < note) notez++;
         if (notez > note) notez--;       
         if (notez>0)
         {
           if (++rain_sequence_off >= sizeof(RAIN_SEQUENCE)) rain_sequence_off=0;
           ti_led = (10-notez)*TIME_LED_SEQ; 
          }
         if (notez<0)
         {
           if (--rain_sequence_off < 0) rain_sequence_off=sizeof(RAIN_SEQUENCE);
           ti_led = (10+notez)*TIME_LED_SEQ; 
         }  
         if (RAIN_SEQUENCE[rain_sequence_off] & 0x1) LED1_ON;else LED1_OFF;
         if (RAIN_SEQUENCE[rain_sequence_off] & 0x2) LED2_ON;else LED2_OFF;
         if (RAIN_SEQUENCE[rain_sequence_off] & 0x4) LED3_ON;else LED3_OFF;
         if (RAIN_SEQUENCE[rain_sequence_off] & 0x8) LED4_ON;else LED4_OFF;
        }  
    
       if (notez==0)
       {
         if (RTC_TSR & 0x01)
         { 
           LED1_ON;
          
         }
         else
         { LED1_OFF; 
       
         }
         if (SW1_ON || Electrode1) LED3_ON;else LED3_OFF;
         if (Electrode2) LED4_ON ; else LED4_OFF;
                 
         if (input_rise(SW1_ON,&aux_SW1)) printf("\n\r SW1 click %i \n\r",sw1_count++);
         if (input_rise(SW2_ON,&aux_SW2)) printf("\n\r SW2 click %i \n\r",sw2_count++);
             
        
         if (ti_led<period_led/2) LED2_ON;else LED2_OFF;
         
         if (!ti_led)
         { 
           period_led=adc_sample/10;
           if (period_led<40) period_led=40;
           ti_led=  period_led;    //adc value goes from 0 to 4095      40 to 409
         }
       }
   
         if (input_rise(base_time125m%0x2,&aux_1))
         { 
                sprintf(gu8USB_Buffer,"%4i", adc_pot);
                vfnLCD_Write_Msg(gu8USB_Buffer);
                printf("\r yz=[%03i],  xz[%03i] ,xy[%03i],  POT=%i, rtc = %i, E1=%03i, E2=%03i ",yz_angle/10, xz_angle/10,xy_angle/10, adc_pot,rtc_seconds_isrv_count, gu16Delta[0], gu16Delta[1]);
              
         }               
   
 }



void task_test_slider(void);

void vfn_led_test(void)
 {
   static char cont_sec=0;
   if (!ti_task)
   {
     ti_task = t_flash;    
     switch (++cont_sec)
     {
      case 1:
        LED1_OFF;
        LED2_OFF;
       break;
       
      case 2:
        LED1_ON;
        LED2_OFF;
       break;

      case 3:
        LED1_OFF;
        LED2_ON;
       break;

      case 4:
        LED1_OFF;
        LED2_ON;
       break;

      case 5:
        LED1_OFF;
        LED2_OFF;
       break;


       
       
      default:
         next_task(task_test_slider);
     }
    }
 }


/******************************************************************************/
void usb_init(void)
{
      //printf("\n********** USB XCVR Module Testing **********\n");
      USB_REG_SET_ENABLE;
      CDC_Init();
      // Pull up enable
      FLAG_SET(USB_CONTROL_DPPULLUPNONOTG_SHIFT,USB0_CONTROL);
}
/******************************************************************************/
void usb_service(void)
{
    // If data transfer arrives
    if(FLAG_CHK(EP_OUT,gu8USB_Flags))
    {
        (void)USB_EP_OUT_SizeCheck(EP_OUT);
        usbEP_Reset(EP_OUT);
        usbSIE_CONTROL(EP_OUT);
        FLAG_CLR(EP_OUT,gu8USB_Flags);

        // Send it back to the PC
        EP_IN_Transfer(EP2,CDC_OUTPointer,1);
    }
           if (!ti_print)
            {
             ti_print = 300;
             sprintf((char *)gu8USB_Buffer,"\r RTC = %5i s", RTC_TSR);
             EP_IN_Transfer(EP2,gu8USB_Buffer,15);
            }
            
}



/*configures TPM0_CH3   green LED to be PWM controlled*/
void tpm_init(void)
{
   SIM_SCGC6 |= SIM_SCGC6_TPM0_MASK;   //enable TPM clock gate
   SIM_SOPT2 |= SIM_SOPT2_TPMSRC(1) | SIM_SOPT2_PLLFLLSEL_MASK;  //select PLL as clock source of 
   
   TPM0_MOD = 1000;  
   TPM0_C3SC = TPM_CnSC_MSB_MASK |TPM_CnSC_ELSB_MASK;  //PWM edge aligned
   TPM0_C3V = 500;
   TPM0_SC |= TPM_SC_PS(0) | TPM_SC_CMOD(1);
  
}


void task_hello(void)
 {
   vfnLCD_Write_Msg("-HI-");
 }


void task_lcd_init_test(void)
{
   vfnLCD_Set_Display();
   if (!ti_task)
   {
   vfnLCD_Clear_Display();
   _FREESCALE_ON();
   next_task(task_accel_led_test);
   }
   
}

void task_test_lcd(void)
{
  static char count_mode=0;
  if (!ti_task)
  {
    ti_task = 200;
    if (count_mode++>=10)count_mode=0;
    
    switch(count_mode)
    {
    vfnLCD_Set_Display();
    case 0: vfnLCD_Write_Msg("0000");  break;
    case 1: vfnLCD_Write_Msg("1111");  break;
    case 2: vfnLCD_Write_Msg("2222");  break;
    case 3: vfnLCD_Write_Msg("3333");  break;
    case 4: vfnLCD_Write_Msg("4444");  break;
    case 5: vfnLCD_Write_Msg("5555");  break;
    case 6: vfnLCD_Write_Msg("6666");  break;
    case 7: vfnLCD_Write_Msg("7777");  break;
    case 8: vfnLCD_Write_Msg("8888");  break;
    case 9: vfnLCD_Write_Msg("9999");  break;
    }
  
  } 
}

void task_test_slider(void)
{
     static int touch_slider_last_valid_value=50;
     if (AbsolutePercentegePosition>0) touch_slider_last_valid_value=AbsolutePercentegePosition;
     t_flash = (touch_slider_last_valid_value*2 + 50);
     printf("\r tsi test%%= %03i  ", (char)AbsolutePercentegePosition);
     
     
     
  if (!ti_task)
   {
     ti_task = t_flash;    
     LED1_TOGGLE;
     LED2_TOGGLE;
     LED3_TOGGLE;
     LED4_TOGGLE;
   }      
}


void task_accel_sensor(void)
{
   if (!ti_task)
   {
     ti_task = 200;    
     accel_read();
   }    
}

/* accelerometer */


void accel_demo(void)
{
        static int note,notez;
        static int cont=0x1;

        note  = yz_angle/100;  //9-9
        if (note != notez && xz_mag > 20 && !ti_delay)
        {
         if (note <-9) note=-9;
         if (note >9)  note =9;
         if (notez < note) notez++;
         if (notez > note) notez--;

         cont = cont << 0x1;
         if (cont >= 0x10 || cont==0) cont = 0x01;
         ti_delay = 75;
        }
}





#define TIME_LED_SEQ   25 // base time for sequence falling


char check_uart_present_character(void)
{

#if TERM_PORT_NUM==0
  return uart0_getchar_present(UART0_BASE_PTR);
#elif TERM_PORT_NUM==1
  return uart_getchar_present(UART1_BASE_PTR);
#elif TERM_PORT_NUM==2
  return uart_getchar_present(UART2_BASE_PTR);
#endif
}




/********************************************************************/
int main (void)
{
    	char ch;
      
     
            int UsbDetected = FALSE;
#ifdef CMSIS  // If we are conforming to CMSIS, we need to call start here
    start();
#endif
        vfnMcuConfig(); //recofigure clocks to support usb module
  	printf("\n\rRunning the TWR-KL46_Demo project.\n\r");
        TSI_Init();
        gpio_init();
        Pit_init();
        vfnLCD_Init();
        rtc_init();  // driven by 32kHz external oscillator
        usb_init();
        next_task_t(task_lcd_init_test,3000);
        accel_init();
        adc_init();        
        tpm_init();  //Green LED 50%
        accel_init();

        
        
// character test        
        vfnLCD_Write_Msg("8888");
       
        
     
        
	while(1)
	{
          
          if (check_uart_present_character())
          {
            ch = in_char();
            printf("\n\r Received char = %c \n\r",ch);            
           if (ch==' ')
           {     
             printf("\n\r adc: pot  = %i",adc_pot);            
             printf("\n\r yz=[%03i],  xz[%03i] ,xy[%03i] ",yz_angle/10, xz_angle/10,xy_angle/10);
             printf("\n\r  Electrode1= %i, Electrode2 = %i", gu16Delta[0], gu16Delta[1]); 
            }
           }
           ptr_next_task();  // do the actual function
            usb_service();
            if (gu8USB_State == uENUMERATED && !UsbDetected) 
            {
               UsbDetected = TRUE;
            }        
                
          if (!ti_accel_sampling)
           {
             ti_accel_sampling = 20; 
             accel_read();
             angle_calculation(); //-900  to  900            
             detect_fall_detection();
             accel_demo();
             adc_pot = adc_read(4)>>2;
           }
            
         if (!ti_tsi_multiplexing)
         {
           ti_tsi_multiplexing=10;
           vfnLCD_disable_module();         
           vfn_start_tsi_scan();
           while(!end_flag){};
       
           TSI_Electrode_detection(); //this function clears end_flag
       
           vfnLCD_enable_module();
          
         }     
                
        }
}
/********************************************************************/
