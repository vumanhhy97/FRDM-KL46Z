
 /**
  \mainpage 
  \n Copyright (c) 2011 Freescale
  \brief 
  \author   	Freescale 
  \author    
  \version      m.n
  \date         Feb 15, 2012
  
  Put here all the information needed of the Project. Basic configuration as well as information on
  the project definition will be very useful 
*/
/****************************************************************************************************/
/*                                                                                                  */
/* All software, source code, included documentation, and any implied know-how are property of      */
/* Freescale Semiconductor and therefore considered CONFIDENTIAL INFORMATION.                       */
/* This confidential information is disclosed FOR DEMONSTRATION PURPOSES ONLY.                      */
/*                                                                                                  */
/* All Confidential Information remains the property of Freescale Semiconductor and will not be     */
/* copied or reproduced without the express written permission of the Discloser, except for copies  */
/* that are absolutely necessary in order to fulfill the Purpose.                                   */
/*                                                                                                  */
/* Services performed by FREESCALE in this matter are performed AS IS and without any warranty.     */
/* CUSTOMER retains the final decision relative to the total design and functionality of the end    */
/* product.                                                                                         */
/* FREESCALE neither guarantees nor will be held liable by CUSTOMER for the success of this project.*/
/*                                                                                                  */
/* FREESCALE disclaims all warranties, express, implied or statutory including, but not limited to, */
/* implied warranty of merchantability or fitness for a particular purpose on any hardware,         */
/* software ore advise supplied to the project by FREESCALE, and or any product resulting from      */
/* FREESCALE services.                                                                              */
/* In no event shall FREESCALE be liable for incidental or consequential damages arising out of     */
/* this agreement. CUSTOMER agrees to hold FREESCALE harmless against any and all claims demands or */
/* actions by anyone on account of any damage,or injury, whether commercial, contractual, or        */
/* tortuous, rising directly or indirectly as a result of the advise or assistance supplied CUSTOMER*/ 
/* in connectionwith product, services or goods supplied under this Agreement.                      */
/*                                                                                                  */
/****************************************************************************************************/

/****************************************************************************************************/
/****************************************************************************************************/
/****************************************NOTES*******************************************************/
/****************************************************************************************************/
/****************************************************************************************************/
/* This project was designed to work on a TWR-KL25Z48M board, configured with PEE at 48Mhz               */
/*      Consult your tower board (or Freedom board) schematics for which jumper to use for current 
*       consumption measurements.  
*/
/****************************************************************************************************/

/*****************************************************************************************************
* Include files
*****************************************************************************************************/
#include "common.h"
#include "low_power_demo.h"
#include "llwu.h"
#include "smc.h"
#include "mcg.h"
#include "pmc.h"
#include "uart.h"
#include "vectors.h"
#include "lptmr.h"
#include "coremark.h"
#include "nmi_isr_handler.h"

#ifdef CMSIS
#include "start.h"
#endif

/*****************************************************************************************************
* Declaration of module wide TYPEs - NOT for use in other modules
*****************************************************************************************************/

/*****************************************************************************************************
* Definition of module wide VARIABLEs - NOT for use in other modules
*****************************************************************************************************/
unsigned char measureFlag = 0;
unsigned char enterVLLSOflag = 0;
unsigned long ram_data[216];

extern int mcg_clk_hz;
extern int mcg_clk_khz;
extern int core_clk_khz;
extern int uart0_clk_khz;

extern void enable_NMI_button(void);


int fast_irc_freq = 4000000; //4000000; // default fast irc frequency is 4MHz
int slow_irc_freq = 32768; // default slow irc frequency is 32768Hz

/*****************************************************************************************************
* Definition of module wide (CONST-) CONSTANTs - NOT for use in other modules
*****************************************************************************************************/
#define MCG_OUT_FREQ 48000000
#define CPO_LOOP_CNT 2
#define IO_NMI_DEF 1  // 1 - sets the Port interrupt, 0 - sets the NMI interrupt
/****************************************************************************************************/
//#define NOT_LOWEST_PWR 1    // define this to leave debugger enabled
//#define NOT_LOWEST_PWR_2 1  // define this to leave uart clock gate on
/*****************************************************************************************************
* Code of project wide FUNCTIONS
*****************************************************************************************************/

char uartx_getchar(void)
{
#if TERM_PORT_NUM == 2
  return uart_getchar(UART2_BASE_PTR);
    
#elif TERM_PORT_NUM == 0
   return uart0_getchar(UART0_BASE_PTR);
   
#elif TERM_PORT_NUM == 1
   return uart_getchar(UART1_BASE_PTR);
#endif
   
}

void uartx_putchar(char ch)
{
#if TERM_PORT_NUM == 2
   uart_putchar(UART2_BASE_PTR,ch);
    
#elif TERM_PORT_NUM == 0
    uart0_putchar(UART0_BASE_PTR,ch);
   
#elif TERM_PORT_NUM == 1
    uart_putchar(UART1_BASE_PTR,ch);
#endif
}


void LEDS_ON(void)
{
  LED0_ON;
  LED1_ON;
}


void LEDS_OFF(void)
{
  LED0_OFF;
  LED1_OFF;
}


int main (void)
{
#ifdef CMSIS
	start();
#endif
      
        /*Enable all operation modes because this is a write once register*/  
        SMC_PMPROT =  SMC_PMPROT_AVLLS_MASK |
                      SMC_PMPROT_ALLS_MASK  |    
                      SMC_PMPROT_AVLP_MASK;

        /*PTC3(SW2) is configured to wake up MCU from VLLSx and LLS modes, Interrup is ne*/
        LLWU_Init();
        /*Configure Tower hardware for the application*/
        Port_Init(IO_NMI_DEF);  
        SW_LED_Init();
        EnableInterrupts;
        /*Start test*/
        LowPowerModes_test();
}



/*******************************************************************************************************/

void LowPowerModes_test(void)
{
  int i;
  unsigned char op_mode,fcrdiv_val;  
  int test_num = UNDEF_VALUE;
  int wait_count;
  char test_val;
  
  printf("\n\r*------------------------------------------------------------*");
  printf("\n\r*                    KL Low Power DEMO                       *");
  printf("\n\r*                    %s %s                    *", __DATE__, __TIME__);
  printf("\n\r*------------------------------------------------------------*\n\r");
                       
	while(1)
	{
	    
            while (test_num > 22 | test_num < 0){
               LEDS_ON();
            
              
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
                printf("\n\rSelect the desired operation \n\r");
                printf("0 for CASE 0: Enter VLLS0 with POR disabled (Very Low Leakage STOP 0) NO POR\n\r");
                printf("1 for CASE 1: Enter VLLS0 with POR enabled (Very Low Leakage STOP 0) with POR\n\r");
                printf("2 for CASE 2: Enter VLLS1 (Very Low Leakage STOP 1)\n\r");
                printf("3 for CASE 3: Enter LLS with LPTMR 1 second wakeup loop (Low Leakage Stop)\n\r");
                printf("4 for CASE 4: Enter VLLS3 (Very Low Leakage STOP 3)\n\r"); 
                printf("5 for CASE 5: Enter LLS(Low Leakage Stop)\n\r");
                printf("6 for CASE 6: Enter VLPS(Very Low Power Stop)\n\r");
                printf("7 for CASE 7: Enter VLPR(Very Low Power RUN) in BLPE (8 MHz Crystal) \n\r");
                printf("8 for CASE 8: Exit VLPR(Very Low Power RUN)\n\r");
                printf("9 for CASE 9: Enter VLPW(Very Low Power WAIT)\n\r");
                printf("A for CASE 10: Enter WAIT from RUN or VLPW from VLPR\n\r");
                printf("B for CASE 11: Enter Normal STOP from RUN or VLPS from VLPR\n\r");
                printf("C for CASE 12: Enter PARTIAL STOP 1 with both system and bus clocks disabled\n\r");
                printf("D for CASE 13: Enter PARTIAL STOP 2 with system clock disabled and bus clock enabled\n\r");
                printf("E for CASE 14: Running coremark 2 times in RUN  with CPO then without CPO\n\r");
                printf("F for CASE 15: Running coremark 2 times in VLPR with CPO then without CPO\n\r");
                printf("G for CASE 16: Enable LPTMR to wakeup every 5 seconds from any mode except VLLS0\n\r");
                printf("H for CASE 17: Disable LPTMR wakeup\n\r");
                printf("I for CASE 18: Enter VLPR in BLPI at Core Frequency of 4 MHz\n\r");
                printf("J for CASE 19: Enter VLPR in BLPI at Core Frequency of 2 MHz\n\r");
                printf("K for CASE 20: Enter Compute Mode run for(i=0;i<wait_count;i++)\n\r");
                printf("L for CASE 21: To enable DEBUG");
                
                printf("\n\r > ");
                
                
                test_val = uartx_getchar();
                
                uartx_putchar(test_val);
                printf("\n\r");

                if((test_val>=0x30) && (test_val<=0x39))
                      test_num = test_val - 0x30;
                if((test_val>=0x41) && (test_val<=0x4C))
                      test_num = test_val - 0x37;
                if((test_val>=0x61) && (test_val<=0x6C))
                      test_num = test_val - 0x57;
              }
          LEDS_OFF();
#ifndef FREEDOM

#else
            printf("\n\n\r For FREEDOM board just press RESET to exit instead of SW3 or SW4\n\r\n");
#endif

            switch(test_num){
              case 0://VLLS0 no POR
                printf("Press any key to enter VLLS0 with POR disable\n\r ");
                uartx_getchar();
                printf("Press SW2 to wake up from VLLS0\n\r ");
                clockMonitor(OFF);
                set_4_lp(0);
                enter_vlls0(1);
                break;
            
             case 1://VLLS0
                printf("Press any key to enter VLLS0 with POR enabled\n\r ");
                uartx_getchar();
                printf("\n\rPress SW2 to wake up from VLLS0\n\r ");
                clockMonitor(OFF);
                set_4_lp(0);
                enter_vlls0(0);
                break;
                
                
              case 2://VLLS1
                printf("Press any key to enter VLLS1\n\r ");
                uartx_getchar();
                printf("Press SW2 to wake up from VLLS1\n\r ");
                clockMonitor(OFF);     
                set_4_lp(0);
                enter_vlls1();
                break;
              
              case 3://LLS loop wake up every 100 ms
                printf("Press any key to enter LLS with LPTMR 1 Second wakeup loop\n\r ");
                uartx_getchar();
                llwu_configure(0x0000, LLWU_PIN_FALLING, 0x1);
                    LPTMR_init(1000,LPTMR_USE_LPOCLK);
                    printf("Press SW2 to wake up from VLLS2\n\r ");
                for (i=0;i<10;i++)
                  {   
                      printf("  Entering LLS mode");
                      clockMonitor(OFF); 
                      set_4_lp(0);
                      enter_lls();
                      set_4_lp(1);
                      clockMonitor(ON);

                      /*After LLS wake up that was enter from PEE the exit will be on PBE */ 
                      /*  for this reason after wake up make sure to get back to desired  */
                      /*  clock mode                                                      */
                      op_mode = what_mcg_mode();
                      if(op_mode==PBE)
                      {
                          mcg_clk_hz = pbe_pee(CLK0_FREQ_HZ);
                          uart0_clk_khz = ((mcg_clk_hz / 2) / 1000); // UART0 clock frequency will equal half the PLL frequency
                          uart_configure(uart0_clk_khz,SIM_SOPT2_UART0SRC(1));
                      }
                      printf(" Loop Count 0x%02X ",i) ;
                      printf("  Back in RUN mode \n\r");
                    }                
               LPTMR0_CSR &= ~LPTMR_CSR_TEN_MASK;
               break;
                
              case 4://VLLS3
                printf("Press any key to enter VLLS3\n\r ");
                uartx_getchar();
                printf("Press SW2 to wake up from VLLS3\n\r ");
                clockMonitor(OFF); 
                // UART(OFF);
                enter_vlls3();
                break;
                
              case 5://LLS
                printf("Press any key to enter LLS\n\r ");
                uartx_getchar();
                printf("Press SW2 to wake up from LLS\n\r ");
                clockMonitor(OFF); 
                set_4_lp(0);
                enter_lls();
                set_4_lp(1);
                /*After LLS wake up that was enter from PEE the exit will be on PBE */ 
                /*  for this reason after wake up make sure to get back to desired  */
                /*  clock mode                                                      */
                op_mode = what_mcg_mode();
                if(op_mode==PBE)
                    {
                          mcg_clk_hz = pbe_pee(CLK0_FREQ_HZ);
                          uart0_clk_khz = ((mcg_clk_hz / 2) / 1000); // UART0 clock frequency will equal half the PLL frequency
                          uart_configure(uart0_clk_khz,SIM_SOPT2_UART0SRC(1));
                    }
                printf("  Back in RUN mode \n\r");               
                               
                break;
                
              case 6://VLPS
                printf("Press any key to enter VLPS\n\r ");
                uartx_getchar();
                printf("Press SW3 to wake up from VLPS\n\r ");
                clockMonitor(OFF);
                /*Go to VLPS*/
                enter_vlps();  //1 Means: Any interrupt could wake up from this mode      
                if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 4){
    	            printf("  in VLPR Mode !\n\r ");
                } else if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 1){
    	            op_mode = what_mcg_mode(); // check if in PBE mode and if so, switch to PEE
                    if(op_mode==PBE)
                    {
                      mcg_clk_hz = pbe_pee(CLK0_FREQ_HZ);
                      uart0_clk_khz = ((mcg_clk_hz / 2) / 1000); // UART0 clock frequency will equal half the PLL frequency
                      uart_configure(uart0_clk_khz,SIM_SOPT2_UART0SRC(1));
                      printf("  in Run Mode  !\n\r ");                  
                    }
                    clockMonitor(ON);
                }
                break;
                
              case 7://VLPR
                if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 4)
                  exit_vlpr();
                 /*Maximum clock frequency for this mode is core 4MHz and Flash 1Mhz*/
                printf("Press any key to enter VLPR\n\r ");
                uartx_getchar();
                printf("Configure clock frequency to 4MHz core clock and 1MHz flash clock\n\r ");
                op_mode = what_mcg_mode();
                if(op_mode==PEE)
                {
                  vlp_clock_config(BLPE);
                } else if (op_mode==FEI)
                {
                  vlp_clock_config(BLPI);
                }
                clockMonitor(OFF);
                /*Go to VLPR*/
                enter_vlpr();   // get out of VLPR back to RUN
                if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 4){
    	            //printf("  in VLPR Mode !\n\r ");
                  asm("nop");
                } else if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 1){
    	            printf("  in Run Mode  !\n\r ");
                    clockMonitor(ON);
                }
                break;


              case 8:// Exit VLPR
                printf("Press any key to exit VLPR\n\r ");
                uartx_getchar();
                /*Exit VLPR*/
                exit_vlpr();
                op_mode = what_mcg_mode();
                if(op_mode==BLPE)
                {
                  vlp_clock_config(PEE);    
                  clockMonitor(ON);
                } else if (op_mode==BLPI)
                {
                  vlp_clock_config(FEI);                  
                }              
                if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 4){
    	            printf("  in VLPR Mode !\n\r ");
                } else if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 1){
    	            printf("  in Run Mode  !\n\r ");
                    
                }
                break;

                
             case 9:// VLPW
                 if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 1){
    	            printf("\n\r ERROR - cannot enter VLPW from Run Mode  !\n\r ");
                } else if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 4){
    	            printf("Press any key to enter VLPW\n\r ");
                    uartx_getchar();
                    printf("Press SW3 to wake up from VLPW\n\r ");  
                    printf("  in VLPR Mode moving to VLPW!\n\r "); 
                    clockMonitor(OFF);
                    /*Enter to VLPW*/
                    enter_wait();
                    //exit_vlpw();
                    if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 4){
    	               printf("  in VLPR Mode !\n\r ");
                    } else if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 1){
    	               printf("  in Run Mode  !\n\r ");
                       clockMonitor(ON);
                   }
                }
                 break;  
                
              case 0xa://WAIT
                  printf("Press any key to enter Wait\n\r ");
                uartx_getchar();
                  printf("Press SW3 to wake up from Wait\n\r "); 
                clockMonitor(OFF);
                /*Enter wait mode*/
                enter_wait();
                if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 4){
    	            printf("  in VLPR Mode !\n\r ");
                } else if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 1){
    	            printf("  in Run Mode  !\n\r ");
                    clockMonitor(ON);
                }

                break;
              
              case 0xb://STOP
                  printf("Press any key to enter Normal Stop\n\r ");
                uartx_getchar();
                  printf("Press SW3 to wake up from Normal Stop\n\r "); 
                clockMonitor(OFF);
                /*Enter stop mode*/
                enter_stop(SMC_STOPCTRL_PSTOPO(0));
                /*After LLS wake up that was enter from PEE the exit will be on PBE */ 
                /*  for this reason after wake up make sure to get back to desired  */
                /*  clock mode                                                      */
                op_mode = what_mcg_mode();
                if(op_mode==PBE)
                {
                  mcg_clk_hz = pbe_pee(CLK0_FREQ_HZ);
                  uart0_clk_khz = ((mcg_clk_hz / 2) / 1000); // UART0 clock frequency will equal half the PLL frequency
                  uart_configure(uart0_clk_khz,SIM_SOPT2_UART0SRC(1));
                }
                if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 4){
    	            printf("  in VLPR Mode !\n\r ");
                } else if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 1){
    	            printf("  in Run Mode  !\n\r ");
                    clockMonitor(ON);
                }

                break; 
                   
              case 0xc://PSTOP1 
                  printf("Press any key to enter Stop PSTOP1\n\r ");
                uartx_getchar();
                  printf("Press SW3 to wake up from Stop PSTOP1 \n\r "); 
                clockMonitor(OFF); 
                /*Enter stop mode
                * Partial Stop Option:  
                *  0x00 = STOP - Normal Stop Mode
                *  0x40 = PSTOP1 - Partial Stop with both system and bus clocks disabled
                *  0x80 = PSTOP2 - Partial Stop with system clock disabled and
                                   bus clock enabled*/
                // The next line is to resolve an errata where the LOCK bit is cleared in CPO
                MCG_C5 |= MCG_C5_PLLSTEN0_MASK; // set PLLSTEN to keep PLL LOCK bit from clearing
                enter_stop(SMC_STOPCTRL_PSTOPO(1));
                // The next line is to clear PLLSTEN associated with PLL LOCK bit errata
                MCG_C5 &= ~MCG_C5_PLLSTEN0_MASK; // clear PLLSTEN now that CPO mode is finished with
                /*After LLS wake up that was enter from PEE the exit will be on PBE */ 
                /*  for this reason after wake up make sure to get back to desired  */
                /*  clock mode                                                      */
                op_mode = what_mcg_mode();
                 if(op_mode==PBE)
                {
                  mcg_clk_hz = pbe_pee(CLK0_FREQ_HZ);
                  uart0_clk_khz = ((mcg_clk_hz / 2) / 1000); // UART0 clock frequency will equal half the PLL frequency
                  uart_configure(uart0_clk_khz,SIM_SOPT2_UART0SRC(1));
                }
                if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 4){
    	            printf("  in VLPR Mode !\n\r ");
                } else if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 1){
    	            printf("  in Run Mode  !\n\r ");
                    clockMonitor(ON);
                }
                break; 

            case 0xd://STOP PSTOP2
                  printf("Press any key to enter Stop PSTOP2\n\r ");
                uartx_getchar();
                  printf("Press SW3 to wake up from Stop PSTOP2\n\r "); 
                clockMonitor(OFF);
                /*Enter stop mode
                * Partial Stop Option:  
                *  0x00 = STOP - Normal Stop Mode
                *  0x40 = PSTOP1 - Partial Stop with both system and bus clocks disabled
                *  0x80 = PSTOP2 - Partial Stop with system clock disabled and bus clock enabled*/
               enter_stop(SMC_STOPCTRL_PSTOPO(2));
                /*After LLS wake up that was enter from PEE the exit will be on PBE */ 
                /*  for this reason after wake up make sure to get back to desired  */
                /*  clock mode                                                      */
                op_mode = what_mcg_mode();
                if(op_mode==PBE)
                {
                  mcg_clk_hz = pbe_pee(CLK0_FREQ_HZ);
                  uart0_clk_khz = ((mcg_clk_hz / 2) / 1000); // UART0 clock frequency will equal half the PLL frequency
                  uart_configure(uart0_clk_khz,SIM_SOPT2_UART0SRC(1));
                }
                if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 4){
    	            printf("  in VLPR Mode !\n\r ");
                } else if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 1){
    	            printf("  in Run Mode  !\n\r ");
                    clockMonitor(ON);
                }
                break; 
              case 0xe://COMPUTE MODE RUN
                exit_vlpr();
                if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 4){
    	            printf("  in VLPR Mode !  ");
                } else if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 1){
    	            printf("  in Run Mode  !  ");
                }
                op_mode = what_mcg_mode();
                if (op_mode ==1) 
                    printf("in BLPI mode now at %d Hz\r\n\r",mcg_clk_hz );
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
                /*Maximum clock frequency for this mode is core 4MHz and Flash 1Mhz*/
                printf("To exit either hit RESET button or wait for loop to complete");
                printf("\n\rTransitioning from RUN mode \n\r");
                // The next line is to resolve an errata where the LOCK bit is cleared in CPO
                MCG_C5 |= MCG_C5_PLLSTEN0_MASK; // set PLLSTEN to keep PLL LOCK bit from clearing
                for (i=0;i<CPO_LOOP_CNT;i++)
                    {
                     printf("Start CoreMark in Compute Only (CPO) Mode --> \n\r");
                     uartx_putchar( ' ');       // ensure UART Tx buffer is empty
                     while(!(UART0_S1 & UART0_S1_TDRE_MASK));  //
                     enable_systick();
                     /* In compute mode */  
                     MCM_CPO |= MCM_CPO_CPOREQ_MASK;
                     while(!(MCM_CPO & MCM_CPO_CPOACK_MASK));
                     
                     wait();
                     /*****/
                     main_app(); //CoreMarks
                     /* Get Out of compute mode */
                     if(MCM_CPO & MCM_CPO_CPOACK_MASK){
                          MCM_CPO &= ~MCM_CPO_CPOREQ_MASK;
                          while (MCM_CPO & MCM_CPO_CPOACK_MASK);    
                     }else {
                       printf("Exiting test early - Did you disable LPTRM interrupt? \n\r");
                       break;
                     }
                     /****/
                     printf(" Now running Coremark without CPO mode enabled. \n\r"); 
                     main_app(); //CoreMarks
                     printf("Coremark iterations = 0x%02X of 2 \n\r",i) ;
                  }                 
                  // The next line is to clear PLLSTEN associated with PLL LOCK bit errata
                  MCG_C5 &= ~MCG_C5_PLLSTEN0_MASK; // clear PLLSTEN now that CPO mode is finished with
                  break;

              case 0xf://Coremark in VLPR
                /*Maximum clock frequency for this mode is core 4MHz and Flash 1Mhz*/
                if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 1){
    	            printf("  in RUN Mode moving to VLPR\n\r ");
                    printf("Press any key to enter VLPR\n\r ");
                    uartx_getchar();
                    printf("Configure clock frequency to 4MHz core clock and 1MHz flash clock\n\r ");
                    vlp_clock_config(BLPE);
                    /*Go to VLPR*/
                    clockMonitor(OFF);
                    enter_vlpr();   // get out of VLPR back to RUN
                } 
                if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 4){
    	            printf("  in VLPR Mode !\n\r ");
                } else if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 1){
    	            printf("  in Run Mode  !\n\r ");
                    clockMonitor(ON);
                }
                op_mode = what_mcg_mode();
                if (op_mode ==1) 
                    printf("in BLPI mode now at %d Hz\r\n\r",mcg_clk_hz );
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
                printf("\n\rTransitioning from VLPR mode \n\r"); 
                for (i=0;i<2;i++)
                  {
                     printf("Starting running Coremark in VLPR with--- CPO mode enabled. \n\r   ");
                     //enable_systick(0x00002000);
										 enable_systick();
                     /* In compute mode */  
                     MCM_CPO |= MCM_CPO_CPOREQ_MASK;
                     while(!(MCM_CPO & MCM_CPO_CPOACK_MASK));
                     
                     wait();
                     /*****/
                     main_app(); //CoreMarks
                     /* Get Out of compute mode */
                     if(MCM_CPO & MCM_CPO_CPOACK_MASK){
                          MCM_CPO &= ~MCM_CPO_CPOREQ_MASK;
                          while (MCM_CPO & MCM_CPO_CPOACK_MASK);    
                     } else{
                       printf("Exiting test early - Did you disable LPTRM interrupt? \n\r");
                       break;
                     }
                     /****/
                     printf("  Back in VLPR mode \r\n"); 
                     printf("Starting running Coremark in VLPR without CPO mode enabled. \n\r"); 
                     main_app(); //CoreMarks
                     printf("Coremark iterations = 0x%02X of 2 \n\r",i) ;
                  }
   
                  break;
 
            case 0x10:  //set up LPTMR alarm to wakeup every 5 secondes
              //enable LPTMR to cause LLWU wakeup
              llwu_configure(0x0000, LLWU_PIN_FALLING, 0x1);
              LPTMR_init(5000,LPTMR_USE_LPOCLK);
 
              break;
           
            case 0x11:  //set up LPTMR alarm to wakeup every 5 secondes
              //disable LPTMR to cause LLWU wakeup
              SIM_SCGC5 |= SIM_SCGC5_LPTMR_MASK;
              LPTMR0_CSR &= ~LPTMR_CSR_TEN_MASK;
              printf("\n\r  Disabled LPTMR  \n\r"); 
              break;
               
             case 0x12://VLPR in BLPI 4 MHZ
                /*Maximum clock frequency for this mode is core 4MHz and Flash 1Mhz*/
                printf("Press any key to enter VLPR in BLPI mode \n\r ");
                uartx_getchar();
                printf("Configure clock frequency to 4MHz core clock and 1MHz flash clock\n\r ");
                /*Get out of VLPR to change clock dividers*/
                exit_vlpr();  // get out of VLPR back to RUN
                vlp_clock_config(BLPI);
                // default clock divider is set to divide by 1 of Fast IRC and Div2 for Bus
                SIM_CLKDIV1 = (   SIM_CLKDIV1_OUTDIV1(0)
                                | SIM_CLKDIV1_OUTDIV4(3) );  /// 4
                
                fcrdiv_val = (1 << ((MCG_SC & MCG_SC_FCRDIV_MASK) >> MCG_SC_FCRDIV_SHIFT)); // calculate the fast IRC divder factor
                mcg_clk_hz =  (fast_irc_freq / fcrdiv_val); // MCGOUT frequency equals fast IRC frequency divided by 2

                mcg_clk_hz =  mcg_clk_hz / (((SIM_CLKDIV1 & SIM_CLKDIV1_OUTDIV1_MASK) >> 28)+ 1);      
                /*Go to VLPR*/
                enter_vlpr();   // get into VLPR 
                if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 4){
    	            printf("  in VLPR Mode !\n\r ");
                } else if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 1){
    	            printf("  in Run Mode  !\n\r ");
                    clockMonitor(ON);
                }

                break;

              case 0x13://VLPR in BLPI 2 MHZ
                /*Maximum clock frequency for this mode is core 4MHz and Flash 1Mhz*/
                printf("Press any key to enter VLPR\n\r ");
                uartx_getchar();
                printf("Configure clock frequency to 2MHz core clock and 1MHz flash clock\n\r ");
                /*Get out of VLPR to change clock dividers*/
                exit_vlpr();  // get out of VLPR back to RUN
                vlp_clock_config(BLPI);
                // default clock divider is set to divide by 1 of Fast IRC and Div2 for Bus
                SIM_CLKDIV1 = (   SIM_CLKDIV1_OUTDIV1(1)
                                | SIM_CLKDIV1_OUTDIV4(1) ); //2
                
                fcrdiv_val = (1 << ((MCG_SC & MCG_SC_FCRDIV_MASK) >> MCG_SC_FCRDIV_SHIFT)); // calculate the fast IRC divder factor
                mcg_clk_hz =  (fast_irc_freq / fcrdiv_val); // MCGOUT frequency equals fast IRC frequency divided by 2

                mcg_clk_hz =  mcg_clk_hz / (((SIM_CLKDIV1 & SIM_CLKDIV1_OUTDIV1_MASK) >> 28)+ 1);      
                /*Go to VLPR*/
                enter_vlpr();   // get into VLPR 
                if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 4){
    	            printf("  in VLPR Mode !\n\r ");
                } else if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 1){
    	            printf("  in Run Mode  !\n\r ");
                    clockMonitor(ON);
                }
                break;   
              
            case 0x14: // enter compute mode
              if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 4){
    	            printf("  in VLPR Mode !  ");
                    wait_count = 20000000;
                } else if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 1){
    	            printf("  in Run Mode  !  ");
                    wait_count = 200000000;
                    // The next line is to resolve an errata where the LOCK bit is cleared in CPO
                    MCG_C5 |= MCG_C5_PLLSTEN0_MASK; // set PLLSTEN to keep PLL LOCK bit from clearing
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
                printf("Start Compute Mode --> waiting for %d counts   ", wait_count);
                enable_systick();
                /* In compute mode */  
                MCM_CPO |= MCM_CPO_CPOREQ_MASK;
                while(!(MCM_CPO & MCM_CPO_CPOACK_MASK));
                     
                wait();
                for (i=0;i<wait_count;i++){
                     if(!(MCM_CPO & MCM_CPO_CPOACK_MASK))
                           break;               
                }      
                if(MCM_CPO & MCM_CPO_CPOACK_MASK){
                  MCM_CPO &= ~MCM_CPO_CPOREQ_MASK;
                  while (MCM_CPO & MCM_CPO_CPOACK_MASK);    
                }           
                printf("         \n\rWaiting is Over - Now OUT of Compute Mode\n\r ");
                break;     
            case 0x15:  // enable debug mode
                printf("\n\r*--------------D E B U G    E N A B L E D--------------------*\n\r ");
                PORTA_PCR0 = PORT_PCR_MUX(7); 
                PORTA_PCR3 = PORT_PCR_MUX(7); 
                break;
                
            }
            
            test_num = UNDEF_VALUE ;    

	} 
}

void LPTMR_init(int count, int clock_source)
{
    SIM_SCGC5 |= SIM_SCGC5_LPTMR_MASK;
    enable_irq(LPTMR_irq_no);

    LPTMR0_PSR = ( LPTMR_PSR_PRESCALE(0) // 0000 is div 2
                 | LPTMR_PSR_PBYP_MASK  // LPO feeds directly to LPT
                 | LPTMR_PSR_PCS(clock_source)) ; // use the choice of clock
    if (clock_source== 0)
      printf("\n\r LPTMR Clock source is the MCGIRCLK \n\r");
    if (clock_source== 1)
      printf("\n\r LPTMR Clock source is the LPOCLK \n\r");
    if (clock_source== 2)
      printf("\n\r LPTMR Clock source is the ERCLK32 \n\r");
    if (clock_source== 3)
      printf("\n\r LPTMR Clock source is the OSCERCLK \n\r");
             
    LPTMR0_CMR = LPTMR_CMR_COMPARE(count);  //Set compare value

    LPTMR0_CSR =(  LPTMR_CSR_TCF_MASK   // Clear any pending interrupt
                 | LPTMR_CSR_TIE_MASK   // LPT interrupt enabled
                 | LPTMR_CSR_TPS(0)     //TMR pin select
                 |!LPTMR_CSR_TPP_MASK   //TMR Pin polarity
                 |!LPTMR_CSR_TFC_MASK   // Timer Free running counter is reset whenever TMR counter equals compare
                 |!LPTMR_CSR_TMS_MASK   //LPTMR0 as Timer
                );
    LPTMR0_CSR |= LPTMR_CSR_TEN_MASK;   //Turn on LPT and start counting
}
/*******************************************************************************************************/
void LLWU_Init(void)
{
    enable_irq(LLWU_irq_no);
    enable_irq(LPTMR_irq_no);

    llwu_configure(0x0080/*PTC3*/, LLWU_PIN_RISING, 0x1);
}

/*******************************************************************************************************/
void SW_LED_Init(void)
{

#ifdef TOWER
   SIM_SCGC5 |= SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTA_MASK ; /* PORT clock enablement */ 
#else
   SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTB_MASK |SIM_SCGC5_PORTC_MASK ; /* PORT clock enablement */    
#endif        
     LED0_EN; 
     LED1_EN; 
}

/*******************************************************************************************************/

void Port_Init(int io_nmi){
  volatile unsigned int buttons_read;
  if (io_nmi){
  /*Configure SW3*/
      PORTA_PCR4 =  PORT_PCR_PS_MASK |
                    PORT_PCR_PE_MASK | 
                    PORT_PCR_PFE_MASK |
                    PORT_PCR_IRQC(10) | 
                   PORT_PCR_MUX(1); /* IRQ Falling edge */  
  }else {
        enable_NMI_button();
  }
  /* Configure SW2 - init for GPIO PTC3/LLWU_P7/UART1_RX/FTM0_CH2/CLKOUT*/
#ifndef FREEDOM
  PORTC_PCR3 = ( PORT_PCR_MUX(1) |
                   PORT_PCR_PE_MASK |
                   PORT_PCR_PFE_MASK |
                   PORT_PCR_PS_MASK);
#endif
  time_delay_ms(10);
#ifndef NOT_LOWEST_PWR

   // time delay to ensure PCR register Write is complete
  buttons_read =  GPIOC_PDIR & 0x00000008 ;
  if (buttons_read == 0x8) {  // SW3 buttons not -pressed
    //disable debug
    //PORTA_PCR0 = PORT_PCR_MUX(0); 
    //PORTA_PCR3 = PORT_PCR_MUX(0); 
    printf("\n\r*--------------D E B U G    D I S A B L E D------------------*");
    #ifndef FREEDOM
    printf("\n\r*-------Press SW2 then press Reset to re-enable debug---------*");
    #endif
    } else {
    printf("\n\r*--------------D E B U G    E N A B L E D--------------------*");
  }
#endif

}
void demo_lptmr_isr(void)
{
  volatile unsigned int dummyread;
  if(MCM_CPO & MCM_CPO_CPOACK_MASK){
      MCM_CPO &= ~MCM_CPO_CPOREQ_MASK;
      while (MCM_CPO & MCM_CPO_CPOACK_MASK);                
      printf("         \n\rLPTRM Interrupt pulled OUT of Compute Mode  \n\r");               
  }
  SIM_SCGC5 |= SIM_SCGC5_LPTMR_MASK;
  printf(" \r[demo_lptmr_isr] \n\r  > ");
 
  LPTMR0_CSR |=  LPTMR_CSR_TCF_MASK;   // write 1 to TCF to clear the LPT timer compare flag
  LPTMR0_CSR = ( LPTMR_CSR_TEN_MASK | LPTMR_CSR_TIE_MASK | LPTMR_CSR_TCF_MASK  );
      // enable timer
      // enable interrupts
      // clear the flag
  /*wait for write to complete to  before returning */  
  dummyread = LPTMR0_CSR;

  return;

}

void porta_isr(void)
{
   if(MCM_CPO & MCM_CPO_CPOACK_MASK){
      MCM_CPO &= ~MCM_CPO_CPOREQ_MASK;
      while (MCM_CPO & MCM_CPO_CPOACK_MASK);                
      printf("         \n\r[porta_isr] Interrupt pulled OUT of Compute Mode  \n\r");               
  }
  
  if(PORTA_ISFR == (1<<4))
  {
   PORTA_PCR4 |= PORT_PCR_ISF_MASK;    
  }
  printf("\n\r[porta_isr] porta4 ISF cleared\n\r");
  return;
}

/**********************************************************/
/* outof_lp = 0 for going into low power mode                */
/* outof_lp = 1 for exiting low power mode                   */
/**********************************************************/

void set_4_lp(unsigned char outof_lp)
{
    if(outof_lp)
    {
        if (TERM_PORT_NUM == 0)
        {
           SIM_SCGC4 |= SIM_SCGC4_UART0_MASK;
  	   /* Enable the pins for the selected SCI */
           /* Enable the SCI1_TXD function on PTC4 */
  	   PORTA_PCR14 = PORT_PCR_MUX(0x3); // LPSCI is alt3 function for this pin
  		
  	   /* Enable the SCI1_RXD function on PTC3 */
  	   PORTA_PCR15 = PORT_PCR_MUX(0x3); // LPSCI is alt3 function for this pin
          
           SIM_SOPT2 |= SIM_SOPT2_UART0SRC(1); // select the PLLFLLCLK as UART0 clock source
        }
  	if (TERM_PORT_NUM == 1)
  	{
           SIM_SCGC4 |= SIM_SCGC4_UART1_MASK;
           /* Enable the SCI1_TXD function on PTC4 */
           PORTC_PCR4 = PORT_PCR_MUX(0x3); // SCI is alt3 function for this pin
  		
  	   /* Enable the SCI1_RXD function on PTC3 */
  	   PORTC_PCR3 = PORT_PCR_MUX(0x3); // SCI is alt3 function for this pin
  	}
        
        if (TERM_PORT_NUM == 2)
  	{
            SIM_SCGC4 |= SIM_SCGC4_UART2_MASK;
            /* Enable the SCI2_TXD function on PTD3 */
  	    PORTE_PCR16 = PORT_PCR_MUX(0x3); // SCI is alt3 function for this pin
  		
  	    /* Enable the SCI2_RXD function on PTD2 */
  	    PORTE_PCR17 = PORT_PCR_MUX(0x3); // SCI is alt3 function for this pin
  	}
      PORTA_PCR0 =  PORT_PCR_MUX(7); // Enable Debug 
      PORTA_PCR3 =  PORT_PCR_MUX(7); // Enable Debug  
      // re-init ports
      if (IO_NMI_DEF){
      /*Configure SW3*/
          PORTA_PCR4 =  PORT_PCR_PS_MASK |
                        PORT_PCR_PE_MASK | 
                        PORT_PCR_PFE_MASK |
                        PORT_PCR_IRQC(10) | 
                        PORT_PCR_MUX(1); /* IRQ Falling edge */  
      }else {
           enable_NMI_button();
      }
      /* Configure SW3 - init for GPIO PTC3/LLWU_P7/UART1_RX/FTM0_CH2/CLKOUT*/
      PORTC_PCR3 = ( PORT_PCR_MUX(1) |
                     PORT_PCR_PE_MASK |
                     PORT_PCR_PFE_MASK |
                     PORT_PCR_PS_MASK);
    
    }else  // outof_lp = 0 for going into low power mode -  turn things off
    {
      if (TERM_PORT_NUM == 0){
         /* clock gate off the UART */
#ifndef NOT_LOWEST_PWR_2
         SIM_SCGC4 &= !SIM_SCGC4_UART0_MASK;
#endif
         PORTA_PCR14 = PORT_PCR_MUX(0x0); // Analog is alt0 function for this pin
         PORTA_PCR15 = PORT_PCR_MUX(0x0); // Analog is alt0 function for this pin}
     
      } else if (TERM_PORT_NUM == 1) {
         /* clock gate off the UART */
#ifndef NOT_LOWEST_PWR_2
         SIM_SCGC4 &= !SIM_SCGC4_UART1_MASK;
#endif
         PORTC_PCR4 = PORT_PCR_MUX(0x0);
         PORTC_PCR3 = PORT_PCR_MUX(0x0);
      } else if (TERM_PORT_NUM == 2) {
         /* clock gate off the UART */
#ifndef NOT_LOWEST_PWR_2
         SIM_SCGC4 &= !SIM_SCGC4_UART2_MASK;
#endif
         PORTE_PCR16 = PORT_PCR_MUX(0x0);
         PORTE_PCR17 = PORT_PCR_MUX(0x0);
      }
      PORTA_PCR4 = PORT_PCR_MUX(0); 
#ifndef NOT_LOWEST_PWR
      PORTA_PCR0 =  PORT_PCR_MUX(0); // Disable Debug 
      PORTA_PCR3 =  PORT_PCR_MUX(0); // Disable Debug  
#endif
   }
}

/*******************************************************************************************************/
void uart_configure(int clock_khz,int uart0_clk_src )
{
     int mcg_clock_khz;
     int core_clock_khz;
     int periph_clk_khz;
    unsigned int temp;
     
     SIM_SOPT2 &= ~SIM_SOPT2_UART0SRC_MASK; // first clear the uart0 clk source field
     SIM_SOPT2 &= ~SIM_SOPT2_UART0SRC_MASK; // first clear the uart0 clk source field
     SIM_SOPT2 |= uart0_clk_src ; // select the UART0 clock source
     
     if (TERM_PORT_NUM == 0){
        uart0_init (UART0_BASE_PTR, clock_khz, TERMINAL_BAUD); 
     }else {
        mcg_clock_khz = clock_khz ; /// 1000
        core_clock_khz = mcg_clock_khz / (((SIM_CLKDIV1 & SIM_CLKDIV1_OUTDIV1_MASK) >> 28)+ 1);
        
        if (TERM_PORT_NUM == 1) 
         {
           uart_init (UART1_BASE_PTR, core_clock_khz, TERMINAL_BAUD);
         }
        else
        {
           periph_clk_khz = core_clock_khz / (((SIM_CLKDIV1 & SIM_CLKDIV1_OUTDIV4_MASK) >> 16)+ 1);
          uart_init (UART2_BASE_PTR, periph_clk_khz*2, TERMINAL_BAUD);//
        }
          
     }
}

/*******************************************************************************************************/
int PEE_to_BLPE(void)
{
     int mcg_clock_hz;    
     
     mcg_clock_hz = atc(FAST_IRC,fast_irc_freq,MCG_OUT_FREQ);

     mcg_clock_hz = pee_pbe(CLK0_FREQ_HZ);
     mcg_clock_hz = pbe_blpe(CLK0_FREQ_HZ);


      return mcg_clock_hz; 
}

/*******************************************************************************************************/
int BLPE_to_PEE(void)
{
      int mcg_clock_hz;    
  
      SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1(0)|SIM_CLKDIV1_OUTDIV4(0x1);
      
      /*After wake up back to the original clock frequency*/            
      mcg_clock_hz = blpe_pbe(CLK0_FREQ_HZ, PLL0_PRDIV,PLL0_VDIV);
      mcg_clock_hz = pbe_pee(CLK0_FREQ_HZ);
      
      return mcg_clock_hz;
}

/*******************************************************************************************************/
void clockMonitor(unsigned char state)
{
    if(state)
      MCG_C6 |= MCG_C6_CME0_MASK;
    else
      MCG_C6 &= ~MCG_C6_CME0_MASK;
}
  
/****************************************************************/
/* vlp_clock_config(char next_mode)
 * 
 * input: next_mode = BLPI or BLPE
 *****************************************************************/
void vlp_clock_config(char next_mode)
{
  int current_mcg_mode,uart_clk_khz;
  unsigned char return_code;
  current_mcg_mode = what_mcg_mode();
  if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 4){
       printf("\n\rIn VLPR Mode ! Must go to RUN to change Clock Modes --> ");
       exit_vlpr();
  }
  if ((SMC_PMSTAT & SMC_PMSTAT_PMSTAT_MASK)== 1)
      printf("In RUN Mode !\n\r");
  switch(current_mcg_mode){
              
  case 1:  //current_mcg_mode ==BLPI
    printf("\n\r in BLPI mode with fast IRC \n\r");
    if(next_mode == FEI){
        mcg_clk_hz = blpi_fbi(slow_irc_freq, 0);
        mcg_clk_hz = fbi_fei(slow_irc_freq);
        MCG_C4 |= (MCG_C4_DRST_DRS(1) | MCG_C4_DMX32_MASK);
        mcg_clk_hz = 48000000; //FEI mode
        
        SIM_CLKDIV1 = (   SIM_CLKDIV1_OUTDIV1(0)
                    | SIM_CLKDIV1_OUTDIV4(1) );
#ifdef TWR_REVA
        uart_clk_khz = (mcg_clk_hz  / 1000); // the uart0 clock frequency will equal the FLL frequency
#else
        uart_clk_khz = (mcg_clk_hz / 2 / 1000); // the uart0 clock frequency will equal the FLL frequency
#endif
        /*Configure UART for the new clock frequency and IR clk*/
        uart_configure(uart_clk_khz,SIM_SOPT2_UART0SRC(1));                  
        SIM_SOPT2 &= ~SIM_SOPT2_PLLFLLSEL_MASK; // clear PLLFLLSEL to select the FLL for this clock source
    }
    break;
  case 2:  // current_mcg_mode ==FBI 
    printf("\n\r in FBI mode now \n\r");
    break;
  case 3:  // current_mcg_mode ==FEI) 
    printf("\n\r in FEI mode ");
    if (next_mode == BLPE)
    {
        mcg_clk_hz =  fei_fbe( CLK0_FREQ_HZ,  1, CRYSTAL);
        mcg_clk_hz = fbe_blpe(CLK0_FREQ_HZ);   
        OSC0_CR |= OSC_CR_ERCLKEN_MASK;
        uart_clk_khz = ((CLK0_FREQ_HZ ) / 1000); // UART0 clock frequency will equal ERCLK
    } else if (next_mode == BLPI)
    { //next_mode is BLPI
      MCG_SC &=  ~MCG_SC_FCRDIV_MASK;  //set to div by 1    
      mcg_clk_hz = fei_fbi(fast_irc_freq,FAST_IRC);
      mcg_clk_hz = fbi_blpi(fast_irc_freq,FAST_IRC);
      /* Internal Fast IRC is 4 MHz so div by 1 for sysclk and div 4 for flash clock*/
      SIM_CLKDIV1 = (   SIM_CLKDIV1_OUTDIV1(0)
                      | SIM_CLKDIV1_OUTDIV4(3) ); // div 5 for flash clk margin
#ifdef TWR_REVA
      uart_clk_khz = (mcg_clk_hz / 1000); // the uart0 clock frequency will equal the FAST IRC frequency
#else
      uart_clk_khz = (mcg_clk_hz / 2 / 1000); // the uart0 clock frequency will equal the FLL frequency
#endif
      MCG_C1 |= MCG_C1_IRCLKEN_MASK;
      MCG_C2 |= MCG_C2_IRCS_MASK;
    }
    /*Configure UART for the oscerclk frequency*/
    uart_configure(uart_clk_khz,SIM_SOPT2_UART0SRC(3));                  
    /* External Reference is 8 MHz so div by 2 for sysclk and div 4 for flash clock*/
    printf("\n\r moved to BLPE mode \n\r");
    break;
  case 4:  // current_mcg_mode ==4) 
    printf("\n\r in FEE mode \n\r");
    if (next_mode == BLPI){
        MCG_C2 &= ~MCG_C2_IRCS_MASK;
        MCG_C1 |= MCG_C1_IRCLKEN_MASK;
        MCG_SC &=  ~MCG_SC_FCRDIV_MASK;  //set to div by 1  
        mcg_clk_hz = fee_fbi(fast_irc_freq, FAST_IRC);
        mcg_clk_hz = fbi_blpi(fast_irc_freq,FAST_IRC);
        /* Internal Fast IRC is 4 MHz so div by 1 for sysclk and div 4 for flash clock*/
        SIM_CLKDIV1 = (   SIM_CLKDIV1_OUTDIV1(0)
                        | SIM_CLKDIV1_OUTDIV4(4) ); // div 5 for flash clk margin
        uart_clk_khz = (mcg_clk_hz / 1000); // the uart0 clock frequency will equal the FAST IRC frequency
        uart_configure(uart_clk_khz,SIM_SOPT2_UART0SRC(3));                  
        /* External Reference is 8 MHz so div by 2 for sysclk and div 4 for flash clock*/
        printf("\n moved to BLPI mode \n");
    }
    else if (next_mode == BLPE){
        mcg_clk_hz =  fee_fbe( CLK0_FREQ_HZ);
        mcg_clk_hz = fbe_blpe(CLK0_FREQ_HZ);   
        OSC0_CR |= OSC_CR_ERCLKEN_MASK;
        uart_clk_khz = ((CLK0_FREQ_HZ ) / 1000); // UART0 clock frequency will equal ERCLK
        uart_configure(uart_clk_khz,SIM_SOPT2_UART0SRC(3));                  
        /* External Reference is 8 MHz so div by 2 for sysclk and div 4 for flash clock*/
        printf("\n moved to BLPE mode \n");
    }
    
    break;
  case 5:  // current_mcg_mode ==FBE) 
    printf("\n\r in FBE mode \n\r");
    break;
  case 6:  // current_mcg_mode ==BLPE) 
    printf("\n\r in BLPE mode \n\r");
    if (next_mode == BLPI){
        MCG_C2 &= ~MCG_C2_IRCS_MASK;
        MCG_C1 |= MCG_C1_IRCLKEN_MASK;
        MCG_SC &=  ~MCG_SC_FCRDIV_MASK;  //set to div by 1  
        mcg_clk_hz = blpe_fbe(CLK0_FREQ_HZ);
        mcg_clk_hz = fbe_fbi(fast_irc_freq, FAST_IRC);
        mcg_clk_hz = fbi_blpi(fast_irc_freq,FAST_IRC);
        /* Internal Fast IRC is 4 MHz so div by 1 for sysclk and div 4 for flash clock*/
        SIM_CLKDIV1 = (   SIM_CLKDIV1_OUTDIV1(0)
                        | SIM_CLKDIV1_OUTDIV4(4) ); // div 5 for flash clk margin
        uart_clk_khz = (mcg_clk_hz / 1000); // the uart0 clock frequency will equal the FAST IRC frequency
        uart_configure(uart_clk_khz,SIM_SOPT2_UART0SRC(3));                  
        /* External Reference is 8 MHz so div by 2 for sysclk and div 4 for flash clock*/
        printf("\n\r moved to BLPI mode \n\r");
    }else if (next_mode == PEE){
       SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1(0)|
                     SIM_CLKDIV1_OUTDIV4(0x1);     
       //After wake up back to the original clock frequency          
       mcg_clk_hz = blpe_pbe(CLK0_FREQ_HZ, PLL0_PRDIV,PLL0_VDIV);
       mcg_clk_hz = pbe_pee(CLK0_FREQ_HZ);
       //Configure UART for the oscerclk frequency
       uart0_clk_khz = ((mcg_clk_hz / 2) / 1000); 
       uart_configure(uart0_clk_khz,SIM_SOPT2_UART0SRC(1));      
       printf("moved to PEE clock mode \n\r");
    }    
    break;
  case 7:  // current_mcg_mode ==PBE) 
    printf("\n\r in PBE mode \n\r");
    if (next_mode == PEE){
       SIM_CLKDIV1 = SIM_CLKDIV1_OUTDIV1(0)|
                     SIM_CLKDIV1_OUTDIV4(0x1);     
       /*After wake up back to the original clock mode*/            
       mcg_clk_hz = pbe_pee(CLK0_FREQ_HZ);
       /*Configure UART for the oscerclk frequency*/
       uart_configure(mcg_clk_hz,SIM_SOPT2_UART0SRC(1));      
    }
    printf("moved to PEE clock mode \n\r");

    break;
  case 8:  // current_mcg_mode ==8) PEE
    if (next_mode == BLPI)
    {
      // run ATC test to determine irc trim
        return_code = atc(FAST_IRC,fast_irc_freq,mcg_clk_hz);
        if (return_code != 0){
          printf("\n\rAutotrim Failed\n\r");
        }
        mcg_clk_hz =  pee_pbe(CRYSTAL);
        mcg_clk_hz = pbe_fbe(CRYSTAL);   
        MCG_SC &=  ~MCG_SC_FCRDIV_MASK;  //set to div by 1    
        mcg_clk_hz = fbe_fbi(fast_irc_freq, FAST_IRC);
        mcg_clk_hz = fbi_blpi(fast_irc_freq, FAST_IRC);
#ifdef TWR_REVA
        uart_clk_khz = ((mcg_clk_hz ) / 1000); // UART0 clock frequency will equal ERCLK
#else
        uart_clk_khz = ((mcg_clk_hz/2 ) / 1000); // 
#endif
        SIM_CLKDIV1 = (   SIM_CLKDIV1_OUTDIV1(0)
                        | SIM_CLKDIV1_OUTDIV4(3) );// div 5 for flash clk margin
        MCG_C1 |= MCG_C1_IRCLKEN_MASK;
        MCG_C2 |= MCG_C2_IRCS_MASK;
        clockMonitor(OFF);
        uart_configure(uart_clk_khz,SIM_SOPT2_UART0SRC(3));// use IRCLK
    
        printf("\n\r Now moved to BLPI clock mode \n\r");
    } else
    {
      printf("\n\r in PEE clock mode moving to BLPE clock mode \n\r");
    return_code = atc(FAST_IRC,fast_irc_freq,mcg_clk_hz);
    if (return_code != 0){
          printf("\n\rAutotrim Failed\n\r");
    }
    mcg_clk_hz = PEE_to_BLPE();
    OSC0_CR |= OSC_CR_ERCLKEN_MASK;
    /*Configure UART for the oscerclk frequency*/
    uart_clk_khz = ((fast_irc_freq ) / 1000); // UART0 clock frequency will equal fast irc
    /* External Reference is 8 MHz so div by 2 for sysclk and div 4 for flash clock*/
    SIM_CLKDIV1 = (   SIM_CLKDIV1_OUTDIV1(1)
                    | SIM_CLKDIV1_OUTDIV4(3) );            
    mcg_clk_hz =  mcg_clk_hz / (((SIM_CLKDIV1 & SIM_CLKDIV1_OUTDIV1_MASK) >> 28)+ 1);    
    MCG_C1 |= MCG_C1_IRCLKEN_MASK; // enable irc
    MCG_C2 |= MCG_C2_IRCS_MASK;    // select fast irc  
    MCG_SC &=  ~MCG_SC_FCRDIV_MASK;  //set to div by 1    
    uart_configure(uart_clk_khz,SIM_SOPT2_UART0SRC(3));// use MCGIRCLK
    printf("\n\r Now moved to BLPE clock mode \n\r");
    }
    break;
   /* end of case statement*/
  }
}
               
void print_mcg_regs(void)
{
  printf("\n\r");
  printf("MCG_C1    = %#02X \r\n", (MCG_C1));
  printf("MCG_C2    = %#02X \r\n", (MCG_C2));
  printf("MCG_C3    = %#02X \r\n", (MCG_C3));
  printf("MCG_C4    = %#02X \r\n", (MCG_C4)) ;
  printf("MCG_C5    = %#02X \r\n", (MCG_C5));
  printf("MCG_C6    = %#02X \r\n", (MCG_C6));
  printf("MCG_S     = %#02X \r\n", (MCG_S)) ;
  printf("MCG_SC    = %#02X \r\n", (MCG_SC)) ;
  printf("MCG_ATCVL = %#02X \r\n", (MCG_ATCVL)) ;
  printf("MCG_ATVCH = %#02X \r\n", (MCG_ATCVH));
  printf("MCG_C7    = %#02X \r\n", (MCG_C7));
  printf("MCG_C8    = %#02X \r\n", (MCG_C8));
} // end print_mcg_regs

void systick_isr(void)
{
  //disable systick
  SYST_CSR &= ~(SysTick_CSR_ENABLE_MASK  | 
                SysTick_CSR_TICKINT_MASK |
                SysTick_CSR_CLKSOURCE_MASK); 
  SYST_RVR = 0x00000000;
  
}

void enable_systick(void)
{
  SYST_RVR = 0x00002000; //SysTick Reload Value Register
  SYST_CVR = 0x00000000;
  SYST_CSR = SysTick_CSR_ENABLE_MASK | 
             SysTick_CSR_TICKINT_MASK |
             SysTick_CSR_CLKSOURCE_MASK;
}
