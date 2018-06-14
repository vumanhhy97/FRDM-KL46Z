#include "bsp_KL25.h"
#include "common.h"
#include "usb_bsp.h"

void vfnfll_init (void);
void fll_init (void);

void _bsp_platform_init(void)
{
    /* MPU Configuration
    MPU_CESR=0;     */                                 // MPU is disable. All accesses from all bus masters are allowed
  
    
    
  
    /** MIC2026*/
    SIM_SCGC5|=  SIM_SCGC5_PORTB_MASK;               // Turn on PTB clocks 
    PORTB_PCR11=(0|PORT_PCR_MUX(1));                 // Configure pin as GPIO
    GPIOB_PDDR|=(1<<11);                             // Set as output
    ENABLE_USB_5V;
    
    /* SIM Configuration */
    vfnInitUSBClock(USB_CLOCK);                    // Feed 48MHz to the USB module  

    // weak pulldowns
    USB0_USBCTRL=0x40;
}

void vfnInitUSBClock (uint8 u8ClkOption)
{

  switch (u8ClkOption)
  {
    case MCGPLL0:
    {
      SIM_SOPT2 |= SIM_SOPT2_PLLFLLSEL_MASK     /** PLL reference */   
                |  SIM_SOPT2_USBSRC_MASK;       /** USB fractional divider like USB reference clock */  
      /** There are no clock dividers for the L2K PLL */
      break;     
    }
    
  
    case MCGFLL:
    {
        /** Configure FLL to generate a 48MHz core clock */
       fll_init();                            
          
        SIM_SOPT2 &= ~SIM_SOPT2_PLLFLLSEL_MASK;       /** FLL reference */   
        SIM_SOPT2 |=  SIM_SOPT2_USBSRC_MASK;          /** USB fractional divider like USB reference clock */ 
        break;
    }
    
    case CLKIN:
    {
      SIM_SOPT2 &= (uint32)(~SIM_SOPT2_USBSRC_MASK);    /** PTA5 selected as USBFS CLK source */ 
      SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK;                //Enable clock on PORTA
      PORTA_PCR5=(0|PORT_PCR_MUX(2));                   // Enabling PTA5 as CLK input    
    }
  }
  
  SIM_SCGC4|=(SIM_SCGC4_USBOTG_MASK);             // USB Clock Gating
}



void fll_init(void)
{
 /**  pee_pbe(CLK0_FREQ_HZ);
  pbe_fbe(CLK0_FREQ_HZ);
  fbe_fee(CLK0_FREQ_HZ);*/

}

/**FLL output like bus clock source, divide reference by 256 (8MHz /256 = 31.25kHz)
	MCG_C1 = MCG_C1_CLKS(0) | MCG_C1_FRDIV(3);
	
	while (!(MCG_S & MCG_S_OSCINIT_MASK)){};  // wait for oscillator to initialize
	while (MCG_S & MCG_S_IREFST_MASK){}; 	  // wait for Reference clock Status bit to clear  */

        /**Fixed DCO multiplier of 1464, MCGOUT = 32.768 kHz * 1464 / 1 = 48 MHz
        MCG_C4 = MCG_C4_DMX32_MASK | MCG_C4_DRST_DRS(1);		       
        while (MCG_S & MCG_S_CLKST_MASK){};    	  // Wait for output of FLL */


/*FUNCTION*-------------------------------------------------------------------
*
* Function Name    : _bsp_usb_host_init
* Returned Value   : 0 for success
* Comments         :
*    This function performs BSP-specific initialization related to USB host
*
*END*----------------------------------------------------------------------*/
int_32 _bsp_usb_host_init(pointer2 param)
{
/*
    SCGC2 |= SCGC2_USB_MASK;       
    SOPT2 &= ~SOPT2_USB_BIGEND_MASK;
    USBTRC0 |= (USBTRC0_USBPU_MASK | USBTRC0_USBVREN_MASK); */
        
    return 0;
}