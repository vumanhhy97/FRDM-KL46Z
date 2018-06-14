/*
 * File:		uart_low_power_wu_dut.c
 * Purpose:		KL46 testing of uart receive
 *                      where we wake up from VLPS or LLS with a received
 *                      character and catch 1st character.
 * I/O:                  Uart 0 UART0_RX(pin 11) and UART0_TX (pin 9) J2  
 *                      Terminal - on REVA KL46 FRDM Board UART1
 */

#include "common.h"
#include "uart_low_power_wu_dut.h"
#include "UART.h"
#include "smc.h"
#include "isr.h"
#include "lptmr.h"
#include "mcg.h"

#ifdef CMSIS
#include "start.h"
#endif

/* global variables */
/* = 9600, 19200, 57600,115200,230400,460800,1000000,2000000  */
int baud_rate[9] ;
extern int uart0_clk_khz;
extern int mcg_clk_hz;
extern int mcg_clk_khz;
extern int core_clk_khz;
extern int periph_clk_khz;

unsigned char receive_char,error_flag;

/********************************************************************/
int main (void)
{
        unsigned char test_num = UNDEF_VALUE;
        unsigned char op_mode;  
        volatile unsigned int dummyread;

#ifdef CMSIS  // If we are conforming to CMSIS, we need to call start here
        start();
#endif
        
  	printf("\n\rRunning the uart_low_power_wu_dut project.\n\r");
        printf("        Initializing UART0_Rx and UART0_Tx\n\r");
        // init baud rate array
        baud_rate[0] = 4800;
        baud_rate[1] = 9600;
        baud_rate[2] = 19200;
        baud_rate[3] = 57600;
        baud_rate[4] = 115200;
        baud_rate[5] = 230400;
        baud_rate[6] = 460800;
        baud_rate[7] = 1000000;
        baud_rate[8] = 2000000;
        // fAST IRC TRIM
        op_mode = atc(FAST_IRC, FAST_IRC_FREQ, PLL_FREQ);
        // SLOW IRC TRIM
        op_mode = atc(SLOW_IRC, SLOW_IRC_FREQ, PLL_FREQ);
        printf("\n\r auto trim performed atc return = %d \r\n",op_mode );
        /* Enable the UART_TXD function on PTA1 */
        PORTA_PCR14 = PORT_PCR_MUX(0x3);          
        /* Enable the UART_TXD function on PTA2 */
        PORTA_PCR15 = PORT_PCR_MUX(0x3);
     while (1)
     {
        test_num = get_test_num();
        switch( test_num) {
        case VLPS_EN_OSCERCLK_57600:
            //SIM_SOPT2 |= SIM_SOPT2_UART0SRC(1); // select the PLLFLLCLK as UART0 clock source
            // 10 OSCERCLK clock
            SIM_SOPT2 &= ~SIM_SOPT2_UART0SRC_MASK;
            SIM_SOPT2 |= SIM_SOPT2_UART0SRC(2); // select the OSCERCLK as UART0 clock source
            //SIM_SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK; // set PLLFLLSEL to select the PLL for this clock source       
            OSC0_CR |= OSC_CR_ERCLKEN_MASK | OSC_CR_EREFSTEN_MASK ;
            uart0_clk_khz = (CLK0_FREQ_HZ / 1000); // UART0 clock frequency will equal the OSCERCLK frequency
            //uart0_clk_khz = ((mcg_clk_hz / 2) / 1000); // UART0 clock frequency will equal half the PLL frequency
            uart0_test_init (UART0_BASE_PTR, uart0_clk_khz, baud_rate[3]);
            
         //     for (receive_char =0x30;receive_char < 0x3a;receive_char ++)
         //     {
         //        uart0_putchar(UART0_BASE_PTR, receive_char); // SEND A CHAR 9 TO UART0
         //     }
            UART0_C3 |= UART0_C3_NEIE_MASK | UART0_C3_ORIE_MASK  | UART0_C3_FEIE_MASK; 
            UART0_C2 |= UART0_C2_RIE_MASK;  // enable receive interrupt
            enable_irq (UART0SE_irq_no);
            if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 4){
    	          printf("  in VLPR Mode !  ");
            } else if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 1){
    	          printf("  in Run Mode  !  ");
            }
            op_mode = what_mcg_mode();
            if (op_mode ==1) 
                printf("in BLPI mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==2) 
                printf(" in FBI mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==3) 
                printf(" in FEI mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==4) 
                printf(" in FEE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==5) 
                printf(" in FBE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==6) 
                printf(" in BLPE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==7) 
                printf(" in PBE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==8) 
                printf(" in PEE mode now at %d Hz\r\n",mcg_clk_hz );
            printf("  Receiving Char from VLPS enabled OSCERCLK at 57600 baud \n\r  ");
            printf(" Hit a CTRL L to exit this loop \r\n");
            
            while(receive_char != 0x0c)  // hit CTRL L to exit
            {
               time_delay_ms(1); 
               enter_vlps();
               if (error_flag ==  PARITY_ERROR_Flag)
                 printf("PARITY_ERROR_Fla\n\n\r");
               if (error_flag ==  FRAMING_ERROR_Flag)
                 printf("FRAMING_ERROR_Flag\n\n\r");
               if (error_flag ==  NOISE_ERROR_Flag)
                 printf("NOISE_ERROR_Flag\n\n\r");
               if (error_flag ==  RX_OVERRUN_Flag)
                 printf("RX_OVERRUN_Flag\n\n\r");    
               
               uart0_putchar(UART0_BASE_PTR, receive_char); //echo to transmitter
               op_mode = what_mcg_mode();
               if(op_mode==PBE)
                      {
                           mcg_clk_hz = pbe_pee(CLK0_FREQ_HZ);
                      } 
               uart_putchar(UART1_BASE_PTR, receive_char);  // echo to terminal
                   
            }  
            break;
        case VLPS_EN_OSCERCLK_115200:
            //SIM_SOPT2 |= SIM_SOPT2_UART0SRC(1); // select the PLLFLLCLK as UART0 clock source
            // 10 OSCERCLK clock
            SIM_SOPT2 &= ~SIM_SOPT2_UART0SRC_MASK;
            SIM_SOPT2 |= SIM_SOPT2_UART0SRC(2); // select the OSCERCLK as UART0 clock source
            //SIM_SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK; // set PLLFLLSEL to select the PLL for this clock source       
            OSC0_CR |= OSC_CR_ERCLKEN_MASK | OSC_CR_EREFSTEN_MASK ;
            uart0_clk_khz = (CLK0_FREQ_HZ / 1000); // UART0 clock frequency will equal the OSCERCLK frequency
            //uart0_clk_khz = ((mcg_clk_hz / 2) / 1000); // UART0 clock frequency will equal half the PLL frequency
            uart0_test_init (UART0_BASE_PTR, uart0_clk_khz, baud_rate[4]);
            
         //     for (receive_char =0x30;receive_char < 0x3a;receive_char ++)
         //     {
         //        uart0_putchar(UART0_BASE_PTR, receive_char); // SEND A CHAR 9 TO UART0
         //     }
            UART0_C3 |= UART0_C3_NEIE_MASK | UART0_C3_ORIE_MASK  | UART0_C3_FEIE_MASK; 
            UART0_C2 |= UART0_C2_RIE_MASK;  // enable receive interrupt
            enable_irq (UART0SE_irq_no);
            if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 4){
    	          printf("  in VLPR Mode !  ");
            } else if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 1){
    	          printf("  in Run Mode  !  ");
            }
            op_mode = what_mcg_mode();
            if (op_mode ==1) 
                printf("in BLPI mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==2) 
                printf(" in FBI mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==3) 
                printf(" in FEI mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==4) 
                printf(" in FEE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==5) 
                printf(" in FBE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==6) 
                printf(" in BLPE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==7) 
                printf(" in PBE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==8) 
                printf(" in PEE mode now at %d Hz\r\n",mcg_clk_hz );
            printf("  Receiving Char from VLPS enabled OSCERCLK at 115200 baud \n\r  ");
            printf(" Hit a CTRL L to exit this loop \r\n");
            
            while(receive_char != 0x0c)  // hit CTRL L to exit
            {
               time_delay_ms(1); 
               enter_vlps();
               if (error_flag ==  PARITY_ERROR_Flag)
                 printf("PARITY_ERROR_Fla\n\n\r");
               if (error_flag ==  FRAMING_ERROR_Flag)
                 printf("FRAMING_ERROR_Flag\n\n\r");
               if (error_flag ==  NOISE_ERROR_Flag)
                 printf("NOISE_ERROR_Flag\n\n\r");
               if (error_flag ==  RX_OVERRUN_Flag)
                 printf("RX_OVERRUN_Flag\n\n\r");    
               
               uart0_putchar(UART0_BASE_PTR, receive_char); //echo to transmitter
               op_mode = what_mcg_mode();
               if(op_mode==PBE)
                      {
                           mcg_clk_hz = pbe_pee(CLK0_FREQ_HZ);
                      } 
               uart_putchar(UART1_BASE_PTR, receive_char);  // echo to terminal
                   
            }  
            break;
        case VLPS_EN_OSCERCLK_230400:
            //SIM_SOPT2 |= SIM_SOPT2_UART0SRC(1); // select the PLLFLLCLK as UART0 clock source
            // 10 OSCERCLK clock
            SIM_SOPT2 &= ~SIM_SOPT2_UART0SRC_MASK;
            SIM_SOPT2 |= SIM_SOPT2_UART0SRC(2); // select the OSCERCLK as UART0 clock source
            //SIM_SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK; // set PLLFLLSEL to select the PLL for this clock source       
            OSC0_CR |= OSC_CR_ERCLKEN_MASK | OSC_CR_EREFSTEN_MASK ;
            uart0_clk_khz = (CLK0_FREQ_HZ / 1000); // UART0 clock frequency will equal the OSCERCLK frequency
            //uart0_clk_khz = ((mcg_clk_hz / 2) / 1000); // UART0 clock frequency will equal half the PLL frequency
            uart0_test_init (UART0_BASE_PTR, uart0_clk_khz, baud_rate[5]);
            
            
            UART0_C2 |= UART0_C2_RIE_MASK;  // enable receive interrupt
            enable_irq (UART0SE_irq_no);
            if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 4){
    	          printf("  in VLPR Mode !  ");
            } else if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 1){
    	          printf("  in Run Mode  !  ");
            }
            op_mode = what_mcg_mode();
            if (op_mode ==1) 
                printf("in BLPI mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==2) 
                printf(" in FBI mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==3) 
                printf(" in FEI mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==4) 
                printf(" in FEE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==5) 
                printf(" in FBE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==6) 
                printf(" in BLPE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==7) 
                printf(" in PBE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==8) 
                printf(" in PEE mode now at %d Hz\r\n",mcg_clk_hz );
            printf("  Receiving Char from VLPS enabled OSCERCLK at 230400 baud \n\r  ");
            printf(" Hit a CTRL L to exit this loop \r\n");
            
            while(receive_char != 0x0c)  // hit CTRL L to exit
            {
               time_delay_ms(1); 
               enter_vlps();
               if (error_flag ==  PARITY_ERROR_Flag)
                 printf("PARITY_ERROR_Fla\n\n\r");
               if (error_flag ==  FRAMING_ERROR_Flag)
                 printf("FRAMING_ERROR_Flag\n\n\r");
               if (error_flag ==  NOISE_ERROR_Flag)
                 printf("NOISE_ERROR_Flag\n\n\r");
               if (error_flag ==  RX_OVERRUN_Flag)
                 printf("RX_OVERRUN_Flag\n\n\r");    
               
               uart0_putchar(UART0_BASE_PTR, receive_char); //echo to transmitter
               op_mode = what_mcg_mode();
               if(op_mode==PBE)
                      {
                           mcg_clk_hz = pbe_pee(CLK0_FREQ_HZ);
                      } 
               uart_putchar(UART1_BASE_PTR, receive_char);  // echo to terminal
                   
            }  
            break;
        case VLPS_EN_OSCERCLK_460800:
            //SIM_SOPT2 |= SIM_SOPT2_UART0SRC(1); // select the PLLFLLCLK as UART0 clock source
            // 10 OSCERCLK clock
            SIM_SOPT2 &= ~SIM_SOPT2_UART0SRC_MASK;
            SIM_SOPT2 |= SIM_SOPT2_UART0SRC(2); // select the OSCERCLK as UART0 clock source
            //SIM_SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK; // set PLLFLLSEL to select the PLL for this clock source       
            OSC0_CR |= OSC_CR_ERCLKEN_MASK | OSC_CR_EREFSTEN_MASK ;
            uart0_clk_khz = (CLK0_FREQ_HZ / 1000); // UART0 clock frequency will equal the OSCERCLK frequency
            //uart0_clk_khz = ((mcg_clk_hz / 2) / 1000); // UART0 clock frequency will equal half the PLL frequency
            uart0_test_init (UART0_BASE_PTR, uart0_clk_khz, baud_rate[6]);
            
            
            UART0_C2 |= UART0_C2_RIE_MASK;  // enable receive interrupt
            enable_irq (UART0SE_irq_no);
            if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 4){
    	          printf("  in VLPR Mode !  ");
            } else if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 1){
    	          printf("  in Run Mode  !  ");
            }
            op_mode = what_mcg_mode();
            if (op_mode ==1) 
                printf("in BLPI mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==2) 
                printf(" in FBI mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==3) 
                printf(" in FEI mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==4) 
                printf(" in FEE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==5) 
                printf(" in FBE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==6) 
                printf(" in BLPE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==7) 
                printf(" in PBE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==8) 
                printf(" in PEE mode now at %d Hz\r\n",mcg_clk_hz );
            printf("  Receiving Char from VLPS enabled OSCERCLK at 460800 baud \n\r  ");
            printf(" Hit a CTRL L to exit this loop \r\n");
            
            while(receive_char != 0x0c)  // hit CTRL L to exit
            {
               time_delay_ms(1); 
               enter_vlps();
               if (error_flag ==  PARITY_ERROR_Flag)
                 printf("PARITY_ERROR_Fla\n\n\r");
               if (error_flag ==  FRAMING_ERROR_Flag)
                 printf("FRAMING_ERROR_Flag\n\n\r");
               if (error_flag ==  NOISE_ERROR_Flag)
                 printf("NOISE_ERROR_Flag\n\n\r");
               if (error_flag ==  RX_OVERRUN_Flag)
                 printf("RX_OVERRUN_Flag\n\n\r");    
               
               uart0_putchar(UART0_BASE_PTR, receive_char); //echo to transmitter
               op_mode = what_mcg_mode();
               if(op_mode==PBE)
                      {
                           mcg_clk_hz = pbe_pee(CLK0_FREQ_HZ);
                      } 
               uart_putchar(UART1_BASE_PTR, receive_char);  // echo to terminal
                   
            }  
            break;
        case VLPS_EN_OSCERCLK_1000000:
            //SIM_SOPT2 |= SIM_SOPT2_UART0SRC(1); // select the PLLFLLCLK as UART0 clock source
            // 10 OSCERCLK clock
            SIM_SOPT2 &= ~SIM_SOPT2_UART0SRC_MASK;
            SIM_SOPT2 |= SIM_SOPT2_UART0SRC(2); // select the OSCERCLK as UART0 clock source
            //SIM_SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK; // set PLLFLLSEL to select the PLL for this clock source       
            OSC0_CR |= OSC_CR_ERCLKEN_MASK | OSC_CR_EREFSTEN_MASK ;
            uart0_clk_khz = (CLK0_FREQ_HZ / 1000); // UART0 clock frequency will equal the OSCERCLK frequency
            //uart0_clk_khz = ((mcg_clk_hz / 2) / 1000); // UART0 clock frequency will equal half the PLL frequency
            uart0_test_init (UART0_BASE_PTR, uart0_clk_khz, baud_rate[7]);
            
           
            UART0_C2 |= UART0_C2_RIE_MASK;  // enable receive interrupt
            enable_irq (UART0SE_irq_no);
            if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 4){
    	          printf("  in VLPR Mode !  ");
            } else if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 1){
    	          printf("  in Run Mode  !  ");
            }
            op_mode = what_mcg_mode();
            if (op_mode ==1) 
                printf("in BLPI mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==2) 
                printf(" in FBI mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==3) 
                printf(" in FEI mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==4) 
                printf(" in FEE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==5) 
                printf(" in FBE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==6) 
                printf(" in BLPE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==7) 
                printf(" in PBE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==8) 
                printf(" in PEE mode now at %d Hz\r\n",mcg_clk_hz );
            printf("  Receiving Char from VLPS Enabled OSCERCLK at 1000000 baud \n\r  ");
            printf(" Hit a CTRL L to exit this loop \r\n");
            
            while(receive_char != 0x0c)  // hit CTRL L to exit
            {
               time_delay_ms(1); 
               enter_vlps();
               if (error_flag ==  PARITY_ERROR_Flag)
                 printf("PARITY_ERROR_Fla\n\n\r");
               if (error_flag ==  FRAMING_ERROR_Flag)
                 printf("FRAMING_ERROR_Flag\n\n\r");
               if (error_flag ==  NOISE_ERROR_Flag)
                 printf("NOISE_ERROR_Flag\n\n\r");
               if (error_flag ==  RX_OVERRUN_Flag)
                 printf("RX_OVERRUN_Flag\n\n\r");    
               
               uart0_putchar(UART0_BASE_PTR, receive_char); //echo to transmitter
               op_mode = what_mcg_mode();
               if(op_mode==PBE)
                      {
                           mcg_clk_hz = pbe_pee(CLK0_FREQ_HZ);
                      } 
               uart_putchar(UART1_BASE_PTR, receive_char);  // echo to terminal
                   
            }  
            break;
        case VLPS_EN_OSCERCLK_2000000:
            //SIM_SOPT2 |= SIM_SOPT2_UART0SRC(1); // select the PLLFLLCLK as UART0 clock source
            // 10 OSCERCLK clock
            SIM_SOPT2 &= ~SIM_SOPT2_UART0SRC_MASK;
            SIM_SOPT2 |= SIM_SOPT2_UART0SRC(2); // select the OSCERCLK as UART0 clock source
            //SIM_SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK; // set PLLFLLSEL to select the PLL for this clock source       
            OSC0_CR |= OSC_CR_ERCLKEN_MASK | OSC_CR_EREFSTEN_MASK ;
            uart0_clk_khz = (CLK0_FREQ_HZ / 1000); // UART0 clock frequency will equal the OSCERCLK frequency
            //uart0_clk_khz = ((mcg_clk_hz / 2) / 1000); // UART0 clock frequency will equal half the PLL frequency
            uart0_test_init (UART0_BASE_PTR, uart0_clk_khz, baud_rate[8]);
            
           
            UART0_C2 |= UART0_C2_RIE_MASK;  // enable receive interrupt
            enable_irq (UART0SE_irq_no);
            if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 4){
    	          printf("  in VLPR Mode !  ");
            } else if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 1){
    	          printf("  in Run Mode  !  ");
            }
            op_mode = what_mcg_mode();
            if (op_mode ==1) 
                printf("in BLPI mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==2) 
                printf(" in FBI mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==3) 
                printf(" in FEI mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==4) 
                printf(" in FEE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==5) 
                printf(" in FBE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==6) 
                printf(" in BLPE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==7) 
                printf(" in PBE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==8) 
                printf(" in PEE mode now at %d Hz\r\n",mcg_clk_hz );
            printf("  Receiving Char from VLPS Enabled OSCERCLK at 2000000 baud \n\r  ");
            printf(" Hit a CTRL L to exit this loop \r\n");
            
            while(receive_char != 0x0c)  // hit CTRL L to exit
            {
               time_delay_ms(1); 
               enter_vlps();
               if (error_flag ==  PARITY_ERROR_Flag)
                 printf("PARITY_ERROR_Fla\n\n\r");
               if (error_flag ==  FRAMING_ERROR_Flag)
                 printf("FRAMING_ERROR_Flag\n\n\r");
               if (error_flag ==  NOISE_ERROR_Flag)
                 printf("NOISE_ERROR_Flag\n\n\r");
               if (error_flag ==  RX_OVERRUN_Flag)
                 printf("RX_OVERRUN_Flag\n\n\r");    
               
               uart0_putchar(UART0_BASE_PTR, receive_char); //echo to transmitter
               op_mode = what_mcg_mode();
               if(op_mode==PBE)
                      {
                           mcg_clk_hz = pbe_pee(CLK0_FREQ_HZ);
                      } 
               uart_putchar(UART1_BASE_PTR, receive_char);  // echo to terminal
                   
            }  
            break;
        case VLPS_EN_FASTIRC_CLK_4800:         
            // 10 OSCERCLK clock
            //SIM_SOPT2 |= SIM_SOPT2_UART0SRC(2); // select the OSCERCLK as UART0 clock source
            // 11 IRCLK     
            SIM_SOPT2 &= ~SIM_SOPT2_UART0SRC_MASK;
            SIM_SOPT2 |= SIM_SOPT2_UART0SRC(3); // select the IRCLK as UART0 clock source
            // enable the IRC in run and stop modes 
            MCG_C1 |= MCG_C1_IRCLKEN_MASK | MCG_C1_IREFSTEN_MASK; 
            // select the FAST (4M IRC)
            MCG_C2 |= MCG_C2_IRCS_MASK;
            //SIM_SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK; // set PLLFLLSEL to select the PLL for this clock source       
            MCG_SC &= ~MCG_SC_FCRDIV_MASK; // SET TO DIVID BY 1 FOR fAST IRC
            //OSC0_CR |= OSC_CR_ERCLKEN_MASK | OSC_CR_EREFSTEN_MASK ;
            //uart0_clk_khz = (CLK0_FREQ_HZ / 1000); // UART0 clock frequency will equal the OSCERCLK frequency
            uart0_clk_khz = (FAST_IRC_FREQ  / 1000); // UART0 clock frequency will equal half the PLL frequency
            //uart0_clk_khz = ((mcg_clk_hz / 2) / 1000); // UART0 clock frequency will equal half the PLL frequency
            uart0_test_init (UART0_BASE_PTR, uart0_clk_khz, baud_rate[0]);
            // enable Framming Overrun and NOISE error flag interrups 
            UART0_C3 |= UART0_C3_NEIE_MASK | UART0_C3_ORIE_MASK  | UART0_C3_FEIE_MASK; 
            UART0_C2 |= UART0_C2_RIE_MASK;  // enable receive interrupt
            enable_irq (UART0SE_irq_no);
            
            if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 4){
    	          printf("  in VLPR Mode !  ");
            } else if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 1){
    	          printf("  in Run Mode  !  ");
            }
            op_mode = what_mcg_mode();
            if (op_mode ==1) 
                printf("in BLPI mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==2) 
                printf(" in FBI mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==3) 
                printf(" in FEI mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==4) 
                printf(" in FEE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==5) 
                printf(" in FBE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==6) 
                printf(" in BLPE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==7) 
                printf(" in PBE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==8) 
                printf(" in PEE mode now at %d Hz\r\n",mcg_clk_hz );
            printf("  Receiving Char from VLPS Enabled Fast IRC at 4800 baud \n\r  ");
            printf(" Hit a CTRL L to exit this loop \r\n");
            clk_monitor_0(OFF);
            while(receive_char != 0x0c)  // hit CTRL L to exit
            {
               time_delay_ms(1); 
               //while (((UART0_S1 & UART0_S1_RDRF_MASK)>>UART0_S1_RDRF_SHIFT)==0);
               enter_vlps();
               if (error_flag ==  PARITY_ERROR_Flag)
                 printf("PARITY_ERROR_Fla\n\n\r");
               if (error_flag ==  FRAMING_ERROR_Flag)
                 printf("FRAMING_ERROR_Flag\n\n\r");
               if (error_flag ==  NOISE_ERROR_Flag)
                 printf("NOISE_ERROR_Flag\n\n\r");
               if (error_flag ==  RX_OVERRUN_Flag)
                 printf("RX_OVERRUN_Flag\n\n\r");    
               
               uart0_putchar(UART0_BASE_PTR, receive_char); //echo to transmitter
               op_mode = what_mcg_mode();
               if(op_mode==PBE)
                      {
                           mcg_clk_hz = pbe_pee(CLK0_FREQ_HZ);
                      } 
               uart_putchar(UART1_BASE_PTR, receive_char);  // echo to terminal
                   
            }  
            clk_monitor_0(ON);

            break;
        case VLPS_EN_FASTIRC_CLK_9600:
            // 10 OSCERCLK clock
            //SIM_SOPT2 |= SIM_SOPT2_UART0SRC(2); // select the OSCERCLK as UART0 clock source
            // 11 IRCLK    
            SIM_SOPT2 &= ~SIM_SOPT2_UART0SRC_MASK;
            SIM_SOPT2 |= SIM_SOPT2_UART0SRC(3); // select the IRCLK as UART0 clock source
            // enable the IRC in run and stop modes 
            MCG_C1 |= MCG_C1_IRCLKEN_MASK | MCG_C1_IREFSTEN_MASK; 
            // select the FAST (4M IRC)
            MCG_C2 |= MCG_C2_IRCS_MASK;
            //SIM_SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK; // set PLLFLLSEL to select the PLL for this clock source       
            MCG_SC &= ~MCG_SC_FCRDIV_MASK; // SET TO DIVID BY 1 FOR fAST IRC
            //OSC0_CR |= OSC_CR_ERCLKEN_MASK | OSC_CR_EREFSTEN_MASK ;
            uart0_clk_khz = (CLK0_FREQ_HZ / 1000); // UART0 clock frequency will equal the OSCERCLK frequency
            uart0_clk_khz = (FAST_IRC_FREQ  / 1000); // UART0 clock frequency will equal half the PLL frequency
            //uart0_clk_khz = ((mcg_clk_hz / 2) / 1000); // UART0 clock frequency will equal half the PLL frequency
            uart0_test_init (UART0_BASE_PTR, uart0_clk_khz, baud_rate[1]);
            // enable Framming Overrun and NOISE error flag interrups 
            UART0_C3 |= UART0_C3_NEIE_MASK | UART0_C3_ORIE_MASK  | UART0_C3_FEIE_MASK; 
            UART0_C2 |= UART0_C2_RIE_MASK;  // enable receive interrupt
            enable_irq (UART0SE_irq_no);
            
            if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 4){
    	          printf("  in VLPR Mode !  ");
            } else if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 1){
    	          printf("  in Run Mode  !  ");
            }
            op_mode = what_mcg_mode();
            if (op_mode ==1) 
                printf("in BLPI mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==2) 
                printf(" in FBI mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==3) 
                printf(" in FEI mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==4) 
                printf(" in FEE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==5) 
                printf(" in FBE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==6) 
                printf(" in BLPE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==7) 
                printf(" in PBE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==8) 
                printf(" in PEE mode now at %d Hz\r\n",mcg_clk_hz );
            printf("  Receiving Char from VLPS Enabled Fast IRC at 9600 baud \n\r  ");
            printf(" Hit a CTRL L to exit this loop \r\n");
            clk_monitor_0(OFF);            
            while(receive_char != 0x0c)  // hit CTRL L to exit
            {
               time_delay_ms(1); 
               //while (((UART0_S1 & UART0_S1_RDRF_MASK)>>UART0_S1_RDRF_SHIFT)==0);
               enter_vlps();
               if (error_flag ==  PARITY_ERROR_Flag)
                 printf("PARITY_ERROR_Fla\n\n\r");
               if (error_flag ==  FRAMING_ERROR_Flag)
                 printf("FRAMING_ERROR_Flag\n\n\r");
               if (error_flag ==  NOISE_ERROR_Flag)
                 printf("NOISE_ERROR_Flag\n\n\r");
               if (error_flag ==  RX_OVERRUN_Flag)
                 printf("RX_OVERRUN_Flag\n\n\r");    
               
               uart0_putchar(UART0_BASE_PTR, receive_char); //echo to transmitter
               op_mode = what_mcg_mode();
               if(op_mode==PBE)
                      {
                           mcg_clk_hz = pbe_pee(CLK0_FREQ_HZ);
                      } 
               uart_putchar(UART1_BASE_PTR, receive_char);  // echo to terminal
                   
            }  
            clk_monitor_0(ON);
            break;
        case VLPS_EN_FASTIRC_CLK_19200:
            // 10 OSCERCLK clock
            //SIM_SOPT2 |= SIM_SOPT2_UART0SRC(2); // select the OSCERCLK as UART0 clock source
            // 11 IRCLK     
            SIM_SOPT2 &= ~SIM_SOPT2_UART0SRC_MASK;
            SIM_SOPT2 |= SIM_SOPT2_UART0SRC(3); // select the IRCLK as UART0 clock source
            // enable the IRC in run and stop modes 
            MCG_C1 |= MCG_C1_IRCLKEN_MASK | MCG_C1_IREFSTEN_MASK; 
            // select the FAST (4M IRC)
            MCG_C2 |= MCG_C2_IRCS_MASK;
            //SIM_SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK; // set PLLFLLSEL to select the PLL for this clock source       
            MCG_SC &= ~MCG_SC_FCRDIV_MASK; // SET TO DIVID BY 1 FOR fAST IRC
            //OSC0_CR |= OSC_CR_ERCLKEN_MASK | OSC_CR_EREFSTEN_MASK ;
            uart0_clk_khz = (CLK0_FREQ_HZ / 1000); // UART0 clock frequency will equal the OSCERCLK frequency
            uart0_clk_khz = (FAST_IRC_FREQ  / 1000); // UART0 clock frequency will equal half the PLL frequency
            //uart0_clk_khz = ((mcg_clk_hz / 2) / 1000); // UART0 clock frequency will equal half the PLL frequency
            uart0_test_init (UART0_BASE_PTR, uart0_clk_khz, baud_rate[2]);
            // enable Framming Overrun and NOISE error flag interrups 
            UART0_C3 |= UART0_C3_NEIE_MASK | UART0_C3_ORIE_MASK  | UART0_C3_FEIE_MASK; 
            UART0_C2 |= UART0_C2_RIE_MASK;  // enable receive interrupt
            enable_irq (UART0SE_irq_no);
            
            if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 4){
    	          printf("  in VLPR Mode !  ");
            } else if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 1){
    	          printf("  in Run Mode  !  ");
            }
            op_mode = what_mcg_mode();
            if (op_mode ==1) 
                printf("in BLPI mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==2) 
                printf(" in FBI mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==3) 
                printf(" in FEI mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==4) 
                printf(" in FEE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==5) 
                printf(" in FBE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==6) 
                printf(" in BLPE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==7) 
                printf(" in PBE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==8) 
                printf(" in PEE mode now at %d Hz\r\n",mcg_clk_hz );
            printf("  Receiving Char from VLPS Enabled Fast IRC at 19200 baud \n\r  ");
            printf(" Hit a CTRL L to exit this loop \r\n");
            clk_monitor_0(OFF);
            while(receive_char != 0x0c)  // hit CTRL L to exit
            {
               time_delay_ms(1); 
               //while (((UART0_S1 & UART0_S1_RDRF_MASK)>>UART0_S1_RDRF_SHIFT)==0);
               enter_vlps();
               if (error_flag ==  PARITY_ERROR_Flag)
                 printf("PARITY_ERROR_Fla\n\n\r");
               if (error_flag ==  FRAMING_ERROR_Flag)
                 printf("FRAMING_ERROR_Flag\n\n\r");
               if (error_flag ==  NOISE_ERROR_Flag)
                 printf("NOISE_ERROR_Flag\n\n\r");
               if (error_flag ==  RX_OVERRUN_Flag)
                 printf("RX_OVERRUN_Flag\n\n\r");    
               
               uart0_putchar(UART0_BASE_PTR, receive_char); //echo to transmitter
               op_mode = what_mcg_mode();
               if(op_mode==PBE)
                      {
                           mcg_clk_hz = pbe_pee(CLK0_FREQ_HZ);
                      } 
               uart_putchar(UART1_BASE_PTR, receive_char);  // echo to terminal
                   
            }  
            clk_monitor_0(ON);
            break;
        case VLPS_EN_FASTIRC_CLK_57600:
            // 10 OSCERCLK clock
            //SIM_SOPT2 |= SIM_SOPT2_UART0SRC(2); // select the OSCERCLK as UART0 clock source
            // 11 IRCLK     
            SIM_SOPT2 &= ~SIM_SOPT2_UART0SRC_MASK;
            SIM_SOPT2 |= SIM_SOPT2_UART0SRC(3); // select the IRCLK as UART0 clock source
            // enable the IRC in run and stop modes 
            MCG_C1 |= MCG_C1_IRCLKEN_MASK | MCG_C1_IREFSTEN_MASK; 
            // select the FAST (4M IRC)
            MCG_C2 |= MCG_C2_IRCS_MASK;
            //SIM_SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK; // set PLLFLLSEL to select the PLL for this clock source       
            MCG_SC &= ~MCG_SC_FCRDIV_MASK; // SET TO DIVID BY 1 FOR fAST IRC
            //OSC0_CR |= OSC_CR_ERCLKEN_MASK | OSC_CR_EREFSTEN_MASK ;
            uart0_clk_khz = (CLK0_FREQ_HZ / 1000); // UART0 clock frequency will equal the OSCERCLK frequency
            uart0_clk_khz = (FAST_IRC_FREQ  / 1000); // UART0 clock frequency will equal half the PLL frequency
            //uart0_clk_khz = ((mcg_clk_hz / 2) / 1000); // UART0 clock frequency will equal half the PLL frequency
            uart0_test_init (UART0_BASE_PTR, uart0_clk_khz, baud_rate[3]);
            // enable Framming Overrun and NOISE error flag interrups 
            UART0_C3 |= UART0_C3_NEIE_MASK | UART0_C3_ORIE_MASK  | UART0_C3_FEIE_MASK; 
            UART0_C2 |= UART0_C2_RIE_MASK;  // enable receive interrupt
            enable_irq (UART0SE_irq_no);
            
            if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 4){
    	          printf("  in VLPR Mode !  ");
            } else if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 1){
    	          printf("  in Run Mode  !  ");
            }
            op_mode = what_mcg_mode();
            if (op_mode ==1) 
                printf("in BLPI mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==2) 
                printf(" in FBI mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==3) 
                printf(" in FEI mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==4) 
                printf(" in FEE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==5) 
                printf(" in FBE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==6) 
                printf(" in BLPE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==7) 
                printf(" in PBE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==8) 
                printf(" in PEE mode now at %d Hz\r\n",mcg_clk_hz );
            printf("  Receiving Char from VLPS Enabled Fast IRC at 57600 baud \n\r  ");
            printf(" Hit a CTRL L to exit this loop \r\n");
            clk_monitor_0(OFF);
            
            while(receive_char != 0x0c)  // hit CTRL L to exit
            {
               time_delay_ms(1); 
               //while (((UART0_S1 & UART0_S1_RDRF_MASK)>>UART0_S1_RDRF_SHIFT)==0);
               enter_vlps();
               if (error_flag ==  PARITY_ERROR_Flag)
                 printf("PARITY_ERROR_Fla\n\n\r");
               if (error_flag ==  FRAMING_ERROR_Flag)
                 printf("FRAMING_ERROR_Flag\n\n\r");
               if (error_flag ==  NOISE_ERROR_Flag)
                 printf("NOISE_ERROR_Flag\n\n\r");
               if (error_flag ==  RX_OVERRUN_Flag)
                 printf("RX_OVERRUN_Flag\n\n\r");    
               
               uart0_putchar(UART0_BASE_PTR, receive_char); //echo to transmitter
               op_mode = what_mcg_mode();
               if(op_mode==PBE)
                      {
                           mcg_clk_hz = pbe_pee(CLK0_FREQ_HZ);
                      } 
               uart_putchar(UART1_BASE_PTR, receive_char);  // echo to terminal
                   
            }  
            clk_monitor_0(ON);
            break;
        case VLPS_EN_FASTIRC_CLK_115200:
            // 10 OSCERCLK clock
            //SIM_SOPT2 |= SIM_SOPT2_UART0SRC(2); // select the OSCERCLK as UART0 clock source
            // 11 IRCLK  
            SIM_SOPT2 &= ~SIM_SOPT2_UART0SRC_MASK;
            SIM_SOPT2 |= SIM_SOPT2_UART0SRC(3); // select the IRCLK as UART0 clock source
            // enable the IRC in run and stop modes 
            MCG_C1 |= MCG_C1_IRCLKEN_MASK | MCG_C1_IREFSTEN_MASK; 
            // select the FAST (4M IRC)
            MCG_C2 |= MCG_C2_IRCS_MASK;
            //SIM_SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK; // set PLLFLLSEL to select the PLL for this clock source       
            MCG_SC &= ~MCG_SC_FCRDIV_MASK; // SET TO DIVID BY 1 FOR fAST IRC
            //OSC0_CR |= OSC_CR_ERCLKEN_MASK | OSC_CR_EREFSTEN_MASK ;
            uart0_clk_khz = (CLK0_FREQ_HZ / 1000); // UART0 clock frequency will equal the OSCERCLK frequency
            uart0_clk_khz = (FAST_IRC_FREQ  / 1000); // UART0 clock frequency will equal half the PLL frequency
            //uart0_clk_khz = ((mcg_clk_hz / 2) / 1000); // UART0 clock frequency will equal half the PLL frequency
            uart0_test_init (UART0_BASE_PTR, uart0_clk_khz, baud_rate[4]);
            // enable Framming Overrun and NOISE error flag interrups 
            UART0_C3 |= UART0_C3_NEIE_MASK | UART0_C3_ORIE_MASK  | UART0_C3_FEIE_MASK; 
            UART0_C2 |= UART0_C2_RIE_MASK;  // enable receive interrupt
            enable_irq (UART0SE_irq_no);
            
            if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 4){
    	          printf("  in VLPR Mode !  ");
            } else if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 1){
    	          printf("  in Run Mode  !  ");
            }
            op_mode = what_mcg_mode();
            if (op_mode ==1) 
                printf("in BLPI mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==2) 
                printf(" in FBI mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==3) 
                printf(" in FEI mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==4) 
                printf(" in FEE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==5) 
                printf(" in FBE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==6) 
                printf(" in BLPE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==7) 
                printf(" in PBE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==8) 
                printf(" in PEE mode now at %d Hz\r\n",mcg_clk_hz );
            printf("  Receiving Char from VLPS Enabled Fast IRC  at 115200 baud \n\r  ");
            printf(" Hit a CTRL L to exit this loop \r\n");
            clk_monitor_0(OFF);
            
            while(receive_char != 0x0c)  // hit CTRL L to exit
            {
               time_delay_ms(1); 
               //while (((UART0_S1 & UART0_S1_RDRF_MASK)>>UART0_S1_RDRF_SHIFT)==0);
               enter_vlps();
               if (error_flag ==  PARITY_ERROR_Flag)
                 printf("PARITY_ERROR_Fla\n\n\r");
               if (error_flag ==  FRAMING_ERROR_Flag)
                 printf("FRAMING_ERROR_Flag\n\n\r");
               if (error_flag ==  NOISE_ERROR_Flag)
                 printf("NOISE_ERROR_Flag\n\n\r");
               if (error_flag ==  RX_OVERRUN_Flag)
                 printf("RX_OVERRUN_Flag\n\n\r");    
               
               uart0_putchar(UART0_BASE_PTR, receive_char); //echo to transmitter
               op_mode = what_mcg_mode();
               if(op_mode==PBE)
                      {
                           mcg_clk_hz = pbe_pee(CLK0_FREQ_HZ);
                      } 
               uart_putchar(UART1_BASE_PTR, receive_char);  // echo to terminal
                   
            }  
            clk_monitor_0(ON);
            break;
        case VLPS_EN_FASTIRC_CLK_230400:
            // 10 OSCERCLK clock
            //SIM_SOPT2 |= SIM_SOPT2_UART0SRC(2); // select the OSCERCLK as UART0 clock source
            // 11 IRCLK     
            SIM_SOPT2 &= ~SIM_SOPT2_UART0SRC_MASK;
            SIM_SOPT2 |= SIM_SOPT2_UART0SRC(3); // select the IRCLK as UART0 clock source
            // enable the IRC in run and stop modes 
            MCG_C1 |= MCG_C1_IRCLKEN_MASK | MCG_C1_IREFSTEN_MASK; 
            // select the FAST (4M IRC)
            MCG_C2 |= MCG_C2_IRCS_MASK;
            //SIM_SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK; // set PLLFLLSEL to select the PLL for this clock source       
            MCG_SC &= ~MCG_SC_FCRDIV_MASK; // SET TO DIVID BY 1 FOR fAST IRC
            //OSC0_CR |= OSC_CR_ERCLKEN_MASK | OSC_CR_EREFSTEN_MASK ;
            //uart0_clk_khz = (CLK0_FREQ_HZ / 1000); // UART0 clock frequency will equal the OSCERCLK frequency
            uart0_clk_khz = (FAST_IRC_FREQ  / 1000); // UART0 clock frequency will equal half the PLL frequency
            //uart0_clk_khz = ((mcg_clk_hz / 2) / 1000); // UART0 clock frequency will equal half the PLL frequency
            uart0_test_init (UART0_BASE_PTR, uart0_clk_khz, baud_rate[5]);
            // enable Framming Overrun and NOISE error flag interrups 
            UART0_C3 |= UART0_C3_NEIE_MASK | UART0_C3_ORIE_MASK  | UART0_C3_FEIE_MASK; 
            UART0_C2 |= UART0_C2_RIE_MASK;  // enable receive interrupt
            enable_irq (UART0SE_irq_no);
            
            if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 4){
    	          printf("  in VLPR Mode !  ");
            } else if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 1){
    	          printf("  in Run Mode  !  ");
            }
            op_mode = what_mcg_mode();
            if (op_mode ==1) 
                printf("in BLPI mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==2) 
                printf(" in FBI mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==3) 
                printf(" in FEI mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==4) 
                printf(" in FEE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==5) 
                printf(" in FBE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==6) 
                printf(" in BLPE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==7) 
                printf(" in PBE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==8) 
                printf(" in PEE mode now at %d Hz\r\n",mcg_clk_hz );
            printf("  Receiving Char from VLPS Enabled Fast IRC at 230600 baud \n\r  ");
            printf(" Hit a CTRL L to exit this loop \r\n");
            clk_monitor_0(OFF);
            
            while(receive_char != 0x0c)  // hit CTRL L to exit
            {
               time_delay_ms(1); 
               //while (((UART0_S1 & UART0_S1_RDRF_MASK)>>UART0_S1_RDRF_SHIFT)==0);
               enter_vlps();
               if (error_flag ==  PARITY_ERROR_Flag)
                 printf("PARITY_ERROR_Fla\n\n\r");
               if (error_flag ==  FRAMING_ERROR_Flag)
                 printf("FRAMING_ERROR_Flag\n\n\r");
               if (error_flag ==  NOISE_ERROR_Flag)
                 printf("NOISE_ERROR_Flag\n\n\r");
               if (error_flag ==  RX_OVERRUN_Flag)
                 printf("RX_OVERRUN_Flag\n\n\r");    
               
               uart0_putchar(UART0_BASE_PTR, receive_char); //echo to transmitter
               op_mode = what_mcg_mode();
               if(op_mode==PBE)
                      {
                           mcg_clk_hz = pbe_pee(CLK0_FREQ_HZ);
                      } 
               uart_putchar(UART1_BASE_PTR, receive_char);  // echo to terminal
                   
            }  
            clk_monitor_0(ON);
            break;
        case VLPS_EN_FASTIRC_CLK_500000:
            // 10 OSCERCLK clock
            //SIM_SOPT2 |= SIM_SOPT2_UART0SRC(2); // select the OSCERCLK as UART0 clock source
            // 11 IRCLK     
            SIM_SOPT2 &= ~SIM_SOPT2_UART0SRC_MASK;
            SIM_SOPT2 |= SIM_SOPT2_UART0SRC(3); // select the IRCLK as UART0 clock source
            // enable the IRC in run and stop modes 
            MCG_C1 |= MCG_C1_IRCLKEN_MASK | MCG_C1_IREFSTEN_MASK; 
            // select the FAST (4M IRC)
            MCG_C2 |= MCG_C2_IRCS_MASK;
            //SIM_SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK; // set PLLFLLSEL to select the PLL for this clock source       
            MCG_SC &= ~MCG_SC_FCRDIV_MASK; // SET TO DIVID BY 1 FOR fAST IRC
            //OSC0_CR |= OSC_CR_ERCLKEN_MASK | OSC_CR_EREFSTEN_MASK ;
            //uart0_clk_khz = (CLK0_FREQ_HZ / 1000); // UART0 clock frequency will equal the OSCERCLK frequency
            uart0_clk_khz = (FAST_IRC_FREQ  / 1000); // UART0 clock frequency will equal half the PLL frequency
            //uart0_clk_khz = ((mcg_clk_hz / 2) / 1000); // UART0 clock frequency will equal half the PLL frequency
            uart0_test_init (UART0_BASE_PTR, uart0_clk_khz, 500000);
            // enable Framming Overrun and NOISE error flag interrups 
            UART0_C3 |= UART0_C3_NEIE_MASK | UART0_C3_ORIE_MASK  | UART0_C3_FEIE_MASK; 
            UART0_C2 |= UART0_C2_RIE_MASK;  // enable receive interrupt
            enable_irq (UART0SE_irq_no);
            
            if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 4){
    	          printf("  in VLPR Mode !  ");
            } else if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 1){
    	          printf("  in Run Mode  !  ");
            }
            op_mode = what_mcg_mode();
            if (op_mode ==1) 
                printf("in BLPI mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==2) 
                printf(" in FBI mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==3) 
                printf(" in FEI mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==4) 
                printf(" in FEE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==5) 
                printf(" in FBE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==6) 
                printf(" in BLPE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==7) 
                printf(" in PBE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==8) 
                printf(" in PEE mode now at %d Hz\r\n",mcg_clk_hz );
            printf("  Receiving Char from VLPS Enabled Fast IRC at 500000 baud \n\r  ");
            printf(" Hit a CTRL L to exit this loop \r\n");
            clk_monitor_0(OFF);
            
            while(receive_char != 0x0c)  // hit CTRL L to exit
            {
               time_delay_ms(1); 
               //while (((UART0_S1 & UART0_S1_RDRF_MASK)>>UART0_S1_RDRF_SHIFT)==0);
               enter_vlps();
               if (error_flag ==  PARITY_ERROR_Flag)
                 printf("PARITY_ERROR_Fla\n\n\r");
               if (error_flag ==  FRAMING_ERROR_Flag)
                 printf("FRAMING_ERROR_Flag\n\n\r");
               if (error_flag ==  NOISE_ERROR_Flag)
                 printf("NOISE_ERROR_Flag\n\n\r");
               if (error_flag ==  RX_OVERRUN_Flag)
                 printf("RX_OVERRUN_Flag\n\n\r");    
               
               uart0_putchar(UART0_BASE_PTR, receive_char); //echo to transmitter
               op_mode = what_mcg_mode();
               if(op_mode==PBE)
                      {
                           mcg_clk_hz = pbe_pee(CLK0_FREQ_HZ);
                      } 
               uart_putchar(UART1_BASE_PTR, receive_char);  // echo to terminal
                   
            }  
            clk_monitor_0(ON);
            break;
        case VLPS_EN_FASTIRC_CLK_1000000:
            // 10 OSCERCLK clock
            //SIM_SOPT2 |= SIM_SOPT2_UART0SRC(2); // select the OSCERCLK as UART0 clock source
            // 11 IRCLK   
            SIM_SOPT2 &= ~SIM_SOPT2_UART0SRC_MASK;
            SIM_SOPT2 |= SIM_SOPT2_UART0SRC(3); // select the IRCLK as UART0 clock source
            // enable the IRC in run and stop modes 
            MCG_C1 |= MCG_C1_IRCLKEN_MASK | MCG_C1_IREFSTEN_MASK; 
            // select the FAST (4M IRC)
            MCG_C2 |= MCG_C2_IRCS_MASK;
            //SIM_SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK; // set PLLFLLSEL to select the PLL for this clock source       
            MCG_SC &= ~MCG_SC_FCRDIV_MASK; // SET TO DIVID BY 1 FOR fAST IRC
            //OSC0_CR |= OSC_CR_ERCLKEN_MASK | OSC_CR_EREFSTEN_MASK ;
            uart0_clk_khz = (CLK0_FREQ_HZ / 1000); // UART0 clock frequency will equal the OSCERCLK frequency
            uart0_clk_khz = (FAST_IRC_FREQ  / 1000); // UART0 clock frequency will equal half the PLL frequency
            //uart0_clk_khz = ((mcg_clk_hz / 2) / 1000); // UART0 clock frequency will equal half the PLL frequency
            uart0_test_init (UART0_BASE_PTR, uart0_clk_khz, baud_rate[7]);
            // enable Framming Overrun and NOISE error flag interrups 
            UART0_C3 |= UART0_C3_NEIE_MASK | UART0_C3_ORIE_MASK  | UART0_C3_FEIE_MASK; 
            UART0_C2 |= UART0_C2_RIE_MASK;  // enable receive interrupt
            enable_irq (UART0SE_irq_no);
            
            if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 4){
    	          printf("  in VLPR Mode !  ");
            } else if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 1){
    	          printf("  in Run Mode  !  ");
            }
            op_mode = what_mcg_mode();
            if (op_mode ==1) 
                printf("in BLPI mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==2) 
                printf(" in FBI mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==3) 
                printf(" in FEI mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==4) 
                printf(" in FEE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==5) 
                printf(" in FBE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==6) 
                printf(" in BLPE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==7) 
                printf(" in PBE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==8) 
                printf(" in PEE mode now at %d Hz\r\n",mcg_clk_hz );
            printf("  Receiving Char from VLPS Enabled Fast IRC at 1 Mega baud \n\r  ");
            printf(" Hit a CTRL L to exit this loop \r\n");
            clk_monitor_0(OFF);
            
            while(receive_char != 0x0c)  // hit CTRL L to exit
            {
               time_delay_ms(1); 
               //while (((UART0_S1 & UART0_S1_RDRF_MASK)>>UART0_S1_RDRF_SHIFT)==0);
               enter_vlps();
               if (error_flag ==  PARITY_ERROR_Flag)
                 printf("PARITY_ERROR_Fla\n\n\r");
               if (error_flag ==  FRAMING_ERROR_Flag)
                 printf("FRAMING_ERROR_Flag\n\n\r");
               if (error_flag ==  NOISE_ERROR_Flag)
                 printf("NOISE_ERROR_Flag\n\n\r");
               if (error_flag ==  RX_OVERRUN_Flag)
                 printf("RX_OVERRUN_Flag\n\n\r");    
               
               uart0_putchar(UART0_BASE_PTR, receive_char); //echo to transmitter
               op_mode = what_mcg_mode();
               if(op_mode==PBE)
                      {
                           mcg_clk_hz = pbe_pee(CLK0_FREQ_HZ);
                      } 
               uart_putchar(UART1_BASE_PTR, receive_char);  // echo to terminal
                   
            }  
            clk_monitor_0(ON);
            break;
        case VLPS_FEI_9600:
            // 01 is MCGFLLCLK
            SIM_SOPT2 &= ~SIM_SOPT2_UART0SRC_MASK;
            SIM_SOPT2 |= SIM_SOPT2_UART0SRC(1); // select the MCGFLL clock as UART0 source
            // move to FEI mode
            mcg_clk_hz = pee_pbe(CLK0_FREQ_HZ);
            mcg_clk_hz = pbe_fbe(CLK0_FREQ_HZ);
            // set FLL freq to max frequency
            MCG_C4 &= ~MCG_C4_DRST_DRS_MASK;
            //MCG_C4 |= MCG_C4_DRST_DRS(1) ;  // DMX32=0 39.0625 kHz x1280 50 MHz
            MCG_C4 |= MCG_C4_DRST_DRS(1) | MCG_C4_DMX32_MASK;  // DMX32=1 32768 kHz x1280 48 MHz

            mcg_clk_hz = fbe_fei(SLOW_IRC_FREQ);
            

            SIM_SOPT2 &= ~SIM_SOPT2_PLLFLLSEL_MASK; // set PLLFLLSEL to select the FLL for this clock source       
            mcg_clk_khz = mcg_clk_hz / 1000;
  	    core_clk_khz = mcg_clk_khz / (((SIM_CLKDIV1 & SIM_CLKDIV1_OUTDIV1_MASK) >> 28)+ 1);
            periph_clk_khz = core_clk_khz / (((SIM_CLKDIV1 & SIM_CLKDIV1_OUTDIV4_MASK) >> 16)+ 1);

            uart0_clk_khz = ((mcg_clk_hz / 2) / 1000); // UART0 clock frequency will equal half the PLL frequency
            uart_init (UART1_BASE_PTR, periph_clk_khz, TERMINAL_BAUD);
            uart0_test_init (UART0_BASE_PTR, uart0_clk_khz, baud_rate[1]);
            UART0_BDH |= UART0_BDH_RXEDGIE_MASK;
            // enable Framming Overrun and NOISE error flag interrups 
            UART0_C3 |= UART0_C3_NEIE_MASK | UART0_C3_ORIE_MASK  | UART0_C3_FEIE_MASK; 
            UART0_C2 |= UART0_C2_RIE_MASK;  // enable receive interrupt
            //clear all interrupt flags
            UART0_S1 |= UART0_S1_PF_MASK |
                        UART0_S1_FE_MASK |
                        UART0_S1_NF_MASK |
                        UART0_S1_OR_MASK  ;  // write 1 to clear error flags
            dummyread = UART0_D; 
            
            enable_irq (UART0SE_irq_no);
            
            if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 4){
    	          printf("  in VLPR Mode !  ");
            } else if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 1){
    	          printf("  in Run Mode  !  ");
            }
            op_mode = what_mcg_mode();
            if (op_mode ==1) 
                printf("in BLPI mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==2) 
                printf(" in FBI mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==3) 
                printf(" in FEI mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==4) 
                printf(" in FEE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==5) 
                printf(" in FBE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==6) 
                printf(" in BLPE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==7) 
                printf(" in PBE mode now at %d Hz\r\n",mcg_clk_hz );
            if (op_mode ==8) 
                printf(" in PEE mode now at %d Hz\r\n",mcg_clk_hz );
            printf("  Receiving Char from VLPS in FEI(48 MHz) at 9600 baud \n\r  ");
            printf(" Hit a CTRL L to exit this loop \r\n");
            clk_monitor_0(OFF);

            while(receive_char != 0x0c)  // hit CTRL L to exit
            {
               time_delay_ms(1); 
               //while (((UART0_S1 & UART0_S1_RDRF_MASK)>>UART0_S1_RDRF_SHIFT)==0);
               enter_vlps();
               if (error_flag ==  PARITY_ERROR_Flag)
                 printf("PARITY_ERROR_Fla\n\n\r");
               if (error_flag ==  FRAMING_ERROR_Flag)
                 printf("FRAMING_ERROR_Flag\n\n\r");
               if (error_flag ==  NOISE_ERROR_Flag)
                 printf("NOISE_ERROR_Flag\n\n\r");
               if (error_flag ==  RX_OVERRUN_Flag)
                 printf("RX_OVERRUN_Flag\n\n\r");    
               
               uart0_putchar(UART0_BASE_PTR, receive_char); //echo to transmitter
               uart_putchar(UART1_BASE_PTR, receive_char);  // echo to terminal
            }  
           
            break;
   
         }
        test_num = UNDEF_VALUE; 
        receive_char = UNDEF_VALUE;

     }
}
/********************************************************************/

/* get_test_num 
 *  input       PTD2            = Lower power mode 0 = VLPS 1 = LLS 
 *              PTD4,5,6,7      = tests to perform
 *  or terminal input 
*/
char get_test_num(void) 
{
  unsigned char test_num,test_val; 
  unsigned char op_mode; 
  printf("\n\rRunning the uart_low_power_wu_dut project.\n\r");
  test_num = 0xff;
  if (1){
     while (test_num > 24 ){
            
         if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 4){
    	      printf("  in VLPR Mode !  ");
          } else if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 1){
    	      printf("  in Run Mode  !  ");
          }
          op_mode = what_mcg_mode();
          if (op_mode ==1) 
              printf("in BLPI mode now at %d Hz\r\n",mcg_clk_hz );
          if (op_mode ==2) 
              printf(" in FBI mode now at %d Hz\r\n",mcg_clk_hz );
          if (op_mode ==3) 
              printf(" in FEI mode now at %d Hz\r\n",mcg_clk_hz );
          if (op_mode ==4) 
              printf(" in FEE mode now at %d Hz\r\n",mcg_clk_hz );
          if (op_mode ==5) 
              printf(" in FBE mode now at %d Hz\r\n",mcg_clk_hz );
          if (op_mode ==6) 
              printf(" in BLPE mode now at %d Hz\r\n",mcg_clk_hz );
          if (op_mode ==7) 
              printf(" in PBE mode now at %d Hz\r\n",mcg_clk_hz );
          if (op_mode ==8) 
              printf(" in PEE mode now at %d Hz\r\n",mcg_clk_hz );
          printf("\nSelect the desired operation \n");
          printf("0 for CASE 0: Enter VLPS with 8M OSCERCLK enabled receive at    57600 baud\n");
          printf("1 for CASE 1: Enter VLPS with 8M OSCERCLK enabled receive at   115200 baud\n");
          printf("2 for CASE 2: Enter VLPS with 8M OSCERCLK enabled receive at   230400 baud\n");
          printf("3 for CASE 3: Enter VLPS with 8M OSCERCLK enabled receive at   460800 baud\n"); 
          printf("4 for CASE 4: Enter VLPS with 8M OSCERCLK enabled receive at  1000000 baud\n"); 
          printf("5 for CASE 5: Enter VLPS with 4M FAST IRC enabled receive at  2000000 baud\n"); 
          printf("6 for CASE 6: Enter VLPS with 4M FAST IRC enabled  receive at    4800 baud\n"); 
          printf("7 for CASE 7: Enter VLPS with 4M FAST IRC enabled  receive at    9600 baud\n"); 
          printf("8 for CASE 8: Enter VLPS with 4M FAST IRC enabled  receive at   19200 baud\n");
          printf("9 for CASE 9: Enter VLPS with 4M FAST IRC enabled  receive at   57600 baud\n");
          printf("A for CASE 10: Enter VLPS with 4M FAST IRC enabled receive at  115200 baud\n");
          printf("B for CASE 11: Enter VLPS with 4M FAST IRC enabled receive at  230400 baud\n");
          printf("C for CASE 12: Enter VLPS with 4M FAST IRC enabled receive at  500000 baud\n");
          printf("C for CASE 13: Enter VLPS with 4M FAST IRC enabled receive at 1000000 baud\n");
          printf("D for CASE 14: Enter VLPS in FEI 48 MHz receive at   9600 baud\n");
          printf("E for CASE 15: Enter LLS in FEI 48 MHz receive at   57600 baud\n");
          printf("F for CASE 16: Enter LLS in FEI 48 MHz receive at  115200 baud\n");
          printf("G for CASE 17: Enter LLS in FEI 48 MHz receive at  230400 baud\n");
          printf("H for CASE 18: Enter LLS in PEE 48 MHz receive at   19200 baud\n");
          printf("I for CASE 19: Enter LLS in FEI 48 MHz 4M FIRC receive at 19200 baud\n");
          printf("J for CASE 20: Enter LLS in FEI 48 MHz 4M FIRC receive at 57600 baud\n");
          printf("K for CASE 21: Enter LLS in FEI 48 MHz 4M FIRC receive at 57600 baud\n");

          printf("\n > ");
          test_val = uart_getchar(UART1_BASE_PTR);
          uart_putchar(UART1_BASE_PTR, test_val);
          printf("\n");

          if((test_val>=0x30) && (test_val<=0x39))
                test_num = test_val - 0x30;
          if((test_val>=0x41) && (test_val<=0x4E))
                test_num = test_val - 0x37;
          if((test_val>=0x61) && (test_val<=0x6E))
                test_num = test_val - 0x57;
        }

  }else
  {
  PORTD_PCR2 = ( PORT_PCR_MUX(1) |
                   PORT_PCR_PE_MASK |
                   PORT_PCR_PFE_MASK |
                   PORT_PCR_PS_MASK);
  PORTD_PCR4 = ( PORT_PCR_MUX(1) |
                   PORT_PCR_PE_MASK |
                   PORT_PCR_PFE_MASK |
                   PORT_PCR_PS_MASK);
  PORTD_PCR5 = ( PORT_PCR_MUX(1) |
                   PORT_PCR_PE_MASK |
                   PORT_PCR_PFE_MASK |
                   PORT_PCR_PS_MASK);
  PORTD_PCR6 = ( PORT_PCR_MUX(1) |
                   PORT_PCR_PE_MASK |
                   PORT_PCR_PFE_MASK |
                   PORT_PCR_PS_MASK);
  PORTD_PCR7 = ( PORT_PCR_MUX(1) |
                   PORT_PCR_PE_MASK |
                   PORT_PCR_PFE_MASK |
                   PORT_PCR_PS_MASK);
  
  if ((GPIOD_PDIR && 0x02)>>2) test_num = 0x10; // VLPS = 0, LLS = 1
  test_num |= ((GPIOD_PDIR && 0xF0)>>4);
  }
  
  return test_num;
}

void uart0_test_init (UART0_MemMapPtr uartch, int sysclk, int baud)
{
    uint8 i;
    uint32 calculated_baud = 0;
    uint32 baud_diff = 0;
    uint32 osr_val = 0;
    uint32 sbr_val, uart0clk;
    uint32 baud_rate;
    uint32 reg_temp = 0;
    uint32 temp = 0;
    
    SIM_SCGC4 |= SIM_SCGC4_UART0_MASK;
    
    // Disable UART0 before changing registers
    UART0_C2 &= ~(UART0_C2_TE_MASK | UART0_C2_RE_MASK);
  
    // Verify that a valid clock value has been passed to the function 
    if ((sysclk > 50000) || (sysclk < 32))
    {
        sysclk = 0;
        reg_temp = SIM_SOPT2;
        reg_temp &= ~SIM_SOPT2_UART0SRC_MASK;
        reg_temp |= SIM_SOPT2_UART0SRC(0);
        SIM_SOPT2 = reg_temp;
			
			  // Enter inifinite loop because the 
			  // the desired system clock value is 
			  // invalid!!
			  while(1)
				{}
    }
        
    
    // Initialize baud rate
    baud_rate = baud;
    
    // Change units to Hz
    uart0clk = sysclk * 1000;
    // Calculate the first baud rate using the lowest OSR value possible.  
    i = 4;
    sbr_val = (uint32)(uart0clk/(baud_rate * i));
    calculated_baud = (uart0clk / (i * sbr_val));
        
    if (calculated_baud > baud_rate)
        baud_diff = calculated_baud - baud_rate;
    else
        baud_diff = baud_rate - calculated_baud;
    
    osr_val = i;
        
    // Select the best OSR value
    for (i = 5; i <= 32; i++)
    {
        sbr_val = (uint32)(uart0clk/(baud_rate * i));
        calculated_baud = (uart0clk / (i * sbr_val));
        
        if (calculated_baud > baud_rate)
            temp = calculated_baud - baud_rate;
        else
            temp = baud_rate - calculated_baud;
        
        if (temp <= baud_diff)
        {
            baud_diff = temp;
            osr_val = i; 
        }
    }
    
    if (baud_diff < ((baud_rate / 100) * 3))
    {
        // If the OSR is between 4x and 8x then both
        // edge sampling MUST be turned on.  
        if ((osr_val >3) && (osr_val < 9))
            UART0_C5|= UART0_C5_BOTHEDGE_MASK;
        
        // Setup OSR value 
        reg_temp = UART0_C4;
        reg_temp &= ~UART0_C4_OSR_MASK;
        reg_temp |= UART0_C4_OSR(osr_val-1);
    
        // Write reg_temp to C4 register
        UART0_C4 = reg_temp;
        
        reg_temp = (reg_temp & UART0_C4_OSR_MASK) + 1;
        sbr_val = (uint32)((uart0clk)/(baud_rate * (reg_temp)));
        
         /* Save off the current value of the uartx_BDH except for the SBR field */
        reg_temp = UART0_BDH & ~(UART0_BDH_SBR(0x1F));
   
        UART0_BDH = reg_temp |  UART0_BDH_SBR(((sbr_val & 0x1F00) >> 8));
        UART0_BDL = (uint8)(sbr_val & UART0_BDL_SBR_MASK);
        
        /* Enable receiver and transmitter */
        UART0_C2 |= (UART0_C2_TE_MASK
                    | UART0_C2_RE_MASK );
    }
    else
		{
        // Unacceptable baud rate difference
        // More than 3% difference!!
        // Enter infinite loop!
        while(1)
				{}
		}					
    
}
