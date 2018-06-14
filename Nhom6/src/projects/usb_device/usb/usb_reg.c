#include "usb_reg.h"
#include "smc.h"
//#include "low_power.h"

/*Erase*/
unsigned char u8Temp;

/*****************************************************/
void USBReg_Testing(void)
{
    unsigned char u8LowPowerSelection;
    
    
    USB_REG_CLEAR_ENABLE;
    USB_REG_CLEAR_STDBY_STOP;   
    USB_REG_CLEAR_STDBY_VLPx;  
    printf("\n Regulator is Disabled\n");    
    printf (" Press any key to continue ");
    (void)in_char();
    //char_present();
    
    USB_REG_SET_ENABLE;
    USB_REG_CLEAR_STDBY_STOP;   
    USB_REG_CLEAR_STDBY_VLPx;
    printf("\n Regulator is in Run mode\n");
    printf (" Press any key to continue \n");
    (void)in_char();    
    
    USB_REG_SET_ENABLE;
    USB_REG_SET_STDBY_STOP;   
    USB_REG_SET_STDBY_VLPx;
    printf("\nRegulator is in standby on VLPx, LLS and VLLSx modes\n");
    
    printf("\n******************* Low Power Menu *************************\n");
    printf("\nSelect a low power mode to enter. Once in the power mode you will");
    printf("\nneed to reset MCU to return to this menu\n");
    printf("\n0) Run Mode");
    printf("\n1) Wait Mode");
    printf("\n2) Stop Mode");
    printf("\n3) VLPR Mode");
    printf("\n4) VLPW Mode");
    printf("\n5) VLPS Mode");
    printf("\n6) LLS Mode");
    printf("\n7) VLLS3 Mode");
    //printf("\n8) VLLS2 Mode");
    printf("\n8) VLLS1 Mode");
    //printf("\na) VLLS0 Mode");
    u8LowPowerSelection = in_char();
    
    switch(u8LowPowerSelection)
    {
    case '0':
      //Do nothig we are already in run mode
      break;
      
    case '1':
      printf("\nWait Mode...\n");
      enter_wait();
      break;
      
    case '2':
      printf("\nStop Mode...\n");
      MCG_C6 &= ~MCG_C6_CME0_MASK;           //Disable clock monitor
      enter_stop(0);
      break;
      
    case '3':
      printf("\nVLPR Mode...\n");
      vfnReduceClocksFromPee();
      enter_vlpr();
      u8Temp = SMC_PMSTAT;
      break;
      
    case '4':
      printf("\nVLPW Mode...\n");
      vfnReduceClocksFromPee();
      enter_vlpr();
      wait();
      break;
      
    case '5':
      printf("\nVLPS Mode...\n");
      vfnReduceClocksFromPee();
      enter_vlps();
      break;
      
    case '6':
      printf("\nLLS Mode...\n");
      enter_lls();
      break;
      
    case '7':
      printf("\nVLLS3 Mode...\n");
      enter_vlls3();
      break;
      
//    case '8':
//      printf("\nVLLS2 Mode...\n");
//      enter_vlls2();
//      break;
      
    case '8':
      printf("\nVLLS1 Mode...\n");
      enter_vlls1();
      break;
      
//    case 'a':
//      printf("\nVLLS0 Mode...\n");
//      enter_vlls0(0);
//      break;
      
    default:
      printf("\nError!! no valid mode, please restart MCU\n");
      break;
    }
    
    printf("\nRun or VLPR mode...\n");
    
    for(;;)
    {
      
    }
}

void vfnReduceClocksFromPee (void)
{
  pee_pbe(CLK0_FREQ_HZ);        //go from PEE to PBE
  pbe_blpe(CLK0_FREQ_HZ);       //go from pbe to blpe
  
  MCG_C6 &= ~(MCG_C6_CME0_MASK);     //Disable clock monitor
  
  /* Set dividers for 2MHz core and 1MHz flash using 8MHz clock */
  SIM_CLKDIV1 = (SIM_CLKDIV1_OUTDIV1(3) | SIM_CLKDIV1_OUTDIV4(1));
  
  /* Now ready to enter in VLPx modes */
  
}