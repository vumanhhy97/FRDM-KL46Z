/*
 * File:		uart_low_power_wu_dut.c
 * Purpose:		KL46 testing of uart receive
 *                      where we wake up from VLPS or LLS with a received
 *                      character and catch 1st character.
 * I/O:                  Uart 0 UART0_RX(pin 11) and UART0_TX (pin 9) J2  
 *                      Terminal - on REVA KL46 FRDM Board UART1
 */

#include "common.h"
#include "uart_low_power_driver.h"
#include "UART.h"
#include "smc.h"
#include "isr.h"
#include "lptmr.h"
#include "mcg.h"

#ifdef CMSIS
#include "start.h"
#endif

/* global variables */
/* = 9600, 19200, 57600,115200,230400,460800,1000000,2000000 */
int baud_rate[10];
extern int uart0_clk_khz;
extern int mcg_clk_hz;
extern int mcg_clk_khz;
extern int core_clk_khz;
extern int periph_clk_khz;
unsigned char sent_char,receive_char;

/********************************************************************/
int main (void)
{
        unsigned char test_num = UNDEF_VALUE; 
        unsigned char atc_status;  
        char sel_clock_source;

 //       unsigned char ch;
#ifdef CMSIS  // If we are conforming to CMSIS, we need to call start here
        start();
#endif
        
  	printf("\n\rRunning the uart_low_power_wu_dut project.\n\r");
        printf("        Initializing UART0_Rx and UART0_Tx \n\r");
        // init baud rate array
        baud_rate[0] = 4800;
        baud_rate[1] = 9600;
        baud_rate[2] = 19200;
        baud_rate[3] = 57600;
        baud_rate[4] = 115200;
        baud_rate[5] = 230400;
        baud_rate[6] = 460800;
        baud_rate[7] = 500000;
        baud_rate[8] = 1000000;
        baud_rate[9] = 2000000;
        // gpio init
        GPIOA_PDOR |= 0x00C0;   // Output high
        GPIOA_PDDR |= 0x00C0;   // PTA14 and 15 Outputs when enabled
        // fAST IRC TRIM
        atc_status = atc(FAST_IRC, FAST_IRC_FREQ, PLL_FREQ);
        printf("\n\r auto trim performed atc return = %d \r\n",atc_status );
       
     while (1)
     {
        /* Enable GPIO PTA1 */
        PORTA_PCR14 = PORT_PCR_MUX(0x1);          
        /* Enable GPIO PTA2 */
        PORTA_PCR15 = PORT_PCR_MUX(0x1);
        
        sel_clock_source =  get_clk_src(); 
        test_num = get_test_num();


        switch( test_num) {
        case XMIT_115200:
            init_uart0_baud(baud_rate[4],sel_clock_source);
            printf(" Transmitting at 115200 baud on UART0  ");             
            send_receive_verify();  
            break;
        case XMIT_230400:
            init_uart0_baud(baud_rate[5], sel_clock_source);
            printf(" Transmitting at 230400 baud on UART0  ");             
            send_receive_verify();  
            break;
        case XMIT_460800:
            init_uart0_baud(baud_rate[6], sel_clock_source);
            printf(" Transmitting at 460800 baud on UART0  "); 
            send_receive_verify();  
            break;
        case XMIT_500000:
            init_uart0_baud(baud_rate[7], sel_clock_source);
            printf(" Transmitting at 500000 baud on UART0  "); 
            send_receive_verify();  
            break;
        case XMIT_1000000:
            init_uart0_baud(baud_rate[8], sel_clock_source);
            printf(" Transmitting at 1000000 baud on UART0  "); 
            send_receive_verify();    
            break;
        case XMIT_2000000:
            init_uart0_baud(baud_rate[9], sel_clock_source);
            printf(" Transmitting at 2000000 baud on UART0  "); 
            send_receive_verify();  
            break;
        case XMIT_4800:
            init_uart0_baud(baud_rate[0], sel_clock_source);
            printf(" Transmitting at 4800 baud on UART0  ");             
            send_receive_verify();  
            break;
        case XMIT_9600:
            init_uart0_baud(baud_rate[1], sel_clock_source);
            printf(" Transmitting at 9600 baud on UART0  ");             
            send_receive_verify();  
            break;
        case XMIT_19200:
            init_uart0_baud(baud_rate[2], sel_clock_source);
            printf(" Transmitting at 19200 baud on UART0  ");             
            send_receive_verify();  
            break;
        case XMIT_57600:
            init_uart0_baud(baud_rate[3], sel_clock_source);
            printf(" Transmitting at 57600 baud on UART0  ");             
            send_receive_verify();  
            break;
        }
        test_num = UNDEF_VALUE; 
        receive_char = UNDEF_VALUE;
        sent_char = UNDEF_VALUE;
     }
}
/********************************************************************/

/* get_test_num 
 *  input       terminal input
 *  output      test_num
 * 
*/
char get_test_num(void) 
{
  unsigned char test_num,test_val; 
  unsigned char op_mode; 
  printf("\n\rRunning the uart_low_power_driver project.\n\r");
  test_num = UNDEF_VALUE;       
  while (test_num > 9 ){
            
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
          printf("  0 for CASE 0: Transmit&receive characters at    4800 baud\n"); 
          printf("  1 for CASE 1: Transmit&receive characters at    9600 baud\n");
          printf("  2 for CASE 2: Transmit&receive characters at   19200 baud\n"); 
          printf("  3 for CASE 3: Transmit&receive characters at   57600 baud\n");
          printf("  4 for CASE 4: Transmit&receive characters at  115200 baud\n");
          printf("  5 for CASE 5: Transmit&receive characters at  230400 baud\n");
          printf("  6 for CASE 6: Transmit&receive characters at  460800 baud\n");
          printf("  7 for CASE 7: Transmit&receive characters at  500000 baud\n");
          printf("  8 for CASE 8: Transmit&receive characters at 1000000 baud\n"); 
          printf("  9 for CASE 9: Transmit&receive characters at 2000000 baud\n"); 
          
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
  
  return test_num;
}
/********************************************************************/

/* get_clk_source 
 *  input       none 
 *  return      clock source to use
*/
char get_clk_src(void) 
{
  unsigned char clk_src_input,clk_src_val; 
  clk_src_val = UNDEF_VALUE;       
  while (clk_src_val > 3 ){
            
          printf("\nSelect the desired clock source \n");
          printf("  0 to use the OSCERCLK 8 MHz\n"); 
          printf("  1 to use the FAST IRC 4 MHz\n");
          printf("  2 to use the PLL_CLK 48 MHz\n"); 
          printf("  3 to use the FLL_CLK 48 MHz\n"); 
          
          printf("\n > ");
          clk_src_input = uart_getchar(UART1_BASE_PTR);
          uart_putchar(UART1_BASE_PTR, clk_src_input);
          printf("\n");

          if((clk_src_input>=0x30) && (clk_src_input<=0x39))
                clk_src_val = clk_src_input - 0x30;
          if((clk_src_input>=0x41) && (clk_src_input<=0x4E))
                clk_src_val = clk_src_input - 0x37;
          if((clk_src_input>=0x61) && (clk_src_input<=0x6E))
                clk_src_val = clk_src_input - 0x57;
        }  
  
  return clk_src_val;
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

/********************************************************************/

void send_receive_verify(void)
{
     volatile unsigned int dummyread;

     while(sent_char != 0x0c)  // hit CTRL L to exit
     {
         sent_char = uart_getchar(UART1_BASE_PTR);  // received from terminal
         uart0_putchar(UART0_BASE_PTR, sent_char); //send to transmitter
//         receive_char = uart0_getchar(UART0_BASE_PTR);
         uart_putchar(UART1_BASE_PTR, sent_char); //echo to terminal
/*         if (receive_char != sent_char) {
           printf (" <- Receive chars does not match sent character -> ");
           uart_putchar(UART1_BASE_PTR, receive_char); //echo to transmitter
           printf (" /r/n");
           // clean up UART0 buffer 
           dummyread = UART0_D;
           UART0_S1 |= UART0_S1_PF_MASK |
                       UART0_S1_FE_MASK |
                       UART0_S1_NF_MASK |
                       UART0_S1_OR_MASK  ;  // write 1 to clear error flags
           dummyread = UART0_S1;
         }*/
         if (sent_char != 0x0c) {
              receive_char = UNDEF_VALUE;
              sent_char = UNDEF_VALUE;
         }
     }  
}

void init_uart0_baud(int baud_rate, unsigned char clk_src)
{
  /* Enable the UART_TXD function on PTA1 */
  PORTA_PCR14 = PORT_PCR_MUX(0x3);          
  /* Enable the UART_TXD function on PTA2 */
  PORTA_PCR15 = PORT_PCR_MUX(0x3);
          
  if (clk_src == OSC_ER_CLK)
  {
    SIM_SOPT2 &= ~SIM_SOPT2_UART0SRC_MASK;
    SIM_SOPT2 |= SIM_SOPT2_UART0SRC(2); // select the OSCERCLK as UART0 clock source
    OSC0_CR |= OSC_CR_ERCLKEN_MASK | OSC_CR_EREFSTEN_MASK ;
    uart0_clk_khz = (CLK0_FREQ_HZ / 1000); // UART0 clock frequency will equal the OSCERCLK frequency
  } 
  else if (clk_src == FAST_IRC_CLK)
  {
    SIM_SOPT2 &= ~SIM_SOPT2_UART0SRC_MASK;
    SIM_SOPT2 |= SIM_SOPT2_UART0SRC(3); // select the FASTIRC as UART0 clock source
    MCG_C1 |= MCG_C1_IRCLKEN_MASK | MCG_C1_IREFSTEN_MASK; 
    MCG_C2 |= MCG_C2_IRCS_MASK;
    MCG_SC &= ~MCG_SC_FCRDIV_MASK; // SET TO DIVID BY 1 FOR fAST IRC  
    uart0_clk_khz = ((FAST_IRC_FREQ / 2) / 1000); // UART0 clock frequency will equal half the PLL frequency
  }  
  else if (clk_src == PLL_CLK){
    SIM_SOPT2 &= ~SIM_SOPT2_UART0SRC_MASK;
    SIM_SOPT2 |= SIM_SOPT2_UART0SRC(1); // select the PLLFLLCLK as UART0 clock source
    SIM_SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK; // set PLLFLLSEL to select the PLL for this clock source       
    uart0_clk_khz = ((mcg_clk_hz / 2) / 1000); // UART0 clock frequency will equal half the PLL frequency
  }
  else if (clk_src == FLL_CLK) {
       SIM_SOPT2 &= ~SIM_SOPT2_UART0SRC_MASK;
       SIM_SOPT2 |= SIM_SOPT2_UART0SRC(1); // select the MCGFLL clock as UART0 source
       // move to FEI mode
       mcg_clk_hz = pee_pbe(CLK0_FREQ_HZ);
       mcg_clk_hz = pbe_fbe(CLK0_FREQ_HZ);
       // set FLL freq to max frequency
       MCG_C4 &= ~MCG_C4_DRST_DRS_MASK;
       MCG_C4 |= MCG_C4_DRST_DRS(1) | MCG_C4_DMX32_MASK;  // DMX32=1 32768 kHz x1280 48 MHz

       mcg_clk_hz = fbe_fei(SLOW_IRC_FREQ);
       

       SIM_SOPT2 &= ~SIM_SOPT2_PLLFLLSEL_MASK; // set PLLFLLSEL to select the FLL for this clock source       
       mcg_clk_khz = mcg_clk_hz / 1000;
  	 core_clk_khz = mcg_clk_khz / (((SIM_CLKDIV1 & SIM_CLKDIV1_OUTDIV1_MASK) >> 28)+ 1);
       periph_clk_khz = core_clk_khz / (((SIM_CLKDIV1 & SIM_CLKDIV1_OUTDIV4_MASK) >> 16)+ 1);

       uart0_clk_khz = ((mcg_clk_hz / 2) / 1000); // UART0 clock frequency will equal half the PLL frequency   
    
  }
  uart0_test_init (UART0_BASE_PTR, uart0_clk_khz, baud_rate);
}