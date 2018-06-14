/******************************************************************************
* File:    low_power_demo.h
* Purpose: Declare functions used by the main application and define
* constants/macros.  
******************************************************************************/

#ifndef __LOW_POWER_DEMO_H
#define __LOW_POWER_DEMO_H 1

/*****************************************************************************************************
* Declaration of module wide FUNCTIONs - NOT for use in other modules
*****************************************************************************************************/
void LowPowerModes_test(void);
void LLWU_Init(void);
void SW_LED_Init(void);
void demo_lptmr_isr(void);
void porta_isr(void);
void Port_Init(int io_nmi);
void LPTMR_init(int count, int clock_source);
void set_4_lp(unsigned char outof_lp);
void uart_configure(int mcg_clock_hz,int uart0_clk_src);
int PEE_to_BLPE(void);
int BLPE_to_PEE(void);
void clockMonitor(unsigned char state);
void JTAG_TDO_PullUp_Enable(void);
void vlp_clock_config(char next_mode);
void print_mcg_regs(void);
void systick_isr(void);
void enable_systick(void);

#ifdef CMSIS
  #define systick_isr			SysTick_Handler
  #define demo_lptmr_isr	LPTimer_IRQHandler
  #define porta_isr				PORTA_IRQHandler
  #define llwu_isr				LLW_IRQHandler
#endif

/*****************************************************************************************************
* Definition of module wide MACROs / #DEFINE-CONSTANTs - NOT for use in other modules
*****************************************************************************************************/
#define UNDEF_VALUE  0xFF

  #define WAIT_TIME 0x0fff

#endif  //__LOW_POWER_DEMO_H

/* End of "low_power_demo.h" */
