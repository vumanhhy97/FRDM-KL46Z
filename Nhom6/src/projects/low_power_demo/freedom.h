/*
 * File:        freedom.h
 * Purpose:     Kinetis Freedom hardware definitions
 *
 * Notes:
 */

#ifndef __FREEDOM_H__
#define __FREEDOM_H__

#include "mcg.h"

/********************************************************************/

/* Global defines to use for all Tower cards */
#define DEBUG_PRINT

// Define which CPU  you are using.
#define CPU_MKL46Z256VMC4

/*
* Input Clock Info
*/
#define CLK0_FREQ_HZ        8000000
#define CLK0_TYPE           CRYSTAL

/*
 * PLL Configuration Info
 */
//#define NO_PLL_INIT  // define this to skip PLL initilaization and stay in default FEI mode

/* The expected PLL output frequency is:
 * PLL out = (((CLKIN/PRDIV) x VDIV) / 2)
 * where the CLKIN is CLK0_FREQ_HZ.
 *
 * For more info on PLL initialization refer to the mcg driver files.
 */

#define PLL0_PRDIV      4       // divider eference by 4 = 2 MHz
#define PLL0_VDIV       24      // multiply reference by 24 = 48 MHz

/* Serial Port Info */
/**************************************************************************
   * Note:
   * 
   * Because of the changes to the UART modules, we can no longer define
   * the TERM_PORT as a base pointer.  The uart functions have been modified 
   * accommodate this change.  Now, TERM_PORT_NUM must be defined as the 
   * number of the UART port desired to use
   *
   * TERM_PORT_NUM = 0  -- This allows you to use UART0; default pins are
   *						PTA14 and PTA15
   *
   * TERM_PORT_NUM = 1  -- This allows you to use UART1; default pins are
   *						PTC3 and PTC4
   * 
   * TERM_PORT_NUM = 2  -- This allows you to use UART2; default pins are
   *						PTD2 and PTD3
   *
   *************************************************************************/

/* Uses UART0 for both Open SDA and TWR-SER Tower card */

#ifdef FRDM_REVA
#define TERM_PORT_NUM       1
#else
#define TERM_PORT_NUM       0
#endif

#define TERMINAL_BAUD       19200
#undef  HW_FLOW_CONTROL
  
#ifdef FRDM_REVA     
  #define LED0_EN (PORTA_PCR6 = PORT_PCR_MUX(1)); (GPIOA_PDDR |= 1<<6);
  #define LED1_EN (PORTC_PCR7= PORT_PCR_MUX(1));  (GPIOC_PDDR |= 1<<7);
  
  #define LED0_TOGGLE (GPIOA_PTOR = (1<<6))
  #define LED1_TOGGLE (GPIOC_PTOR = (1<<7))
  
  #define LED0_OFF (GPIOA_PSOR = (1<<6))
  #define LED1_OFF (GPIOC_PSOR = (1<<7))
  
  #define LED0_ON (GPIOA_PCOR = (1<<6))
  #define LED1_ON (GPIOC_PCOR = (1<<7))
  
  /* fast GPIO through IOPORT */
  #define _LED0_TOGGLE (FGPIOA_PTOR = (1<<6))
  #define _LED1_TOGGLE (FGPIOC_PTOR = (1<<7))
  
  //OR
  #define F_LED0_TOGGLE (FGPIOA_PTOR = (1<<6))
  #define F_LED1_TOGGLE (FGPIOC_PTOR = (1<<7))
  
  #define F_LED0_OFF (FGPIOA_PSOR = (1<<6))
  #define F_LED1_OFF (FGPIOC_PSOR = (1<<7))
    
  #define F_LED0_ON (FGPIOA_PCOR = (1<<6))
  #define F_LED1_ON (FGPIOC_PCOR = (1<<7))

#else   // FRDM_REVB
  #define LED0_EN (PORTD_PCR5 = PORT_PCR_MUX(1)); (GPIOD_PDDR |= 1<<5);
  #define LED1_EN (PORTE_PCR29= PORT_PCR_MUX(1)); (GPIOE_PDDR |= 1<<29);
  
  #define LED0_TOGGLE (GPIOD_PTOR = (1<<5))
  #define LED1_TOGGLE (GPIOE_PTOR = (1<<29))
  
  #define LED0_OFF (GPIOD_PSOR = (1<<5))
  #define LED1_OFF (GPIOE_PSOR = (1<<29))
  
  #define LED0_ON (GPIOD_PCOR = (1<<5))
  #define LED1_ON (GPIOE_PCOR = (1<<29))
  
  /* fast GPIO through IOPORT */
  #define _LED0_TOGGLE (FGPIOD_PTOR = (1<<5))
  #define _LED1_TOGGLE (FGPIOE_PTOR = (1<<29))
  
  //OR
  #define F_LED0_TOGGLE (FGPIOD_PTOR = (1<<5))
  #define F_LED1_TOGGLE (FGPIOE_PTOR = (1<<29))
  
  #define F_LED0_OFF (FGPIOD_PSOR = (1<<5))
  #define F_LED1_OFF (FGPIOE_PSOR = (1<<29))
    
  #define F_LED0_ON (FGPIOD_PCOR = (1<<5))
  #define F_LED1_ON (FGPIOE_PCOR = (1<<29))
#endif
     
     


#endif /* __FREEDOM_H__ */
