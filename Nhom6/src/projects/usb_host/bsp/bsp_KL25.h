
#include "common.h"


enum usb_clock
{
  MCGPLL0,
  MCGFLL,
  CLKIN
};


/* Select Clock source */
#define USB_CLOCK   MCGPLL0
//#define USB_CLOCK   MCGFLL
//#define USB_CLOCK   CLKIN

/* USB Fractional Divider value for 120MHz input */
/** USB Clock = PLL x (FRAC +1) / (DIV+1)       */
/** USB Clock = 48MHz x (0+1) / (0+1) = 48 MHz    */
  #define USB_FRAC    0
  #define USB_DIV     0         //L2K does not have USB_DIV


#define ENABLE_USB_5V   GPIOB_PSOR|=(1<<11)
#define DISABLE_USB_5V  GPIOB_PSOR|=(1<<11)



void vfnInitUSBClock (uint8 u8ClkOption);
void _bsp_platform_init(void);

