/********************************************************************
*
* File: xcvr_test.c
*
* Purpose: XCVR Test
*
* Date: Dec 5 2012
*
*
********************************************************************/

/*******************************************************************
* Header files
*******************************************************************/
#include "xcvr_test.h"
#include "smc.h"

/*******************************************************************
* User definitions
*******************************************************************/
/* Out of reset registers status test */
//#define REGISTERS_OUT_OF_RESET
#define USB_REGISTER_COUNT              69



/*****************************************************************
* Locals
*****************************************************************/
__no_init unsigned long USB_BASE_REG @0x40072000;

/******************************************************************
* Functions
******************************************************************/
void vfnXcvrTest (void)
{
  #ifdef REGISTERS_OUT_OF_RESET
  unsigned long* pu32UsbBase = &USB_BASE_REG;
  unsigned char u8Counter;
  
  printf("\nOut of reset USB register status check...\n");
  
  SIM_SCGC4 |= SIM_SCGC4_USBOTG_MASK;
  
  for(u8Counter = 0; u8Counter < USB_REGISTER_COUNT; u8Counter++)
  {
    printf("\nUSB Register: Memory Map 0x%X = 0x%X", pu32UsbBase, (unsigned char) *pu32UsbBase);
    pu32UsbBase++;
  }
  #endif
  
  printf("\n\n********** XCVR Test **********\n\n");
  printf("Waiting for enumeration...\n");
  CDC_Init();
  CDC_Engine();
  printf("Enumerated!! Send any character to the recently enumerated device\n");
  
  for(;;)
  {
    //enter_wait();
    CDC_Engine();
  }
}

void vfnResetMcu (void)
{
  SCB_AIRCR = SCB_AIRCR_VECTKEY(0x05FA) | SCB_AIRCR_SYSRESETREQ_MASK;       //Request software reset
  
  while(1);  
}