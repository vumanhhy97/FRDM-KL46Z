/*
* File:	   ridd.c
* Purpose: Configure MCU to capture low power mode current.
* Author:  Ky Nguyen, Freescale Semiconductor
* Email:   g16648@freescale.com
* Date:    May 24, 2012
* RELEASE HISTORY
* VERSION DATE       AUTHOR   DESCRIPTION
* 1.0     2012-05-24 Ky Nguyen  Initial version
*
*/

#include "common.h"
#include "ridd.h"
////////////////////////////////////////////////////////////////////////////////
//                                                                            //
//                             MAIN PROGRAM                                   //
//                                                                            //
////////////////////////////////////////////////////////////////////////////////
int main(void)
{ 
  unsigned int power_mode=0;
  clock_divider clkdiv;

  // Watchdog already disabled in pre_post_main.h, so comment it out here
  disable_watchdog();
  
  // Disable LVD (if MCU is in very low power or low leakage modes, the LVD
  // system is disabled regardless of LVDSC1 settings)
  PMC_LVDSC1 = 0x00;
  
  //debug
  configure_max_clock_gating();
  MCG_C3 =0x80;   //unit was untrim, when unit is trim remove this line
  
  //wait for labview to unlock the trap
  wait_hsdio_reset();

  // Drive clocks out to port if PTC2 [ASB817 A12] is tied high
  config_clocks_out_to_port();
  
  // Adjust clock freq (div 1,2,2,4)
  clkdiv.CORE_SYS = CLKDIV1; 
  clkdiv.PERIPHERAL = CLKDIV2;
  clkdiv.FLEXBUS = CLKDIV2;
  clkdiv.FLASH = CLKDIV4;      
  set_SIM_CLKDIV1(&clkdiv); //update sim clk divider
  

  // Determine power mode that is to be entered by checking PTC[11:4] settings
  power_mode = determine_power_mode_entry_type();
 
  // Based on power_mode value determined above, configure MCU for appropriate
  // low power mode entry
  
  switch (power_mode)
  {
    case VLLS0A: // VLLS0A mode entry
      vlls0a_entry_setup();
      dut_labview_handshake();
      enter_low_power_mode();
      break;
    case VLLS0B: // VLLS0B mode entry
      vlls0b_entry_setup();
      dut_labview_handshake();
      enter_low_power_mode();
      break;    
    case VLLS1: // VLLS1 mode entry
      vlls1_entry_setup();
      dut_labview_handshake();
      enter_low_power_mode();
      break;
    case VLLS2: // VLLS2 mode entry
      vlls2_entry_setup();
      dut_labview_handshake();
      enter_low_power_mode();
      break;
    case VLLS3: // VLLS3 mode entry
      vlls3_entry_setup();
      dut_labview_handshake();
      enter_low_power_mode();      
      break;
    case LLS: // LLS mode entry
      lls_entry_setup();
      dut_labview_handshake();
      enter_low_power_mode();      
      break;
    case VLPS: // VLPS mode entry
      vlps_entry_setup();
      dut_labview_handshake();
      enter_low_power_mode();            
      break;
    case STOP: // Stop mode entry
      stop_entry_setup();
      dut_labview_handshake();
      enter_low_power_mode();            
      break;
    // ********* 1 - 1 - 1 - 2 ***************
    //8Mhz ext clk
    // VLPW mode -- BLPE 2 MHz core clock, 2 MHz bus clock, 2 MHz flexbus clock, 1 MHz flash clock
    case VLPW_CG_OFF_2M: // VLPW mode entry (minimal module clocks on)
      // Reduce clock freq (div 4,4,4,8) and change to VLPR mode
      clkdiv.CORE_SYS = CLKDIV4; 
      clkdiv.PERIPHERAL = CLKDIV4;
      clkdiv.FLEXBUS = CLKDIV4;
      clkdiv.FLASH = CLKDIV8;      
      change_from_run_to_vlpr_mode(&clkdiv); 
      configure_min_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      asm("wfi"); // Enter VLPW mode
      break;
    case VLPW_CG_ON_2M: // VLPW mode entry (minimal module clocks on)
      // Reduce clock freq (div 4,4,4,8) and change to VLPR mode
      clkdiv.CORE_SYS = CLKDIV4; 
      clkdiv.PERIPHERAL = CLKDIV4;
      clkdiv.FLEXBUS = CLKDIV4;
      clkdiv.FLASH = CLKDIV8;      
      change_from_run_to_vlpr_mode(&clkdiv); 
      configure_max_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      asm("wfi"); // Enter VLPW mode
      break;
    // ********* 1 - 1 - 1 - 4 ***************
    //8Mhz ext clk, BLPE
    //4 MHz core clock, 4 MHz bus clock, 4 MHz flexbus clock, 1 MHz flash clock
    case VLPW_CG_OFF_4M: // VLPW mode entry (minimal module clocks on)
      // Reduce clock freq (div 2,2,2,8) and change to VLPR mode
      clkdiv.CORE_SYS = CLKDIV2; 
      clkdiv.PERIPHERAL = CLKDIV2;
      clkdiv.FLEXBUS = CLKDIV2;
      clkdiv.FLASH = CLKDIV8;      
      change_from_run_to_vlpr_mode(&clkdiv); 
      configure_min_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      asm("wfi"); // Enter VLPW mode
      break;
    case VLPW_CG_ON_4M: // VLPW mode entry (minimal module clocks on)
      // Reduce clock freq (div 2,2,2,8) and change to VLPR mode
      clkdiv.CORE_SYS = CLKDIV2; 
      clkdiv.PERIPHERAL = CLKDIV2;
      clkdiv.FLEXBUS = CLKDIV2;
      clkdiv.FLASH = CLKDIV8;      
      change_from_run_to_vlpr_mode(&clkdiv); 
      configure_max_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      asm("wfi"); // Enter VLPW mode
      break;
    // ********* 1 - 1 - 1 - 1 ***************
    //8Mhz ext clk, BLPE
    //1 MHz core clock, 1 MHz bus clock, 1 MHz flexbus clock, 1 MHz flash clock      
    case VLPR_CG_OFF_CACHE_1M:
      // Reduce clock freq (div 8,8,8,8) and change to VLPR mode
      clkdiv.CORE_SYS = CLKDIV8; 
      clkdiv.PERIPHERAL = CLKDIV8;
      clkdiv.FLEXBUS = CLKDIV8;
      clkdiv.FLASH = CLKDIV8;      
      cache_enable();
      change_from_run_to_vlpr_mode(&clkdiv);
      configure_min_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;
    case VLPR_CG_ON_CACHE_1M:
      // Reduce clock freq (div 8,8,8,8) and change to VLPR mode
      clkdiv.CORE_SYS = CLKDIV8; 
      clkdiv.PERIPHERAL = CLKDIV8;
      clkdiv.FLEXBUS = CLKDIV8;
      clkdiv.FLASH = CLKDIV8;      
      cache_enable();
      change_from_run_to_vlpr_mode(&clkdiv);
      configure_max_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;
    case VLPR_CG_OFF_NOCACHE_1M:
      // Reduce clock freq (div 8,8,8,8) and change to VLPR mode
      clkdiv.CORE_SYS = CLKDIV8; 
      clkdiv.PERIPHERAL = CLKDIV8;
      clkdiv.FLEXBUS = CLKDIV8;
      clkdiv.FLASH = CLKDIV8;      
      cache_disable();
      change_from_run_to_vlpr_mode(&clkdiv);
      configure_min_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;
    case VLPR_CG_ON_NOCACHE_1M:
      // Reduce clock freq (div 8,8,8,8) and change to VLPR mode
      clkdiv.CORE_SYS = CLKDIV8; 
      clkdiv.PERIPHERAL = CLKDIV8;
      clkdiv.FLEXBUS = CLKDIV8;
      clkdiv.FLASH = CLKDIV8;      
      cache_disable();
      change_from_run_to_vlpr_mode(&clkdiv);
      configure_max_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;
    // ********* 1 - 1 - 1 - .5 ***************
    //8Mhz ext clk, BLPE
    //1 MHz core clock, 1 MHz bus clock, 1 MHz flexbus clock, 0.5 MHz flash clock      
    case VLPR_CG_OFF_CACHE_1M_FLASH_0P5M:
      // Reduce clock freq (div 8,8,8,16) and change to VLPR mode
      clkdiv.CORE_SYS = CLKDIV8; 
      clkdiv.PERIPHERAL = CLKDIV8;
      clkdiv.FLEXBUS = CLKDIV8;
      clkdiv.FLASH = CLKDIV16;      
      cache_enable();
      change_from_run_to_vlpr_mode(&clkdiv);
      configure_min_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;
    case VLPR_CG_ON_CACHE_1M_FLASH_0P5M:
      // Reduce clock freq (div 8,8,8,16) and change to VLPR mode
      clkdiv.CORE_SYS = CLKDIV8; 
      clkdiv.PERIPHERAL = CLKDIV8;
      clkdiv.FLEXBUS = CLKDIV8;
      clkdiv.FLASH = CLKDIV16;      
      cache_enable();
      change_from_run_to_vlpr_mode(&clkdiv);
      configure_max_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;
    case VLPR_CG_OFF_NOCACHE_1M_FLASH_0P5M:
      // Reduce clock freq (div 8,8,8,16) and change to VLPR mode
      clkdiv.CORE_SYS = CLKDIV8; 
      clkdiv.PERIPHERAL = CLKDIV8;
      clkdiv.FLEXBUS = CLKDIV8;
      clkdiv.FLASH = CLKDIV16;      
      cache_disable();
      change_from_run_to_vlpr_mode(&clkdiv);
      configure_min_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;
    case VLPR_CG_ON_NOCACHE_1M_FLASH_0P5M:
      // Reduce clock freq (div 8,8,8,16) and change to VLPR mode
      clkdiv.CORE_SYS = CLKDIV8; 
      clkdiv.PERIPHERAL = CLKDIV8;
      clkdiv.FLEXBUS = CLKDIV8;
      clkdiv.FLASH = CLKDIV16;      
      cache_disable();
      change_from_run_to_vlpr_mode(&clkdiv);
      configure_max_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;      
          // ********* 1 - 1 - 1 - 2 ***************
    //8Mhz ext clk, BLPE
    //2 MHz core clock, 2 MHz bus clock, 2 MHz flexbus clock, 1 MHz flash clock
  case VLPR_CG_OFF_CACHE_2M:
      // Reduce clock freq (div 4,4,4,8) and change to VLPR mode
      clkdiv.CORE_SYS = CLKDIV4; 
      clkdiv.PERIPHERAL = CLKDIV4;
      clkdiv.FLEXBUS = CLKDIV4;
      clkdiv.FLASH = CLKDIV8;  
      cache_enable();
      change_from_run_to_vlpr_mode(&clkdiv);
      configure_min_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;
  case VLPR_CG_ON_CACHE_2M:
      // Reduce clock freq (div 4,4,4,8) and change to VLPR mode
      clkdiv.CORE_SYS = CLKDIV4; 
      clkdiv.PERIPHERAL = CLKDIV4;
      clkdiv.FLEXBUS = CLKDIV4;
      clkdiv.FLASH = CLKDIV8;  
      cache_enable();
      change_from_run_to_vlpr_mode(&clkdiv);
      configure_max_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;
  case VLPR_CG_OFF_NOCACHE_2M:
      // Reduce clock freq (div 4,4,4,8) and change to VLPR mode
      clkdiv.CORE_SYS = CLKDIV4; 
      clkdiv.PERIPHERAL = CLKDIV4;
      clkdiv.FLEXBUS = CLKDIV4;
      clkdiv.FLASH = CLKDIV8;  
      cache_disable();
      change_from_run_to_vlpr_mode(&clkdiv);
      configure_min_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;
  case VLPR_CG_ON_NOCACHE_2M:
      // Reduce clock freq (div 4,4,4,8) and change to VLPR mode
      clkdiv.CORE_SYS = CLKDIV4; 
      clkdiv.PERIPHERAL = CLKDIV4;
      clkdiv.FLEXBUS = CLKDIV4;
      clkdiv.FLASH = CLKDIV8;  
      cache_disable();
      change_from_run_to_vlpr_mode(&clkdiv);
      configure_max_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;      
          // ********* 1 - 2 - 2 - 4 ***************
    //8Mhz ext clk, BLPE
    //2 MHz core clock, 1 MHz bus clock, 1 MHz flexbus clock, 0.5 MHz flash clock
  case VLPR_CG_OFF_CACHE_2M_BUS_1M_FLASH_0P5M:
      // Reduce clock freq (div 4,8,8,16) and change to VLPR mode
      clkdiv.CORE_SYS = CLKDIV4; 
      clkdiv.PERIPHERAL = CLKDIV8;
      clkdiv.FLEXBUS = CLKDIV8;
      clkdiv.FLASH = CLKDIV16;  
      cache_enable();
      change_from_run_to_vlpr_mode(&clkdiv);
      configure_min_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;
  case VLPR_CG_ON_CACHE_2M_BUS_1M_FLASH_0P5M:
      // Reduce clock freq (div 4,8,8,16) and change to VLPR mode
      clkdiv.CORE_SYS = CLKDIV4; 
      clkdiv.PERIPHERAL = CLKDIV8;
      clkdiv.FLEXBUS = CLKDIV8;
      clkdiv.FLASH = CLKDIV16;  
      cache_enable();
      change_from_run_to_vlpr_mode(&clkdiv);
      configure_max_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;
  case VLPR_CG_OFF_NOCACHE_2M_BUS_1M_FLASH_0P5M:
      // Reduce clock freq (div 4,8,8,16) and change to VLPR mode
      clkdiv.CORE_SYS = CLKDIV4; 
      clkdiv.PERIPHERAL = CLKDIV8;
      clkdiv.FLEXBUS = CLKDIV8;
      clkdiv.FLASH = CLKDIV16;  
      cache_disable();
      change_from_run_to_vlpr_mode(&clkdiv);
      configure_min_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;
  case VLPR_CG_ON_NOCACHE_2M_BUS_1M_FLASH_0P5M:
      // Reduce clock freq (div 4,8,8,16) and change to VLPR mode
      clkdiv.CORE_SYS = CLKDIV4; 
      clkdiv.PERIPHERAL = CLKDIV8;
      clkdiv.FLEXBUS = CLKDIV8;
      clkdiv.FLASH = CLKDIV16;  
      cache_disable();
      change_from_run_to_vlpr_mode(&clkdiv);
      configure_max_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;      
          // ********* 1 - 1 - 1 - 4 ***************
    //8Mhz ext clk, BLPE
    //2 MHz core clock, 2 MHz bus clock, 2 MHz flexbus clock, 0.5 MHz flash clock
  case VLPR_CG_OFF_CACHE_2M_BUS_2M_FLASH_0P5M:
      // Reduce clock freq (div 4,4,4,16) and change to VLPR mode
      clkdiv.CORE_SYS = CLKDIV4; 
      clkdiv.PERIPHERAL = CLKDIV4;
      clkdiv.FLEXBUS = CLKDIV4;
      clkdiv.FLASH = CLKDIV16;  
      cache_enable();
      change_from_run_to_vlpr_mode(&clkdiv);
      configure_min_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;
  case VLPR_CG_ON_CACHE_2M_BUS_2M_FLASH_0P5M:
      // Reduce clock freq (div 4,4,4,16) and change to VLPR mode
      clkdiv.CORE_SYS = CLKDIV4; 
      clkdiv.PERIPHERAL = CLKDIV4;
      clkdiv.FLEXBUS = CLKDIV4;
      clkdiv.FLASH = CLKDIV16;  
      cache_enable();
      change_from_run_to_vlpr_mode(&clkdiv);
      configure_max_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;
  case VLPR_CG_OFF_NOCACHE_2M_BUS_2M_FLASH_0P5M:
      // Reduce clock freq (div 4,4,4,16) and change to VLPR mode
      clkdiv.CORE_SYS = CLKDIV4; 
      clkdiv.PERIPHERAL = CLKDIV4;
      clkdiv.FLEXBUS = CLKDIV4;
      clkdiv.FLASH = CLKDIV16;  
      cache_disable();
      change_from_run_to_vlpr_mode(&clkdiv);
      configure_min_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;
  case VLPR_CG_ON_NOCACHE_2M_BUS_2M_FLASH_0P5M:
      // Reduce clock freq (div 4,4,4,16) and change to VLPR mode
      clkdiv.CORE_SYS = CLKDIV4; 
      clkdiv.PERIPHERAL = CLKDIV4;
      clkdiv.FLEXBUS = CLKDIV4;
      clkdiv.FLASH = CLKDIV16;  
      cache_disable();
      change_from_run_to_vlpr_mode(&clkdiv);
      configure_max_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;      
    // ********* 1 - 1 - 1 - 4 ***************
    //8Mhz ext clk, BLPE
    //4 MHz core clock, 4 MHz bus clock, 4 MHz flexbus clock, 1 MHz flash clock
    case VLPR_CG_OFF_CACHE_4M:
      // Reduce clock freq (div 2,2,2,8) and change to VLPR mode
      clkdiv.CORE_SYS = CLKDIV2; 
      clkdiv.PERIPHERAL = CLKDIV2;
      clkdiv.FLEXBUS = CLKDIV2;
      clkdiv.FLASH = CLKDIV8;      
      cache_enable();
      change_from_run_to_vlpr_mode(&clkdiv);
      configure_min_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;
    case VLPR_CG_ON_CACHE_4M:
      // Reduce clock freq (div 2,2,2,8) and change to VLPR mode
      clkdiv.CORE_SYS = CLKDIV2; 
      clkdiv.PERIPHERAL = CLKDIV2;
      clkdiv.FLEXBUS = CLKDIV2;
      clkdiv.FLASH = CLKDIV8;      
      cache_enable();
      change_from_run_to_vlpr_mode(&clkdiv);
      configure_max_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;
    case VLPR_CG_OFF_NOCACHE_4M:
      // Reduce clock freq (div 2,2,2,8) and change to VLPR mode
      clkdiv.CORE_SYS = CLKDIV2; 
      clkdiv.PERIPHERAL = CLKDIV2;
      clkdiv.FLEXBUS = CLKDIV2;
      clkdiv.FLASH = CLKDIV8;      
      cache_disable();
      change_from_run_to_vlpr_mode(&clkdiv);
      configure_min_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;
    case VLPR_CG_ON_NOCACHE_4M:
      // Reduce clock freq (div 2,2,2,8) and change to VLPR mode
      clkdiv.CORE_SYS = CLKDIV2; 
      clkdiv.PERIPHERAL = CLKDIV2;
      clkdiv.FLEXBUS = CLKDIV2;
      clkdiv.FLASH = CLKDIV8;      
      cache_disable();
      change_from_run_to_vlpr_mode(&clkdiv);
      configure_max_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;
    // ********* 1 - 2 - 2 - 4 ***************
    //8Mhz ext clk, BLPE
    //4 MHz core clock, 2 MHz bus clock, 2 MHz flexbus clock, 1 MHz flash clock
    case VLPR_CG_OFF_CACHE_4M_BUS_2M:
      // Reduce clock freq (div 2,4,4,8) and change to VLPR mode
      clkdiv.CORE_SYS = CLKDIV2; 
      clkdiv.PERIPHERAL = CLKDIV4;
      clkdiv.FLEXBUS = CLKDIV4;
      clkdiv.FLASH = CLKDIV8;      
      cache_enable();
      change_from_run_to_vlpr_mode(&clkdiv);
      configure_min_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;
    case VLPR_CG_ON_CACHE_4M_BUS_2M:
      // Reduce clock freq (div 2,4,4,8) and change to VLPR mode
      clkdiv.CORE_SYS = CLKDIV2; 
      clkdiv.PERIPHERAL = CLKDIV4;
      clkdiv.FLEXBUS = CLKDIV4;
      clkdiv.FLASH = CLKDIV8;      
      cache_enable();
      change_from_run_to_vlpr_mode(&clkdiv);
      configure_max_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;
    case VLPR_CG_OFF_NOCACHE_4M_BUS_2M:
      // Reduce clock freq (div 2,4,4,8) and change to VLPR mode
      clkdiv.CORE_SYS = CLKDIV2; 
      clkdiv.PERIPHERAL = CLKDIV4;
      clkdiv.FLEXBUS = CLKDIV4;
      clkdiv.FLASH = CLKDIV8;      
      cache_disable();
      change_from_run_to_vlpr_mode(&clkdiv);
      configure_min_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;
    case VLPR_CG_ON_NOCACHE_4M_BUS_2M:
      // Reduce clock freq (div 2,4,4,8) and change to VLPR mode
      clkdiv.CORE_SYS = CLKDIV2; 
      clkdiv.PERIPHERAL = CLKDIV4;
      clkdiv.FLEXBUS = CLKDIV4;
      clkdiv.FLASH = CLKDIV8;      
      cache_disable();
      change_from_run_to_vlpr_mode(&clkdiv);
      configure_max_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;      
    case WAIT_CG_OFF_25M_FEI:
      SIM_CLKDIV1 = 0x00000000;      
      sctrim_25Mhz();  //trim to get ~25Mhz.
      //Configure clocks: core/system div 1 bus div 1 Flexbus div 1 Flash div 1
      clkdiv.CORE_SYS = CLKDIV1; 
      clkdiv.PERIPHERAL = CLKDIV1;
      clkdiv.FLEXBUS = CLKDIV1;
      clkdiv.FLASH = CLKDIV1;      
      set_SIM_CLKDIV1(&clkdiv);
      // Change DCO output to low range (DCO out 25 MHz)
      change_mcg_fei_range(DCO25MHZ);	
      configure_min_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      asm("wfi"); // Enter VLPW mode
      break;
    case WAIT_CG_ON_25M_FEI:
      SIM_CLKDIV1 = 0x00000000;      
      sctrim_25Mhz();  //trim to get ~25Mhz.
      //Configure clocks: core/system div 1 bus div 1 Flexbus div 1 Flash div 1
      clkdiv.CORE_SYS = CLKDIV1; 
      clkdiv.PERIPHERAL = CLKDIV1;
      clkdiv.FLEXBUS = CLKDIV1;
      clkdiv.FLASH = CLKDIV1;      
      set_SIM_CLKDIV1(&clkdiv);
      // Change DCO output to low range (DCO out 25 MHz)
      change_mcg_fei_range(DCO25MHZ);	
      configure_max_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      asm("wfi"); // Enter VLPW mode
      break;      
    case WAIT_CG_OFF_50M_FEI: 
      SIM_CLKDIV1 = 0x00000000;      
      sctrim_25Mhz();  //trim to get ~25Mhz.
      //Configure clocks: core/system div 1 bus div 2 Flexbus div 2 Flash div 4
      clkdiv.CORE_SYS = CLKDIV1; 
      clkdiv.PERIPHERAL = CLKDIV2;
      clkdiv.FLEXBUS = CLKDIV2;
      clkdiv.FLASH = CLKDIV2;      
      set_SIM_CLKDIV1(&clkdiv);
      // Change DCO output to high range (DCO out 50MHz)
      change_mcg_fei_range(DCO50MHZ);	
      configure_min_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      asm("wfi"); // Enter VLPW mode
      break;
    case WAIT_CG_ON_50M_FEI: 
      SIM_CLKDIV1 = 0x00000000;      
      sctrim_25Mhz();  //trim to get ~25Mhz.
      //Configure clocks: core/system div 1 bus div 2 Flexbus div 2 Flash div 4
      clkdiv.CORE_SYS = CLKDIV1; 
      clkdiv.PERIPHERAL = CLKDIV2;
      clkdiv.FLEXBUS = CLKDIV2;
      clkdiv.FLASH = CLKDIV2;      
      set_SIM_CLKDIV1(&clkdiv);
      // Change DCO output to high range (DCO out 50MHz)
      change_mcg_fei_range(DCO50MHZ);	
      configure_max_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      asm("wfi"); // Enter VLPW mode
      break;
    // ********* 1 - 1 - 1 - 1 ***************
    //6.25Mhz ext clk, FBE
    //6.25 MHz core clock, 6.25 MHz bus clock, 6.25 MHz flexbus clock, 6.25 MHz flash clock
    case RIDD_CG_OFF_CACHE_1M_FBE:
    case RIDD_CG_OFF_CACHE_2M_FBE:
    case RIDD_CG_OFF_CACHE_4M_FBE:
    case RIDD_CG_OFF_CACHE_6P25M_FBE:
      // Adjust clock freq (div 1,1,1,1)
      clkdiv.CORE_SYS = CLKDIV1; 
      clkdiv.PERIPHERAL = CLKDIV1;
      clkdiv.FLEXBUS = CLKDIV1;
      clkdiv.FLASH = CLKDIV1;      
      set_SIM_CLKDIV1(&clkdiv); //update sim clk divider
      mcg_osc_fbe(FREQ_1MHZ_8MHZ);//select osc 1Mhz to 8Mhz
      cache_enable();
      configure_min_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;
    case RIDD_CG_ON_CACHE_1M_FBE:
    case RIDD_CG_ON_CACHE_2M_FBE:
    case RIDD_CG_ON_CACHE_4M_FBE:
    case RIDD_CG_ON_CACHE_6P25M_FBE:
      // Adjust clock freq (div 1,1,1,1)
      clkdiv.CORE_SYS = CLKDIV1; 
      clkdiv.PERIPHERAL = CLKDIV1;
      clkdiv.FLEXBUS = CLKDIV1;
      clkdiv.FLASH = CLKDIV1;      
      set_SIM_CLKDIV1(&clkdiv); //update sim clk divider
      mcg_osc_fbe(FREQ_1MHZ_8MHZ);//select osc 1Mhz to 8Mhz
      cache_enable();
      configure_max_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;
    case RIDD_CG_OFF_NOCACHE_1M_FBE:
    case RIDD_CG_OFF_NOCACHE_2M_FBE:
    case RIDD_CG_OFF_NOCACHE_4M_FBE:
    case RIDD_CG_OFF_NOCACHE_6P25M_FBE:
      // Adjust clock freq (div 1,1,1,1)
      clkdiv.CORE_SYS = CLKDIV1; 
      clkdiv.PERIPHERAL = CLKDIV1;
      clkdiv.FLEXBUS = CLKDIV1;
      clkdiv.FLASH = CLKDIV1;      
      set_SIM_CLKDIV1(&clkdiv); //update sim clk divider
      mcg_osc_fbe(FREQ_1MHZ_8MHZ);//select osc 1Mhz to 8Mhz
      cache_disable();
      configure_min_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;
    case RIDD_CG_ON_NOCACHE_1M_FBE:
    case RIDD_CG_ON_NOCACHE_2M_FBE:
    case RIDD_CG_ON_NOCACHE_4M_FBE:
    case RIDD_CG_ON_NOCACHE_6P25M_FBE:
      // Adjust clock freq (div 1,1,1,1)
      clkdiv.CORE_SYS = CLKDIV1; 
      clkdiv.PERIPHERAL = CLKDIV1;
      clkdiv.FLEXBUS = CLKDIV1;
      clkdiv.FLASH = CLKDIV1;      
      set_SIM_CLKDIV1(&clkdiv); //update sim clk divider
      mcg_osc_fbe(FREQ_1MHZ_8MHZ);//select osc 1Mhz to 8Mhz
      cache_disable();
      configure_max_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;       
    // ********* 1 - 1 - 1 - 1 ***************
    //12.5Mhz, 25Mhz ext clk, FBE
    case RIDD_CG_OFF_CACHE_12P5M_FBE:
    case RIDD_CG_OFF_CACHE_25M_FBE:
      // Adjust clock freq (div 1,1,1,1)
      clkdiv.CORE_SYS = CLKDIV1; 
      clkdiv.PERIPHERAL = CLKDIV1;
      clkdiv.FLEXBUS = CLKDIV1;
      clkdiv.FLASH = CLKDIV1;      
      set_SIM_CLKDIV1(&clkdiv); //update sim clk divider
      mcg_osc_fbe(FREQ_8MHZ_32MHZ);//select osc 8Mhz to 32Mhz
      cache_enable();
      configure_min_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;
    case RIDD_CG_ON_CACHE_12P5M_FBE:
    case RIDD_CG_ON_CACHE_25M_FBE:
      // Adjust clock freq (div 1,1,1,1)
      clkdiv.CORE_SYS = CLKDIV1; 
      clkdiv.PERIPHERAL = CLKDIV1;
      clkdiv.FLEXBUS = CLKDIV1;
      clkdiv.FLASH = CLKDIV1;      
      set_SIM_CLKDIV1(&clkdiv); //update sim clk divider
      mcg_osc_fbe(FREQ_8MHZ_32MHZ);//select osc 8Mhz to 32Mhz
      cache_enable();
      configure_max_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;
    case RIDD_CG_OFF_NOCACHE_12P5M_FBE:
    case RIDD_CG_OFF_NOCACHE_25M_FBE:
      // Adjust clock freq (div 1,1,1,1)
      clkdiv.CORE_SYS = CLKDIV1; 
      clkdiv.PERIPHERAL = CLKDIV1;
      clkdiv.FLEXBUS = CLKDIV1;
      clkdiv.FLASH = CLKDIV1;      
      set_SIM_CLKDIV1(&clkdiv); //update sim clk divider
      mcg_osc_fbe(FREQ_8MHZ_32MHZ);//select osc 8Mhz to 32Mhz
      cache_disable();
      configure_min_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;
    case RIDD_CG_ON_NOCACHE_12P5M_FBE:
    case RIDD_CG_ON_NOCACHE_25M_FBE:
      // Adjust clock freq (div 1,1,1,1)
      clkdiv.CORE_SYS = CLKDIV1; 
      clkdiv.PERIPHERAL = CLKDIV1;
      clkdiv.FLEXBUS = CLKDIV1;
      clkdiv.FLASH = CLKDIV1;      
      set_SIM_CLKDIV1(&clkdiv); //update sim clk divider
      mcg_osc_fbe(FREQ_8MHZ_32MHZ);//select osc 8Mhz to 32Mhz
      cache_disable();
      configure_max_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;
    case RIDD_CG_OFF_CACHE_25M_FEI: 
      SIM_CLKDIV1 = 0x00000000;      
      sctrim_25Mhz();  //trim to get ~25Mhz.
      //Configure clocks: core/system div 1 bus div 1 Flexbus div 1 Flash div 1
      clkdiv.CORE_SYS = CLKDIV1; 
      clkdiv.PERIPHERAL = CLKDIV1;
      clkdiv.FLEXBUS = CLKDIV1;
      clkdiv.FLASH = CLKDIV1;
      //update sim clk divider
      set_SIM_CLKDIV1(&clkdiv);
      // Change DCO output to high range (DCO out 25MHz)
      change_mcg_fei_range(DCO25MHZ);	
      cache_enable();
      configure_min_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;
    case RIDD_CG_ON_CACHE_25M_FEI: 
      SIM_CLKDIV1 = 0x00000000;      
      sctrim_25Mhz();  //trim to get ~25Mhz.
      //Configure clocks: core/system div 1 bus div 1 Flexbus div 1 Flash div 1
      clkdiv.CORE_SYS = CLKDIV1; 
      clkdiv.PERIPHERAL = CLKDIV1;
      clkdiv.FLEXBUS = CLKDIV1;
      clkdiv.FLASH = CLKDIV1;
      //update sim clk divider
      set_SIM_CLKDIV1(&clkdiv);
      // Change DCO output to high range (DCO out 25MHz)
      change_mcg_fei_range(DCO25MHZ);	
      cache_enable();
      configure_max_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;
    case RIDD_CG_OFF_NOCACHE_25M_FEI: 
      SIM_CLKDIV1 = 0x00000000;      
      sctrim_25Mhz();  //trim to get ~25Mhz.
      //Configure clocks: core/system div 1 bus div 1 Flexbus div 1 Flash div 1
      clkdiv.CORE_SYS = CLKDIV1; 
      clkdiv.PERIPHERAL = CLKDIV1;
      clkdiv.FLEXBUS = CLKDIV1;
      clkdiv.FLASH = CLKDIV1;
      //update sim clk divider
      set_SIM_CLKDIV1(&clkdiv);
      // Change DCO output to high range (DCO out 25MHz)
      change_mcg_fei_range(DCO25MHZ);	
      cache_disable();
      configure_min_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;
      
    case RIDD_CG_ON_NOCACHE_25M_FEI: 
      SIM_CLKDIV1 = 0x00000000;      
      sctrim_25Mhz();  //trim to get ~25Mhz.
      //Configure clocks: core/system div 1 bus div 1 Flexbus div 1 Flash div 1
      clkdiv.CORE_SYS = CLKDIV1; 
      clkdiv.PERIPHERAL = CLKDIV1;
      clkdiv.FLEXBUS = CLKDIV1;
      clkdiv.FLASH = CLKDIV1;
      //update sim clk divider
      set_SIM_CLKDIV1(&clkdiv);
      // Change DCO output to high range (DCO out 25MHz)
      change_mcg_fei_range(DCO25MHZ);	
      cache_disable();
      configure_max_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;       
    // ********* 1 - 2 - 2 - 2 ***************
    //10Mhz ext clk, FBE
    //50 MHz core clock, 25 MHz bus clock, 25 MHz flexbus clock, 25 MHz flash clock
    case RIDD_CG_OFF_CACHE_50M_FBE:
      // Adjust clock freq (div 1,2,2,2)
      clkdiv.CORE_SYS = CLKDIV1; 
      clkdiv.PERIPHERAL = CLKDIV2;
      clkdiv.FLEXBUS = CLKDIV2;
      clkdiv.FLASH = CLKDIV2;      
      set_SIM_CLKDIV1(&clkdiv); //update sim clk divider
      mcg_osc_fbe(FREQ_8MHZ_32MHZ);//select osc 8Mhz to 32Mhz
      cache_enable();
      configure_min_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;
    case RIDD_CG_ON_CACHE_50M_FBE:
      // Adjust clock freq (div 1,2,2,2)
      clkdiv.CORE_SYS = CLKDIV1; 
      clkdiv.PERIPHERAL = CLKDIV2;
      clkdiv.FLEXBUS = CLKDIV2;
      clkdiv.FLASH = CLKDIV2;      
      set_SIM_CLKDIV1(&clkdiv); //update sim clk divider
      mcg_osc_fbe(FREQ_8MHZ_32MHZ);//select osc 8Mhz to 32Mhz
      cache_enable();
      configure_max_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;
    case RIDD_CG_OFF_NOCACHE_50M_FBE:
      // Adjust clock freq (div 1,2,2,2)
      clkdiv.CORE_SYS = CLKDIV1; 
      clkdiv.PERIPHERAL = CLKDIV2;
      clkdiv.FLEXBUS = CLKDIV2;
      clkdiv.FLASH = CLKDIV2;      
      set_SIM_CLKDIV1(&clkdiv); //update sim clk divider
      mcg_osc_fbe(FREQ_8MHZ_32MHZ);//select osc 8Mhz to 32Mhz
      cache_disable();
      configure_min_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;
    case RIDD_CG_ON_NOCACHE_50M_FBE:
      // Adjust clock freq (div 1,2,2,2)
      clkdiv.CORE_SYS = CLKDIV1; 
      clkdiv.PERIPHERAL = CLKDIV2;
      clkdiv.FLEXBUS = CLKDIV2;
      clkdiv.FLASH = CLKDIV2;      
      set_SIM_CLKDIV1(&clkdiv); //update sim clk divider
      mcg_osc_fbe(FREQ_8MHZ_32MHZ);//select osc 8Mhz to 32Mhz
      cache_disable();
      configure_max_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;     
    // ********* 1 - 2 - 2 - 2 ***************
    //9.6Mhz ext clk, FEE
    //72 MHz core clock, 36 MHz bus clock, 36 MHz flexbus clock, 24 MHz flash clock
    case WAIT_CG_OFF_72M_FEE:
      // Adjust clock freq (div 1,2,2,3)
      clkdiv.CORE_SYS = CLKDIV1; 
      clkdiv.PERIPHERAL = CLKDIV2;
      clkdiv.FLEXBUS = CLKDIV2;
      clkdiv.FLASH = CLKDIV3;      
      set_SIM_CLKDIV1(&clkdiv); //update sim clk divider
      /*Change to external clock  range selected for the crystal 
      oscillator of 8 MHz to 32 MHz*/
      mcg_osc_fbe(FREQ_8MHZ_32MHZ);//select osc 8Mhz to 32Mhz
      /*Switch from ext clk to PLL. Assumes 9.6 MHz ext clk*/
      mcg_osc_fee(DCO75MHZ);  //get 72Mhz
      cache_enable();
      configure_min_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      asm("wfi"); // Enter VLPW mode
      break;
    case WAIT_CG_ON_72M_FEE:
      // Adjust clock freq (div 1,2,2,3)
      clkdiv.CORE_SYS = CLKDIV1; 
      clkdiv.PERIPHERAL = CLKDIV2;
      clkdiv.FLEXBUS = CLKDIV2;
      clkdiv.FLASH = CLKDIV3;      
      set_SIM_CLKDIV1(&clkdiv); //update sim clk divider
      /*Change to external clock  range selected for the crystal 
      oscillator of 8 MHz to 32 MHz*/
      mcg_osc_fbe(FREQ_8MHZ_32MHZ);//select osc 8Mhz to 32Mhz
      /*Switch from ext clk to PLL. Assumes 9.6 MHz ext clk*/
      mcg_osc_fee(DCO75MHZ);  //get 72Mhz
      cache_enable();
      configure_max_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      asm("wfi"); // Enter VLPW mode
      break;     
    case RIDD_CG_OFF_CACHE_72M_FEE:
      // Adjust clock freq (div 1,2,2,3)
      clkdiv.CORE_SYS = CLKDIV1; 
      clkdiv.PERIPHERAL = CLKDIV2;
      clkdiv.FLEXBUS = CLKDIV2;
      clkdiv.FLASH = CLKDIV3;      
      set_SIM_CLKDIV1(&clkdiv); //update sim clk divider
      /*Change to external clock  range selected for the crystal 
      oscillator of 8 MHz to 32 MHz*/
      mcg_osc_fbe(FREQ_8MHZ_32MHZ);//select osc 8Mhz to 32Mhz
      /*Switch from ext clk to PLL. Assumes 9.6 MHz ext clk*/
      mcg_osc_fee(DCO75MHZ);  //get 72Mhz
      cache_enable();
      configure_min_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;
    case RIDD_CG_ON_CACHE_72M_FEE:
      // Adjust clock freq (div 1,2,2,3)
      clkdiv.CORE_SYS = CLKDIV1; 
      clkdiv.PERIPHERAL = CLKDIV2;
      clkdiv.FLEXBUS = CLKDIV2;
      clkdiv.FLASH = CLKDIV3;      
      set_SIM_CLKDIV1(&clkdiv); //update sim clk divider
      /*Change to external clock  range selected for the crystal 
      oscillator of 8 MHz to 32 MHz*/
      mcg_osc_fbe(FREQ_8MHZ_32MHZ);//select osc 8Mhz to 32Mhz
      /*Switch from ext clk to PLL. Assumes 9.6 MHz ext clk*/
      mcg_osc_fee(DCO75MHZ);  //get 72Mhz
      cache_enable();
      configure_max_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;
    case RIDD_CG_OFF_NOCACHE_72M_FEE:
      // Adjust clock freq (div 1,2,2,3)
      clkdiv.CORE_SYS = CLKDIV1; 
      clkdiv.PERIPHERAL = CLKDIV2;
      clkdiv.FLEXBUS = CLKDIV2;
      clkdiv.FLASH = CLKDIV3;      
      set_SIM_CLKDIV1(&clkdiv); //update sim clk divider
      /*Change to external clock  range selected for the crystal 
      oscillator of 8 MHz to 32 MHz*/
      mcg_osc_fbe(FREQ_8MHZ_32MHZ);//select osc 8Mhz to 32Mhz
      /*Switch from ext clk to PLL. Assumes 9.6 MHz ext clk*/
      mcg_osc_fee(DCO75MHZ);  //get 72Mhz
      cache_disable();
      configure_min_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;
    case RIDD_CG_ON_NOCACHE_72M_FEE:
      // Adjust clock freq (div 1,2,2,3)
      clkdiv.CORE_SYS = CLKDIV1; 
      clkdiv.PERIPHERAL = CLKDIV2;
      clkdiv.FLEXBUS = CLKDIV2;
      clkdiv.FLASH = CLKDIV3;      
      set_SIM_CLKDIV1(&clkdiv); //update sim clk divider
      /*Change to external clock  range selected for the crystal 
      oscillator of 8 MHz to 32 MHz*/
      mcg_osc_fbe(FREQ_8MHZ_32MHZ);//select osc 8Mhz to 32Mhz
      /*Switch from ext clk to PLL. Assumes 9.6 MHz ext clk*/
      mcg_osc_fee(DCO75MHZ);  //get 72Mhz
      cache_disable();
      configure_max_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;  
    // ********* 1 - 2 - 2 - 2 ***************
    //10Mhz ext clk, PEE
    //72 MHz core clock, 36 MHz bus clock, 36 MHz flexbus clock, 24 MHz flash clock
    case WAIT_CG_OFF_72M_PEE:
      // Adjust clock freq (div 1,2,2,3)
      clkdiv.CORE_SYS = CLKDIV1; 
      clkdiv.PERIPHERAL = CLKDIV2;
      clkdiv.FLEXBUS = CLKDIV2;
      clkdiv.FLASH = CLKDIV3;      
      set_SIM_CLKDIV1(&clkdiv); //update sim clk divider
      /*Change to external clock  range selected for the crystal 
      oscillator of 8 MHz to 32 MHz*/
      mcg_osc_fbe(FREQ_8MHZ_32MHZ);//select osc 8Mhz to 32Mhz
      /*Switch from ext clk to PLL. Assumes 10 MHz ext clk*/
      pll_initb(PRDIV5,VDIV36); // set PLL to 2MhzX36=72MHz.      
      cache_enable();
      configure_min_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      asm("wfi"); // Enter VLPW mode
      break;
    case WAIT_CG_ON_72M_PEE:
      // Adjust clock freq (div 1,2,2,3)
      clkdiv.CORE_SYS = CLKDIV1; 
      clkdiv.PERIPHERAL = CLKDIV2;
      clkdiv.FLEXBUS = CLKDIV2;
      clkdiv.FLASH = CLKDIV3;      
      set_SIM_CLKDIV1(&clkdiv); //update sim clk divider
      /*Change to external clock  range selected for the crystal 
      oscillator of 8 MHz to 32 MHz*/
      mcg_osc_fbe(FREQ_8MHZ_32MHZ);//select osc 8Mhz to 32Mhz
      /*Switch from ext clk to PLL. Assumes 10 MHz ext clk*/
      pll_initb(PRDIV5,VDIV36); // set PLL to 2MhzX36=72MHz. 
      cache_enable();
      configure_max_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      asm("wfi"); // Enter VLPW mode
      break;     
    case RIDD_CG_OFF_CACHE_72M_PEE:
      // Adjust clock freq (div 1,2,2,3)
      clkdiv.CORE_SYS = CLKDIV1; 
      clkdiv.PERIPHERAL = CLKDIV2;
      clkdiv.FLEXBUS = CLKDIV2;
      clkdiv.FLASH = CLKDIV3;      
      set_SIM_CLKDIV1(&clkdiv); //update sim clk divider
      /*Change to external clock  range selected for the crystal 
      oscillator of 8 MHz to 32 MHz*/
      mcg_osc_fbe(FREQ_8MHZ_32MHZ);//select osc 8Mhz to 32Mhz
      /*Switch from ext clk to PLL. Assumes 10 MHz ext clk*/
      pll_initb(PRDIV5,VDIV36); // set PLL to 2MhzX36=72MHz.      
      cache_enable();
      configure_min_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;
    case RIDD_CG_ON_CACHE_72M_PEE:
      // Adjust clock freq (div 1,2,2,3)
      clkdiv.CORE_SYS = CLKDIV1; 
      clkdiv.PERIPHERAL = CLKDIV2;
      clkdiv.FLEXBUS = CLKDIV2;
      clkdiv.FLASH = CLKDIV3;      
      set_SIM_CLKDIV1(&clkdiv); //update sim clk divider
      /*Change to external clock  range selected for the crystal 
      oscillator of 8 MHz to 32 MHz*/
      mcg_osc_fbe(FREQ_8MHZ_32MHZ);//select osc 8Mhz to 32Mhz
      /*Switch from ext clk to PLL. Assumes 10 MHz ext clk*/
      pll_initb(PRDIV5,VDIV36); // set PLL to 2MhzX36=72MHz. 
      cache_enable();
      configure_max_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;
    case RIDD_CG_OFF_NOCACHE_72M_PEE:
      // Adjust clock freq (div 1,2,2,3)
      clkdiv.CORE_SYS = CLKDIV1; 
      clkdiv.PERIPHERAL = CLKDIV2;
      clkdiv.FLEXBUS = CLKDIV2;
      clkdiv.FLASH = CLKDIV3;      
      set_SIM_CLKDIV1(&clkdiv); //update sim clk divider
      /*Change to external clock  range selected for the crystal 
      oscillator of 8 MHz to 32 MHz*/
      mcg_osc_fbe(FREQ_8MHZ_32MHZ);//select osc 8Mhz to 32Mhz
      /*Switch from ext clk to PLL. Assumes 10 MHz ext clk*/
      pll_initb(PRDIV5,VDIV36); // set PLL to 2MhzX36=72MHz. 
      cache_disable();
      configure_min_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;
    case RIDD_CG_ON_NOCACHE_72M_PEE:
      // Adjust clock freq (div 1,2,2,3)
      clkdiv.CORE_SYS = CLKDIV1; 
      clkdiv.PERIPHERAL = CLKDIV2;
      clkdiv.FLEXBUS = CLKDIV2;
      clkdiv.FLASH = CLKDIV3;      
      set_SIM_CLKDIV1(&clkdiv); //update sim clk divider
      /*Change to external clock  range selected for the crystal 
      oscillator of 8 MHz to 32 MHz*/
      mcg_osc_fbe(FREQ_8MHZ_32MHZ);//select osc 8Mhz to 32Mhz
      /*Switch from ext clk to PLL. Assumes 10 MHz ext clk*/
      pll_initb(PRDIV5,VDIV36); // set PLL to 2MhzX36=72MHz. 
      cache_disable();
      configure_max_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;    
    case RIDD_CG_OFF_CACHE_50M_FEI: 
      SIM_CLKDIV1 = 0x00000000;      
      sctrim_25Mhz();  //trim to get ~25Mhz.
      //Configure clocks: core/system div 1 bus div 2 Flexbus div 2 Flash div 4
      clkdiv.CORE_SYS = CLKDIV1; 
      clkdiv.PERIPHERAL = CLKDIV2;
      clkdiv.FLEXBUS = CLKDIV2;
      clkdiv.FLASH = CLKDIV2;
      //update sim clk divider
      set_SIM_CLKDIV1(&clkdiv);
      // Change DCO output to high range (DCO out 50MHz)
      change_mcg_fei_range(DCO50MHZ);	
      cache_enable();
      configure_min_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;
    case RIDD_CG_ON_CACHE_50M_FEI: 
      SIM_CLKDIV1 = 0x00000000;      
      sctrim_25Mhz();  //trim to get ~25Mhz.
      //Configure clocks: core/system div 1 bus div 2 Flexbus div 2 Flash div 4
      clkdiv.CORE_SYS = CLKDIV1; 
      clkdiv.PERIPHERAL = CLKDIV2;
      clkdiv.FLEXBUS = CLKDIV2;
      clkdiv.FLASH = CLKDIV2;
      //update sim clk divider
      set_SIM_CLKDIV1(&clkdiv);
      // Change DCO output to high range (DCO out 50MHz)
      change_mcg_fei_range(DCO50MHZ);	
      cache_enable();
      configure_max_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;
    case RIDD_CG_OFF_NOCACHE_50M_FEI: 
      SIM_CLKDIV1 = 0x00000000;      
      sctrim_25Mhz();  //trim to get ~25Mhz.
      //Configure clocks: core/system div 1 bus div 2 Flexbus div 2 Flash div 4
      clkdiv.CORE_SYS = CLKDIV1; 
      clkdiv.PERIPHERAL = CLKDIV2;
      clkdiv.FLEXBUS = CLKDIV2;
      clkdiv.FLASH = CLKDIV2;
      //update sim clk divider
      set_SIM_CLKDIV1(&clkdiv);
      // Change DCO output to high range (DCO out 50MHz)
      change_mcg_fei_range(DCO50MHZ);	
      cache_disable();
      configure_min_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;
    case RIDD_CG_ON_NOCACHE_50M_FEI: 
      SIM_CLKDIV1 = 0x00000000;      
      sctrim_25Mhz();  //trim to get ~25Mhz.
      //Configure clocks: core/system div 1 bus div 2 Flexbus div 2 Flash div 4
      clkdiv.CORE_SYS = CLKDIV1; 
      clkdiv.PERIPHERAL = CLKDIV2;
      clkdiv.FLEXBUS = CLKDIV2;
      clkdiv.FLASH = CLKDIV2;
      //update sim clk divider
      set_SIM_CLKDIV1(&clkdiv);
      // Change DCO output to high range (DCO out 50MHz)
      change_mcg_fei_range(DCO50MHZ);	
      cache_disable();
      configure_max_clock_gating();
      dut_labview_handshake();
      SetPortACtoDefault();//disable all portc and 0-5 on porta
      while(1){};
      break;       
    // Other cases below will be implemented at a future date                       
    // If power_mode is not any of the above values, stay in Run mode
    // by entering an infinite loop
    default: while(1); 
  }
  
///////////////////////////////////////////////////////////////////////////////
// Code shouldn't go past here -- MCU to recover through wakeup or interrupt
///////////////////////////////////////////////////////////////////////////////
}

////////////////////////////////////////////////////////////////////////////////
// MCG_CONFIG_FREQ_RANGE
// Inputs: Freqency range (valid values are 0, 1, 2 or 3 (2 and 3 translates
// to same range
// Configures frequency range for crystal oscillator or external freq source.
////////////////////////////////////////////////////////////////////////////////
void mcg_config_freq_range(uint8_t value)
{    		
  uint8_t mcg_reg1;						        		
				        		
  mcg_reg1 = MCG_C2;
  MCG_C2 = value << 4 | (mcg_reg1 & 0x0f);
}

////////////////////////////////////////////////////////////////////////////////
// MCG_CONFIG_LP_MODE
// Configures MCG for low power mode operation.
////////////////////////////////////////////////////////////////////////////////
void mcg_config_lp_mode(void)
{
//  MCGC2_LP = 1;  // Set low power bit
  MCG_C2 |= MCG_C2_LP_MASK;  
}

////////////////////////////////////////////////////////////////////////////////
// MCG_CHECK_BLPE_MODE
// Verifies that MCG has been configured for use with external clock source and
// for low power mode operation.
////////////////////////////////////////////////////////////////////////////////
void mcg_check_blpe_mode() {
  while ((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT != 0x02);
  /* stay in loop if LP is not 1 */
  while ((MCG_C2 & MCG_C2_LP_MASK) != MCG_C2_LP_MASK);
}

////////////////////////////////////////////////////////////////////////////////
// MCG_OSC_BLPE
// Inputs: Freqency range (valid values are 0, 1, 2 or 3 (2 and 3 translates
// to same range
// Configures frequency accordingly and changes MCG mode from FEI to BLPE mode.
////////////////////////////////////////////////////////////////////////////////
void mcg_osc_blpe(uint8_t range)
{
  mcg_config_freq_range(range);
  mcg_config_fbe_mode();
  mcg_config_lp_mode();
  mcg_check_blpe_mode();  
}

void change_mcg_fei_range(uint8_t range)
{
  if (range < 0x4)
  {
    //clear DRS bits first
    MCG_C4 &= ~MCG_C4_DRST_DRS_MASK;
    //now update DRS bits
    MCG_C4 |= MCG_C4_DRST_DRS(range);
  }
}
////////////////////////////////////////////////////////////////////////////////
// DISABLE_WATCHDOG
// Disables the watchdog module.
////////////////////////////////////////////////////////////////////////////////
void disable_watchdog(void)
{
  SIM_COPC = 0x00;
}

////////////////////////////////////////////////////////////////////////////////
// CONFIGURE_CLOCK_GATING
// Turns on clock gating for select modules.
////////////////////////////////////////////////////////////////////////////////
void configure_min_clock_gating(void)
{
  SIM_SCGC4 = 0x00000000;  
  /*verify bit*/
  while (SIM_SCGC4 != 0xF0000030) {};  
  SIM_SCGC5 = 0x00000000;  
  /*verify bit*/
  while (SIM_SCGC5 != 0x00000180) {};  
  SIM_SCGC6 = SIM_SCGC6_FTF_MASK;
  /*verify bit*/
  while (SIM_SCGC6 != 0x00000001) {};  
  SIM_SCGC7 = 0x00000000;
  /*verify bit*/
  while (SIM_SCGC7 != 0){};
  // Disable USB regulator
  SIM_SOPT1CFG |= SIM_SOPT1CFG_URWE_MASK;
  SIM_SOPT1 &= ~SIM_SOPT1_USBREGEN_MASK;  
  /*verify bit*/
  while ((SIM_SOPT1 & SIM_SOPT1_USBREGEN_MASK) != 0){};
  
}


////////////////////////////////////////////////////////////////////////////////
// configure_max_clock_gating
// Turns clocks for all modules on
////////////////////////////////////////////////////////////////////////////////
void configure_max_clock_gating(void)
{

  SIM_SCGC4 = 0xFFFFFFFF;
  /*verify bit*/
  while (SIM_SCGC4 != 0xF0CC1CF0){};  
  SIM_SCGC5 = 0xFFFFFFFF;
  /*verify bit*/
  while (SIM_SCGC5 != 0x00003FA1){};  
  SIM_SCGC6 = 0xFFFFFFFF;
  /*verify bit*/
  while (SIM_SCGC6 != 0xAF800003){};  
  SIM_SCGC7 = 0xFFFFFFFF;
  /*verify bit*/
  while (SIM_SCGC7 != 0x00000100){};
  // Enable USB regulator
  SIM_SOPT1CFG |= SIM_SOPT1CFG_URWE_MASK;
  SIM_SOPT1 |= SIM_SOPT1_USBREGEN_MASK;  
  /*verify bit*/
  while ((SIM_SOPT1 & SIM_SOPT1_USBREGEN_MASK) != SIM_SOPT1_USBREGEN_MASK){};
  
}

////////////////////////////////////////////////////////////////////////////////
// CONFIG_CLOCKS_OUT_TO_PORT
// Configures ports to drive MCU clocks out on them if PTB16 is tied high.
////////////////////////////////////////////////////////////////////////////////
void config_clocks_out_to_port(void)
{
  unsigned int portb_temp;
  SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK | SIM_SCGC5_PORTD_MASK; 
  PORTD_PCR2=(0|PORTD_PCR2_MUX(1)); //Set PTD2 to GPIO
  portb_temp = (GPIOD_PDIR & 0x00000004);
  if (portb_temp == 0x00000004)  
  {
    PORTC_PCR3 |= PORTC_PCR3_MUX(5); //set PTC3 as clock out
    /*
    Selects the clock to output on the CLKOUT pin.
    000 Reserved
    001 Reserved
    010 Bus clock
    011 LPO clock (1 kHz)
    100 IRCLK
    101 Reserved
    110 ERCLK0
    111 Reserved
    */
    SIM_SOPT2 |= SIM_SOPT2_CLKOUTSEL(2);
    
  }
}

////////////////////////////////////////////////////////////////////////////////
// DETERMINE_POWER_MODE_ENTRY_TYPE
// Set up MCU for low power mode entry based on jumper settings on PTC[11:4]
////////////////////////////////////////////////////////////////////////////////
int determine_power_mode_entry_type()
{
  unsigned int portb_temp;
  unsigned int power_mode;
  SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK | SIM_SCGC5_PORTD_MASK; 
  PORTD_PCR7=(0|PORTD_PCR7_MUX(1)); //Set PTC7 to GPIO
  PORTD_PCR6=(0|PORTD_PCR6_MUX(1)); //Set PTC6 to GPIO
  PORTD_PCR5=(0|PORTD_PCR5_MUX(1)); //Set PTC5 to GPIO
  PORTD_PCR4=(0|PORTD_PCR4_MUX(1)); //Set PTC4 to GPIO
  PORTC_PCR2=(0|PORTC_PCR2_MUX(1)); //Set PTC2 to GPIO
  PORTC_PCR1=(0|PORTC_PCR1_MUX(1)); //Set PTC1 to GPIO
  PORTC_PCR0=(0|PORTC_PCR0_MUX(1)); //Set PTC0 to GPIO
  portb_temp = (GPIOD_PDIR & 0x000000F0);
  power_mode = (portb_temp >> 4);
  portb_temp = (GPIOC_PDIR & 0x00000007);
  power_mode = (power_mode | (portb_temp << 4));
  return power_mode;
}

////////////////////////////////////////////////////////////////////////////////
// EXECUTION_DELAY
// Pauses execution for a specified period.
////////////////////////////////////////////////////////////////////////////////
void execution_delay(int cycles)
{
  unsigned int n;
  for(n=0;n<cycles;n++)
    {
  }  
}


////////////////////////////////////////////////////////////////////////////////
// Perform dut labview handshake
// Use PTD0 as input and PTD1 as output. MCU output high out PTD1 to let labview
// know code has advance to new branch never, labview needs to drive high
// to release DUT to enter branch never stage for current measurement.
////////////////////////////////////////////////////////////////////////////////
void dut_labview_handshake(void)
{
  unsigned int sim_scgc5_temp;
  sim_scgc5_temp = SIM_SCGC5;       //save for retore before exit
  SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK;
  PORTD_PCR0=(0|PORTD_PCR0_MUX(1)); //Set PTD0 to GPIO
  PORTD_PCR1=(0|PORTD_PCR1_MUX(1)); //Set PTD1 to GPIO
  GPIOD_PDDR |= 0x2;  //Set PTD1 as an output
  GPIOD_PDOR |= 0x2; //Set PTD1 to 1
  while ((GPIOD_PDIR & 0x1) != 0x1){};//branch never if PTC0 not 1	   
  GPIOD_PCOR = 0x2;   //drive output bit 6 low
  /*
  Disable clk out pin
  */  
  PORTC_PCR3=(0|PORTC_PCR3_MUX(1)); //Set PTC3 to GPIO
  
  SIM_SCGC5 = sim_scgc5_temp;   //restore clock gate as previously
  
}

////////////////////////////////////////////////////////////////////////////////
// VLLS0B_ENTRY_SETUP
// Configure MCU for VLLS1 mode entry.
////////////////////////////////////////////////////////////////////////////////
void vlls0b_entry_setup(void)
{
  // Configure MCU to allow VLLS mode
  SMC_PMPROT = SMC_PMPROT_AVLLS_MASK;
  //Clear STOPM first
  SMC_PMCTRL &= ~SMC_PMCTRL_STOPM_MASK;
  SMC_PMCTRL |= SMC_PMCTRL_STOPM(0x4);
  //Clear VLLSM first
  SMC_VLLSCTRL &= ~SMC_VLLSCTRL_VLLSM_MASK;
  SMC_VLLSCTRL |= SMC_VLLSCTRL_VLLSM(0);
  //check status bit for correct mode indicator
  while (SMC_PMSTAT && SMC_PMSTAT_PMSTAT(0x40) != SMC_PMSTAT_PMSTAT(0x40));
}
////////////////////////////////////////////////////////////////////////////////
// VLLS0A_ENTRY_SETUP
// Configure MCU for VLLS1 mode entry.
////////////////////////////////////////////////////////////////////////////////
void vlls0a_entry_setup(void)
{
  // Configure MCU to allow VLLS mode
  SMC_PMPROT = SMC_PMPROT_AVLLS_MASK;
  //Clear STOPM first
  SMC_PMCTRL &= ~SMC_PMCTRL_STOPM_MASK;
  SMC_PMCTRL |= SMC_PMCTRL_STOPM(0x4);
  //Clear VLLSM first
  SMC_VLLSCTRL &= ~SMC_VLLSCTRL_VLLSM_MASK;
  SMC_VLLSCTRL |= SMC_VLLSCTRL_VLLSM(0) | SMC_VLLSCTRL_PORPO_MASK;
}
////////////////////////////////////////////////////////////////////////////////
// VLLS1_ENTRY_SETUP
// Configure MCU for VLLS1 mode entry.
////////////////////////////////////////////////////////////////////////////////
void vlls1_entry_setup(void)
{
  // Configure MCU to allow VLLS mode
  SMC_PMPROT = SMC_PMPROT_AVLLS_MASK;
  //Clear STOPM first
  SMC_PMCTRL &= ~SMC_PMCTRL_STOPM_MASK;
  SMC_PMCTRL |= SMC_PMCTRL_STOPM(0x4);
  //Clear VLLSM first
  SMC_VLLSCTRL &= ~SMC_VLLSCTRL_VLLSM_MASK;
  SMC_VLLSCTRL |= SMC_VLLSCTRL_VLLSM(1);
}

////////////////////////////////////////////////////////////////////////////////
// VLLS2_ENTRY_SETUP
// Configure MCU for VLLS2 mode entry.
////////////////////////////////////////////////////////////////////////////////
void vlls2_entry_setup(void)
{
  // Configure MCU to allow VLLS mode
  SMC_PMPROT = SMC_PMPROT_AVLLS_MASK;
  //Clear STOPM first
  SMC_PMCTRL &= ~SMC_PMCTRL_STOPM_MASK;
  SMC_PMCTRL |= SMC_PMCTRL_STOPM(0x4);
  //Clear VLLSM first
  SMC_VLLSCTRL &= ~SMC_VLLSCTRL_VLLSM_MASK;
  SMC_VLLSCTRL |= SMC_VLLSCTRL_VLLSM(2);
}

////////////////////////////////////////////////////////////////////////////////
// VLLS3_ENTRY_SETUP
// Configure MCU for VLLS3 mode entry.
////////////////////////////////////////////////////////////////////////////////
void vlls3_entry_setup(void)
{
  // Configure MCU to allow VLLS mode
  SMC_PMPROT = SMC_PMPROT_AVLLS_MASK;
  //Clear STOPM first
  SMC_PMCTRL &= ~SMC_PMCTRL_STOPM_MASK;
  SMC_PMCTRL |= SMC_PMCTRL_STOPM(0x4);
  //Clear VLLSM first
  SMC_VLLSCTRL &= ~SMC_VLLSCTRL_VLLSM_MASK;
  SMC_VLLSCTRL |= SMC_VLLSCTRL_VLLSM(3);
}

////////////////////////////////////////////////////////////////////////////////
// LLS_ENTRY_SETUP
// Configure MCU for LLS mode entry.
////////////////////////////////////////////////////////////////////////////////
void lls_entry_setup(void)
{
  // Configure MCU to enter LLS mode
  SMC_PMPROT = SMC_PMPROT_ALLS_MASK;
  //Clear STOPM first
  SMC_PMCTRL &= ~SMC_PMCTRL_STOPM_MASK;
  SMC_PMCTRL |= SMC_PMCTRL_STOPM(0x3);
}

////////////////////////////////////////////////////////////////////////////////
// VLPS_ENTRY_SETUP
// Configure MCU for VLPS mode entry.
////////////////////////////////////////////////////////////////////////////////
void vlps_entry_setup(void)
{
  // Configure MCU to allow VLPS mode
  SMC_PMPROT = SMC_PMPROT_AVLP_MASK;
  //Clear STOPM first
  SMC_PMCTRL &= ~SMC_PMCTRL_STOPM_MASK;
  SMC_PMCTRL |= SMC_PMCTRL_STOPM(0x2);
}

////////////////////////////////////////////////////////////////////////////////
// STOP_ENTRY_SETUP
// Configure MCU for Stop mode entry.
////////////////////////////////////////////////////////////////////////////////
void stop_entry_setup(void)
{
  // Configure MCU to enter Stop mode
  SMC_PMCTRL =0x00;
}

////////////////////////////////////////////////////////////////////////////////
// change_from_run_to_vlpr_mode
// Force MCU to transition from run to VLPR mode.
////////////////////////////////////////////////////////////////////////////////
void change_from_run_to_vlpr_mode(clock_divider *p)
{
  //Change MCG mode from FEI to FBE to BLPE mode (assumes 8 MHz external osc)
  mcg_osc_blpe(0x02);
  //Configure clocks: core/system div 4, bus div 4, Flexbus div 4, Flash div 8
  //SIM_CLKDIV1 = 0x33370000;
  set_SIM_CLKDIV1(p);
  //Configure MCU to allow VLPR mode
  SMC_PMPROT = SMC_PMPROT_AVLP_MASK;
  //Set RUNM to 10b to enter VLPR
  SMC_PMCTRL &= ~SMC_PMCTRL_RUNM_MASK;
  SMC_PMCTRL |= SMC_PMCTRL_RUNM(0x2);
  //check status bit for correct mode indicator
  while (SMC_PMSTAT & 0x04 != 0x04);
  //Wait for REGON to clear to indicate that MCU is in non regulation mode
  while ((PMC_REGSC & PMC_REGSC_REGONS_MASK) == PMC_REGSC_REGONS_MASK);
}

////////////////////////////////////////////////////////////////////////////////
// ENTER_LOW_POWER_MODE
// Direct MCU mode to enter low power mode based on PMPROT and PMCTRL settings.
////////////////////////////////////////////////////////////////////////////////
void enter_low_power_mode(void)
{
 //disable all portc and 0-5 on porta
  SetPortACtoDefault();
  //put CM4 into sleepdeep via WFI instruction
  SCB_SCR = SCB_SCR_SLEEPDEEP_MASK;
  asm("wfi");
}

void SetPortACtoDefault(void)
{
  unsigned int sim_scgc5_temp;
  sim_scgc5_temp = SIM_SCGC5;       //save for retore before exit
  
  SIM_SCGC5 |= SIM_SCGC5_PORTA_MASK | SIM_SCGC5_PORTC_MASK;  

  GPIOA_PDDR = 0x00000000; //set as input  
  //disable PTA[0-5]
  PORTA_PCR0=0x0; //pin disable
  PORTA_PCR1=0x0; //pin disable
  PORTA_PCR2=0x0; //pin disable
  PORTA_PCR3=0x0; //pin disable
  PORTA_PCR4=0x0; //pin disable
  PORTA_PCR5=0x0; //pin disable

  GPIOC_PDDR = 0x00000000; //set as input
  //disable PTC[0-12]
  PORTC_PCR0=0x0; //pin disable
  PORTC_PCR1=0x0; //pin disable
  PORTC_PCR2=0x0; //pin disable
  PORTC_PCR3=0x0; //pin disable
  PORTC_PCR4=0x0; //pin disable
  PORTC_PCR5=0x0; //pin disable
  PORTC_PCR6=0x0; //pin disable
  PORTC_PCR7=0x0; //pin disable
  PORTC_PCR8=0x0; //pin disable
  PORTC_PCR9=0x0; //pin disable
  PORTC_PCR10=0x0; //pin disable
  PORTC_PCR11=0x0; //pin disable
  PORTC_PCR12=0x0; //pin disable  
  SIM_SCGC5 = sim_scgc5_temp;   //restore clock gate as previously
}

////////////////////////////////////////////////////////////////////////////////
// MemoryRead
// Inputs: start address and end address
// Read range of memory space specify be the caller
////////////////////////////////////////////////////////////////////////////////
void MemoryRead(uint32_t startAddr, uint32_t endAddr)
{
  volatile uint32_t flash_addr=0;
  uint32_t flash_read_data=0;
  for(flash_addr = startAddr; flash_addr < endAddr; flash_addr+=0x4)
  {
	flash_read_data += (*(volatile uint32_t*) (flash_addr));
   }
}

void sctrim_25Mhz(void)
{
  uint8_t prevTrim;
  uint8_t currTrim;
  uint8_t nextTrim;
  uint8_t State=0;
  uint8_t delta=0;
  uint8_t count=0;

  SIM_SCGC5 |= SIM_SCGC5_PORTC_MASK | SIM_SCGC5_PORTD_MASK; 
  //use 2 inputs from power mode selection case and DUT IN
  PORTD_PCR6=(0|PORTD_PCR6_MUX(1)); //Set PTD6 to GPIO
  PORTD_PCR5=(0|PORTD_PCR5_MUX(1)); //Set PTD5 to GPIO
  PORTD_PCR0=(0|PORTD_PCR0_MUX(1)); //Set PTD0 to GPIO
  
  PORTC_PCR3 |= PORTC_PCR3_MUX(5); //set PTC3 as clock out
  /*
  Selects the clock to output on the CLKOUT pin.
  000 Reserved
  001 Reserved
  010 Flash clock
  011 LPO clock (1 kHz)
  100 IRCLK
  101 RTC 32.768kHz
  110 ERCLK0
  111 Reserved  
  */
  SIM_SOPT2 |= SIM_SOPT2_CLKOUTSEL(2);
  
  while (count < 20){
    switch (State) //default:0
    {
      case 0: // start here
//        prevTrim = MCG_C3;
//        prevTrim = MCG_C3 + 0x20;
        prevTrim = 0xff;
        currTrim = 0x00;
        delta = prevTrim - currTrim;
        State = 1;//automatic go to greater state
        break;
      case 1: //meas > target
        delta = prevTrim - currTrim;
        nextTrim = currTrim + (uint8_t)(delta/2);
        MCG_C3 = nextTrim;	//update trim and take freq measurement
        //wait for a pulse on PTD0
        while ((GPIOD_PDIR & 0x00000001) != 0x00000001){};//branch never if PTD0 not 1	
        while ((GPIOD_PDIR & 0x00000001) == 0x00000001){};//branch never if PTD0 not 0    
        if ((GPIOD_PDIR & 0x00000020) != 0x00000020){	//if PTD5=0 mean meas<target (change state), 1 mean meas>target (stay put)
                State = 2;
                prevTrim = currTrim;
                currTrim = nextTrim;
        }else{
                State = 1;
                currTrim = nextTrim;
        }
        if ((GPIOD_PDIR & 0x00000040) == 0x00000040){	//if PTD6=1 (success) go to exit
                State = 3;
        }
        count++;
        break;
      case 2: //meas < target
        delta = currTrim - prevTrim;
        nextTrim = prevTrim + (uint8_t)(delta/2);
        MCG_C3 = nextTrim;	//update trim and take freq measurement
        //wait for a pulse on PTD0
        while ((GPIOD_PDIR & 0x00000001) != 0x00000001){};//branch never if PTD0 not 1	
        while ((GPIOD_PDIR & 0x00000001) == 0x00000001){};//branch never if PTD0 not 0    
        if ((GPIOD_PDIR & 0x00000020) == 0x00000020){	//if PTD5=0 mean meas<target (stay put), 1 mean meas>target (change state)
                State = 1;
                prevTrim = currTrim;
                currTrim = nextTrim;
        }else{
                State = 2;
                currTrim = nextTrim;
        }
        if ((GPIOD_PDIR & 0x00000040) == 0x00000040){	//if PTD6=1 (success) go to exit
                State = 3;
        }
        count++;
        break;
      case 3:	//quit
        count = 30;	//set above the limit for while statement to exit.
        break;
      default:
        State=0;
        break;
    }
                  
  }
}

void set_SIM_CLKDIV1(clock_divider *p)
{
  //update core/sys, peripheral,flexbus,flash clock dividers
  SIM_CLKDIV1 =                0
                               | SIM_CLKDIV1_OUTDIV1(p->CORE_SYS)
//not in l2k                   | SIM_CLKDIV1_OUTDIV2(p->PERIPHERAL)
//not in l2k                   | SIM_CLKDIV1_OUTDIV3(p->FLEXBUS)
                               | SIM_CLKDIV1_OUTDIV4(p->FLASH);    
}
__ramfunc void cache_disable(void)
{  
//kn todo  FMC_PFB0CR = FMC_PFB0CR & 0xFFFFFFE0;
//  FMC_PFB1CR = FMC_PFB1CR & 0xFFFFFFE0;
  /*
  Code will compile when running out of RAM.  This is part of the project option
  define.
  */  
  #ifdef DISABLE_FLASH
  SIM_FCFG1 |= SIM_FCFG1_FLASHDIS_MASK;  
  #endif  
  

}

__ramfunc void cache_enable(void)
{  
//kn todo  FMC_PFB0CR = FMC_PFB0CR | 0x00F8001F;
//  FMC_PFB1CR = FMC_PFB1CR | 0x0000001F;
  /*
  Code will compile when running out of RAM.  This is part of the project option
  define.
  */  
  #ifdef DISABLE_FLASH
  SIM_FCFG1 |= SIM_FCFG1_FLASHDIS_MASK;  
  #endif  
  
}
////////////////////////////////////////////////////////////////////////////////
// MCG_OSC_FBE
// Inputs: Freqency range (valid values are 0, 1, 2 or 3 (2 and 3 translates
// to same range
// Configures frequency accordingly and changes MCG mode from FEI to FBE.
////////////////////////////////////////////////////////////////////////////////

void mcg_osc_fbe(uint8_t range)
{
  mcg_config_freq_range(range);
  mcg_config_fbe_mode();
}
////////////////////////////////////////////////////////////////////////////////
// MCG_OSC_FEE
// Inputs: DCO range (valid values are 0, 1, 2 or 3 
// Configures frequency accordingly and changes MCG mode from FEI to FEE.
////////////////////////////////////////////////////////////////////////////////

void mcg_osc_fee(uint8_t dco)
{ 
  //Assume 10Mhz Ext clk, 25,50,75,100
  mcg_config_freq_range(FREQ_8MHZ_32MHZ);
  mcg_config_fee_mode(dco);
}
////////////////////////////////////////////////////////////////////////////////
// MCG_CONFIG_FBE_MODE
// Forces MCG mode to change from FEI mode to FBE mode.
////////////////////////////////////////////////////////////////////////////////
void mcg_config_fbe_mode(void)
{
// Transition to FBE mode
// First enable external oscillator, RANGE=1, HGO=0, EREFS=0, LP=0, IRCS=0
//  MCG_C2 = MCG_C2_RANGE(1) | MCG_C2_HGO_MASK | MCG_C2_EREFS_MASK;
//    MCG_C2 = MCG_C2_RANGE(1);
    
// Select Clock Mode and Reference Divider and clear IREFS to start ext osc
// CLKS=2, FRDIV=2, IREFS=0, IRCLKEN=0, IREFSTEN=0
  MCG_C1 = MCG_C1_CLKS(2) | MCG_C1_FRDIV(2);
 
  //while (!(MCG_S & MCG_S_OSCINIT_MASK)){};  // wait for oscillator to initialize 

  while (MCG_S & MCG_S_IREFST_MASK){}; //wait for Reference Status bit to update

  // Wait for clock status bits to update
  while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x2){};
}
////////////////////////////////////////////////////////////////////////////////
// MCG_CONFIG_FBE_MODE
// Forces MCG mode to change from FEI mode to FEE mode.
////////////////////////////////////////////////////////////////////////////////
void mcg_config_fee_mode(uint8_t dco)
{
// Transition to FEE mode
    
// Select Clock Mode and Reference Divider and clear IREFS to start ext osc
// CLKS=0, FRDIV=3, IREFS=0, IRCLKEN=0, IREFSTEN=0
  MCG_C1 = MCG_C1_CLKS(0) | MCG_C1_FRDIV(3);  //Assume 10Mhz Ext Ref
 
  //while (!(MCG_S & MCG_S_OSCINIT_MASK)){};  // wait for oscillator to initialize 

  while (MCG_S & MCG_S_IREFST_MASK){}; //wait for Reference Status bit to update

  // Wait for clock status bits to update
  while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x0){};
  
  MCG_C4 |= MCG_C4_DRST_DRS(dco);
}

/*
Trap DUT code here so that HSDIO can go thru its reset sequence
to prepare for DUT POR otherwise result in improper POR sequence.
*/
void wait_hsdio_reset (void)
{
  SIM_SCGC5 |= SIM_SCGC5_PORTD_MASK; 
  PORTD_PCR0=(0|PORTD_PCR0_MUX(1)); //Set PTD0 to GPIO
  //wait for a pulse on PTC0
  while ((GPIOD_PDIR & 0x00000001) != 0x00000001){};//branch never if PTD0 not 1	
  while ((GPIOD_PDIR & 0x00000001) == 0x00000001){};//branch never if PTD0 not 0            
}
void pll_initb(uint8_t prdiv, uint8_t vdiv)
{

// Ensure MCG_C6 is at the reset default of 0. LOLIE disabled, PLL disabled, clk monitor disabled, PLL VCO divider is clear
  MCG_C6 = 0x0;
// Configure PLL Ref Divider, PLLCLKEN=0, PLLSTEN=0, PRDIV=5  
// The crystal frequency is used to select the PRDIV value. Only even frequency crystals are supported
// that will produce a 2MHz reference clock to the PLL.
  MCG_C5 = MCG_C5_PRDIV0(prdiv); // Set PLL ref divider to match the crystal used

  MCG_C6 = MCG_C6_PLLS_MASK | MCG_C6_VDIV0(vdiv); //VDIV = 26 (x50)
  while (!(MCG_S & MCG_S_PLLST_MASK)){}; // wait for PLL status bit to set 
  while (!(MCG_S & MCG_S_LOCK0_MASK)){}; // Wait for LOCK bit to set 
  MCG_C1 &= ~MCG_C1_CLKS_MASK;
  while (((MCG_S & MCG_S_CLKST_MASK) >> MCG_S_CLKST_SHIFT) != 0x3){};
}