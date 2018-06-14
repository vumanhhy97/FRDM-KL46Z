#include "common.h"
#include "prelim_ftfl.h"


//These variables are used by the run_ccob_cmd() function to support
//interrupt handling.
static uint8_t ftfl_enable_interrupts_flag = FALSE;
static uint8_t ftfl_interrupt_mask = 0;

//A callback function which get's called if a CCOB command is taking a long time to run
//and interrupts are disabled.
FTFL_CALLBACK_TYPE ftfl_callback_function = NULL;


//Function prototyps used only by functions within this file.
uint8_t ftfl_run_ccob_cmd(uint8_t command[], uint16_t count);


//Setting up a callback function is purely optional.   If you don't set up the callback
//function, when a CCOB command executes it will simply wait forever until the command is
//complete.   With a callback function, the callback will get called while waiting.
//If interrupts are enabled, the callback function does not get called.   The interrupt
//handler will be executed.
void ftfl_set_callback_function( FTFL_CALLBACK_TYPE callback_func)
{
	ftfl_callback_function = callback_func;
}

//Setup static global variables so that the run_ccob_command function will enable interrupts
//after it starts a command running.
void ftfl_enable_interrupts( uint8_t set_ccie, uint8_t set_rdcollie)
{
    ftfl_enable_interrupts_flag = TRUE;    
    ftfl_interrupt_mask = (set_ccie << FTFL_FCNFG_CCIE_SHIFT ) | (set_rdcollie << FTFL_FCNFG_RDCOLLIE_SHIFT );    
}

//Setup static global variables to stop the run_ccob_command function from enabling interrupts after
//it starts a command running.
void ftfl_disable_interrupts( void )
{
	//Make sure the interrupt enable bits are cleared to 0.
    FTFL_FCNFG = FTFL_FCNFG & ~(FTFL_FCNFG_CCIE_MASK | FTFL_FCNFG_RDCOLLIE_MASK);
    ftfl_enable_interrupts_flag = FALSE;
    ftfl_interrupt_mask = 0;
}


//All ccob commands are executed by calling this function.
__ramfunc uint8_t ftfl_run_ccob_cmd(uint8_t command[], uint16_t count)
{
  uint8_t ret_val = 0;
  uint8_t idx;
  uint16_t cmd[12];

  //Clear any previous errors.   Some errors prevent a new command from running.
  FTFL_FSTAT = FTFL_FSTAT_FPVIOL_MASK | FTFL_FSTAT_ACCERR_MASK | FTFL_FSTAT_RDCOLERR_MASK;
  
  //To make the memory map in the debugger cleaner, set all values to 0.  This is optional.
  for(idx = 0; idx < 12; idx++) 
  {
    cmd[idx] = 0;
  } 
   
  //Copy the command into the FTFL module's CCOB registers.
  for(idx = 0; idx < count; idx++) 
  {
    cmd[idx] = command[idx];
  }  
  FTFL_FCCOB0 = cmd[0];  
  FTFL_FCCOB1 = cmd[1];
  FTFL_FCCOB2 = cmd[2];
  FTFL_FCCOB3 = cmd[3];
  FTFL_FCCOB4 = cmd[4];
  FTFL_FCCOB5 = cmd[5];
  FTFL_FCCOB6 = cmd[6];
  FTFL_FCCOB7 = cmd[7];
  FTFL_FCCOB8 = cmd[8];
  FTFL_FCCOB9 = cmd[9];
  FTFL_FCCOBA = cmd[10];  
  FTFL_FCCOBB = cmd[11];
  
  //Run command
  FTFL_FSTAT = FTFL_FSTAT_CCIF_MASK;

  if (ftfl_enable_interrupts_flag)
  {
        FTFL_FCNFG = FTFL_FCNFG | ftfl_interrupt_mask;
  }

  //If the command complete interrupt is not enabled,
//  if (!(ftfl_interrupt_mask & FTFL_FCNFG_CCIE_MASK))
  {
      //wait for command to complete
      while(( FTFL_FSTAT & FTFL_FSTAT_CCIF_MASK) == 0) 
      {
          //While waiting, should a callback function be called?
	      if (ftfl_callback_function != NULL)
	      {
		      ftfl_callback_function();
	      }
      } 
     
      ret_val = ftfl_check_for_fstat_errors();
  }
  
  // Return FSTAT when complete or 0 if interrupts are enabled
  return(ret_val);	
}
       

//Return fstat with all bits but the error bits masked off.
//Errors bits the calling routine should look out for are:
//
//FTFL_FSTAT_MGSTAT0_MASK  - a lot of commands use this to indicate pass/fail status.
//FTFL_FSTAT_ACCERR_MASK   - usually indicates an invalid command, address or parameter was used.
//FTFL_FSTAT_RDCOLERR_MASK - Accessing the FLEXNVM while erasing the FLEXNVM is one example of this.
//FTFL_FSTAT_FPVIOL_MASK   - protection violation.
//
__ramfunc uint8_t ftfl_check_for_fstat_errors(void)
{
  uint8_t ret_val;
      
  ret_val = FTFL_FSTAT;
  ret_val &= (FTFL_FSTAT_FPVIOL_MASK | FTFL_FSTAT_ACCERR_MASK | 
                          FTFL_FSTAT_MGSTAT0_MASK | FTFL_FSTAT_RDCOLERR_MASK);
         
  return ret_val;
}

//All CCOB address fields are only 23 bits long.   Only the low order address
//bits are used to identify the data.   Bit 23 selects the pflash (0) or the
//FLEXNVM(1).
uint32_t ftfl_convert_addr_to_ccob_addr(uint32_t addr) 
{
     //If address is in the data flash block(s).
     if (addr >= FTFL_FLEXNVM_START_ADDR) 
     {
          addr = (addr ^ FTFL_FLEXNVM_START_ADDR) | FTFL_FLEXNVM_CCOB_START_ADDR;
     }
     
     return addr;
}

//Suspend erase sector.   TBD -- Not clear whether ERSBLK can be suspended from the spec...
//Spec doesn't refer to it but seems like it should be...
uint8_t suspend_erase(uint8_t *suspended)
{
    uint8_t ret_val;
    *suspended = FALSE;
  
    //Request to suspend.  If the erase completes
    //before it is suspended, this bit is cleared.
    FTFL_FCNFG |= FTFL_FCNFG_ERSSUSP_MASK;
    
    //Waiting on suspend.
    while( (FTFL_FSTAT & FTFL_FSTAT_CCIF_MASK) == 0)
    {
        //While waiting, should a callback function be called?
	    if (ftfl_callback_function != NULL)
	    {
		    ftfl_callback_function();
	    }	    
    }
    
    if (FTFL_FCNFG & FTFL_FCNFG_ERSSUSP_MASK)
    {
        *suspended = TRUE;
    }
    else
    {
        //erase completed before being suspended
    }
    
   //returns 0 for pass or fstat error bits for fail.
   ret_val = ftfl_check_for_fstat_errors();

   return ret_val;
}

//Resume erasing.
uint8_t resume_erase(void)
{
  uint8_t ret_val;

  if (FTFL_FCNFG & FTFL_FCNFG_ERSSUSP_MASK)
  {
     //Resume erase command
     FTFL_FSTAT = FTFL_FSTAT_CCIF_MASK;
   }
     
     //wait for command to complete
     while(( FTFL_FSTAT & FTFL_FSTAT_CCIF_MASK) == 0) 
     {
        //While waiting, should a callback function be called?
	    if (ftfl_callback_function != NULL)
	    {
	     ftfl_callback_function();
	    }
   } 
     
   //returns 0 for pass or fstat error bits for fail.
   ret_val = ftfl_check_for_fstat_errors();

   return ret_val;
}


//abort erasing.
uint8_t abort_erase(void)
{
  uint8_t ret_val;
  
  if (FTFL_FCNFG & FTFL_FCNFG_ERSSUSP_MASK)
  {
      //The next time a command is executed, the erase
      //operation that is currently suspended will be 
      //aborted and the next command will be executed.
      FTFL_FCNFG = FTFL_FCNFG & ~FTFL_FCNFG_ERSSUSP_MASK;
  }

  //returns 0 for pass or fstat error bits for fail.
  ret_val = ftfl_check_for_fstat_errors();

  return ret_val;
}



//The Read 1s Block command.  Checks to see if an entire program flash or data flash logical
//block has been erased to the specified margin level. The FCCOB flash address bits
//determine which logical block is erase-verified.   Remember that bit 23 indicates whether
//the block will be a pflash (0) or FLEXNVM block (1).
//
//addr must be 128 bit aligned.
//
//read_1_margin_choices:
//	- FTFL_NORMAL_READ_LEVEL    
//	- FTFL_USER_MARGIN_LEVEL    
//	- FTFL_FACTORY_MARGIN_LEVEL 
uint8_t FTFL_RD1BLK(uint32_t addr, uint8_t read_1_margin_choice) 
{
   uint8_t cmd_ary[12];
   uint8_t ret_val;
     
   //Don't forget to do this -> 
   addr = ftfl_convert_addr_to_ccob_addr(addr); //if you
   //are using a pflash/FLEXNVM part.     
     
   cmd_ary[0] = FTFL_RD1BLK_CMD;
   cmd_ary[1] = bits_23_16(addr);
   cmd_ary[2] = bits_15_8(addr);
   cmd_ary[3] = bits_7_0(addr);
   cmd_ary[4] = read_1_margin_choice;

   ret_val = ftfl_run_ccob_cmd(cmd_ary, 5);        

   //returns 0 for pass or fstat error bits for fail.
   return ret_val;
}


//Read 1s Section.   checks if a section of program flash or data flash memory
//is erased to the specified read margin level. The Read 1s Section command defines the
//starting address and the number of 128 bits to be verified.
//
//addr must be 128 bit aligned.
//
//read_1_margin_choices:
//	- FTFL_NORMAL_READ_LEVEL    
//	- FTFL_USER_MARGIN_LEVEL    
//	- FTFL_FACTORY_MARGIN_LEVEL 
uint8_t FTFL_RD1SEC(uint32_t addr, uint16_t num_of_128_bits_to_be_verified, uint8_t read_1_margin_choice) 
{
    uint8_t cmd_ary[12];
    uint8_t ret_val;
     
   //Don't forget to do this -> 
   addr = ftfl_convert_addr_to_ccob_addr(addr); //if you
   //are using a pflash/FLEXNVM part.     
     
    cmd_ary[0] = FTFL_RD1SEC_CMD;
    cmd_ary[1] = bits_23_16(addr);
    cmd_ary[2] = bits_15_8(addr);
    cmd_ary[3] = bits_7_0(addr);
     
    cmd_ary[4] = bits_15_8(num_of_128_bits_to_be_verified);
    cmd_ary[5] = bits_7_0(num_of_128_bits_to_be_verified);
     
    cmd_ary[6] = read_1_margin_choice;

    ret_val = ftfl_run_ccob_cmd(cmd_ary, 7);        

    //returns 0 for pass or fstat error bits for fail.
    return ret_val;
}


//Program Check.   Tests a previously programmed program flash or data flash longword to see 
//if it reads correctly at the specified margin level.
//
//addr must be 32 bit aligned.
//
//read_1_margin_choices:
//	- FTFL_NORMAL_READ_LEVEL    
//	- FTFL_USER_MARGIN_LEVEL    
//	- FTFL_FACTORY_MARGIN_LEVEL 
uint8_t FTFL_PGMCHK(uint32_t addr, uint32_t expected_data, uint8_t read_1_margin_choice) 
{
    uint8_t cmd_ary[12];
    uint8_t ret_val;
     
   //Don't forget to do this -> 
   addr = ftfl_convert_addr_to_ccob_addr(addr); //if you
   //are using a pflash/FLEXNVM part.     
     
    cmd_ary[0] = FTFL_PGMCHK_CMD;
    cmd_ary[1] = bits_23_16(addr);
    cmd_ary[2] = bits_15_8(addr);
    cmd_ary[3] = bits_7_0(addr);
    
    cmd_ary[4] = read_1_margin_choice;

    cmd_ary[8]   = bits_31_24(expected_data);
    cmd_ary[9]   = bits_23_16(expected_data);
    cmd_ary[0xa] = bits_15_8(expected_data);
    cmd_ary[0xb] = bits_7_0(expected_data);
         
    ret_val = ftfl_run_ccob_cmd(cmd_ary, 12);        

    //returns 0 for pass or fstat error bits for fail.
    return ret_val;
}

//Read Resource Command.  Provided for the user to read data from special-purpose
//memory resources located within the Flash module.
//
//addr must be 64 bit aligned.
//resource_select_code = FTFL_IFR_RSRC or FTFL_VERSION_ID_RSRC
uint8_t FTFL_RDRSRC(uint32_t addr, uint8_t resource_select_code, uint32_t *data) 
{
   uint8_t cmd_ary[12];
   uint32_t ret_val;
   uint32_t data0;
     
   //initialize return variables.
   *data = 0;
          
   //Don't forget to do this -> 
   addr = ftfl_convert_addr_to_ccob_addr(addr); //if you
   //are using a pflash/FLEXNVM part.     
     
   cmd_ary[0] = FTFL_RDRSRC_CMD;
   cmd_ary[1] = bits_23_16(addr);
   cmd_ary[2] = bits_15_8(addr);
   cmd_ary[3] = bits_7_0(addr);

   cmd_ary[8] = resource_select_code;
     
   ret_val = ftfl_run_ccob_cmd(cmd_ary, 9);        

   if (ret_val == 0)
   {
   	//Read the data out of the CCOB's
   	data0 = (FTFL_FCCOB4 << 24) + (FTFL_FCCOB5 << 16) + (FTFL_FCCOB6 << 8) + FTFL_FCCOB7;

   	*data = data0;
   }
        
   //returns 0 for pass or fstat error bits for fail.
   return ret_val;
}


//Program Phrase.  Programs eight previously-erased bytes in the program
//flash memory or in the flexNVM flash memory using an embedded algorithm.
//
//addr must be 32 bit aligned.
//
uint8_t FTFL_PGM4(uint32_t addr, uint32_t data) 
{
   uint8_t cmd_ary[12];
   uint8_t ret_val;
    
   //Don't forget to do this -> 
   addr = ftfl_convert_addr_to_ccob_addr(addr); //if you
   //are using a pflash/FLEXNVM part.     
 
   cmd_ary[0] = FTFL_PGM4_CMD;
   cmd_ary[1] = bits_23_16(addr);
   cmd_ary[2] = bits_15_8(addr);
   cmd_ary[3] = bits_7_0(addr);
                                      
   cmd_ary[4] = bits_31_24(data);     
   cmd_ary[5] = bits_23_16(data);     
   cmd_ary[6] = bits_15_8(data);      
   cmd_ary[7] = bits_7_0(data);       

   cmd_ary[8]   = 0;     
   cmd_ary[9]   = 0;     
   cmd_ary[0xa] = 0;      
   cmd_ary[0xb] = 0;       

   ret_val = ftfl_run_ccob_cmd(cmd_ary, 12);
    
   //returns 0 for pass or fstat error bits for fail.
   return ret_val;
}

//Erase Flash Block.  Erases all addresses in a single program flash or flexNVM
//flash block.
//
//addr must be 128 bit aligned.
//
uint8_t FTFL_ERSBLK( uint32_t addr) 
{
   uint8_t cmd_ary[12];
   uint8_t ret_val;
     
   //Don't forget to do this -> 
   addr = ftfl_convert_addr_to_ccob_addr(addr); //if you
   //are using a pflash/FLEXNVM part.     
     
   cmd_ary[0] = FTFL_ERSBLK_CMD;
   cmd_ary[1] = bits_23_16(addr);
   cmd_ary[2] = bits_15_8(addr);
   cmd_ary[3] = bits_7_0(addr);

   ret_val = ftfl_run_ccob_cmd(cmd_ary, 4);

   //returns 0 for pass or fstat error bits for fail.
   return ret_val;     
}

//Erase Flash Sector.  Erases all 4K byte addresses in a flash sector.
//
//addr must be 128 bit aligned.
//
uint8_t FTFL_ERSSCR( uint32_t addr) 
{
   uint8_t cmd_ary[12];
   uint8_t ret_val;
     
   //Don't forget to do this -> 
   addr = ftfl_convert_addr_to_ccob_addr(addr); //if you
   //are using a pflash/FLEXNVM part.     
     
   cmd_ary[0] = FTFL_ERSSCR_CMD;
   cmd_ary[1] = bits_23_16(addr);
   cmd_ary[2] = bits_15_8(addr);
   cmd_ary[3] = bits_7_0(addr);

   ret_val = ftfl_run_ccob_cmd(cmd_ary, 4);

   //returns 0 for pass or fstat error bits for fail.
   return ret_val;     
}

//Program section.  Programs the data found in the FlexRAM (lower quarter address only=0 to FFF) to
//flash memory using an embedded algorithm.
//
//addr must be 128 bit aligned.
//
uint8_t FTFL_PGMSEC(uint32_t addr, uint16_t num_of_64_bits_to_program) 
{
    uint8_t cmd_ary[12];
    uint8_t ret_val;
     
   //Don't forget to do this -> 
   addr = ftfl_convert_addr_to_ccob_addr(addr); //if you
   //are using a pflash/FLEXNVM part.     
     
    cmd_ary[0] = FTFL_PGMSEC_CMD;
    cmd_ary[1] = bits_23_16(addr);
    cmd_ary[2] = bits_15_8(addr);
    cmd_ary[3] = bits_7_0(addr);
     
    cmd_ary[4] = bits_15_8(num_of_64_bits_to_program);
    cmd_ary[5] = bits_7_0(num_of_64_bits_to_program);
     
    ret_val = ftfl_run_ccob_cmd(cmd_ary, 6);        

    //returns 0 for pass or fstat error bits for fail.
    return ret_val;
}


//Read 1's all blocks.  Checks if the program flash blocks, data flash blocks,
//EEPROM backup records, and data flash IFR have been erased to the specified read
//margin level, if applicable, and releases security if the readout passes, i.e. all data reads as
//'1'.
uint8_t FTFL_RD1ALL( uint8_t margin_level) 
{
   uint8_t cmd_ary[12];
   uint8_t ret_val;
     
   cmd_ary[0] = FTFL_RD1ALL_CMD;
   cmd_ary[1] = margin_level;
   

   ret_val = ftfl_run_ccob_cmd(cmd_ary, 2);

   //returns 0 for pass or fstat error bits for fail.
   return ret_val;     
}

//Read Once command.  Provides read access to a reserved 64-byte field located in the
//program flash 0 IFR (see Program flash 0 IFR map and Program Once field).
//
//record_index = 0 to 7.
//
uint8_t FTFL_RDONCE(uint8_t record_index, uint32_t *data) 
{
   uint8_t cmd_ary[12];
   uint8_t ret_val;
   uint32_t data0;
    
   *data = 0;
    
   cmd_ary[0] = FTFL_RDONCE_CMD;
   cmd_ary[1] = record_index;
                                      
   ret_val = ftfl_run_ccob_cmd(cmd_ary, 12);

   if (ret_val == 0)
   {
   	//Read the data out of the CCOB's
   	data0 = (FTFL_FCCOB4 << 24) + (FTFL_FCCOB5 << 16) + (FTFL_FCCOB6 << 8) + FTFL_FCCOB7;

   	*data = data0;
   }
       
   //returns 0 for pass or fstat error bits for fail.
   return ret_val;
}

//Program Once command.  enables programming to a reserved 64-byte field in the
//program flash 0 IFR (see Program flash 0 IFR map and Program Once field).
//
//record_index = 0 to 15.
//
uint8_t FTFL_PGMONCE(uint8_t record_index, uint32_t data) 
{
   uint8_t cmd_ary[12];
   uint8_t ret_val;
     
   cmd_ary[0] = FTFL_PGMONCE_CMD;
   cmd_ary[1] = record_index;
                                      
   cmd_ary[4] = bits_31_24(data);     
   cmd_ary[5] = bits_23_16(data);     
   cmd_ary[6] = bits_15_8(data);      
   cmd_ary[7] = bits_7_0(data);            

   ret_val = ftfl_run_ccob_cmd(cmd_ary, 12);
    
   //returns 0 for pass or fstat error bits for fail.
   return ret_val;
}


//Erase all blocks command.  erases all flash memory, initializes the FlexRAM, verifies
//all memory contents, and releases MCU security.
//
uint8_t FTFL_ERSALL(void) 
{
   uint8_t cmd_ary[12];
   uint8_t ret_val;
     
   cmd_ary[0] = FTFL_ERSALL_CMD;

   ret_val = ftfl_run_ccob_cmd(cmd_ary, 1);

   //returns 0 for pass or fstat error bits for fail.
   return ret_val;     
}


//Verify backdoor access key command.  only executes if the mode and security
//conditions are satisfied (see Flash commands by mode). Execution of the Verify
//Backdoor Access Key command is further qualified by the FSEC[KEYEN] bits. The
//Verify Backdoor Access Key command releases security if user-supplied keys in the
//FCCOB match those stored in the Backdoor Comparison Key bytes of the Flash
//Configuration Field.
uint8_t FTFL_VFYKEY(uint32_t most_significant_word, uint32_t least_significant_word) 
{
   uint8_t cmd_ary[12];
   uint8_t ret_val;
     
   cmd_ary[0] = FTFL_VFYKEY_CMD;
                                      
   cmd_ary[4] = bits_31_24(most_significant_word);     
   cmd_ary[5] = bits_23_16(most_significant_word);     
   cmd_ary[6] = bits_15_8(most_significant_word);      
   cmd_ary[7] = bits_7_0(most_significant_word);       

   cmd_ary[8]   = bits_31_24(least_significant_word);     
   cmd_ary[9]   = bits_23_16(least_significant_word);     
   cmd_ary[0xa] = bits_15_8(least_significant_word);      
   cmd_ary[0xb] = bits_7_0(least_significant_word);       

   ret_val = ftfl_run_ccob_cmd(cmd_ary, 12);
    
   //returns 0 for pass or fstat error bits for fail.
   return ret_val;
}


uint8_t FTFL_SWAP( uint32_t addr, uint8_t swap_control_code, uint8_t *current_swap_mode, uint8_t *current_swap_block_status, uint8_t *next_swap_block_status) 
{
   uint8_t cmd_ary[12];
   uint8_t ret_val;

   //Initialize return variables.
   *current_swap_mode = 0xff;
   *current_swap_block_status = 0xff;
   *next_swap_block_status = 0xff;
        
   //Don't forget to do this -> 
   addr = ftfl_convert_addr_to_ccob_addr(addr); //if you
   //are using a pflash/FLEXNVM part.     
     
   cmd_ary[0] = FTFL_SWAP_CMD;
   cmd_ary[1] = bits_23_16(addr);
   cmd_ary[2] = bits_15_8(addr);
   cmd_ary[3] = bits_7_0(addr);
   
   cmd_ary[4] = swap_control_code;

   //Recommended to initialize return codes to ff since an access error will prevent these from getting updated.
   cmd_ary[5] = 0xff;
   cmd_ary[6] = 0xff;
   cmd_ary[7] = 0xff;
   
   ret_val = ftfl_run_ccob_cmd(cmd_ary, 8);

   if (swap_control_code == FTFL_SWAP_REPORT_SWAP_STATUS)
   {
     *current_swap_mode = FTFL_FCCOB5;
     *current_swap_block_status = FTFL_FCCOB6;
     *next_swap_block_status = FTFL_FCCOB6;
   }
   
   //returns 0 for pass or fstat error bits for fail.
   return ret_val;     
}


//Program partition command.  Prepares the FlexNVM block for use as data flash,
//EEPROM backup, or a combination of both and initializes the FlexRAM.
//
uint8_t FTFL_PGMPART(uint8_t eeprom_data_size_code, uint8_t flexnvm_partition_code) 
{
   uint8_t cmd_ary[12];
   uint8_t ret_val;
     
   cmd_ary[0] = FTFL_PGMPART_CMD;                                      
   cmd_ary[4] = eeprom_data_size_code;     
   cmd_ary[5] = flexnvm_partition_code;     

   ret_val = ftfl_run_ccob_cmd(cmd_ary, 6);
    
   //returns 0 for pass or fstat error bits for fail.
   return ret_val;
}

//Set flexRAM command.  changes the function of the FlexRAM:
//?When not partitioned for EEPROM, the FlexRAM is typically used as traditional RAM.
//?When partitioned for EEPROM, the FlexRAM is typically used to store EEPROM data.
//
//values for flexram_function_control_code:
//0xff = set FlexRAM as RAM.
//0x00 = set FlexRAM as EEPROM.
//
uint8_t FTFL_SETRAM(uint8_t flexram_function_control_code) 
{
   uint8_t cmd_ary[12];
   uint8_t ret_val;
     
   cmd_ary[0] = FTFL_SETRAM_CMD;
   cmd_ary[1] = flexram_function_control_code;
                                      
   ret_val = ftfl_run_ccob_cmd(cmd_ary, 2);
    
   //returns 0 for pass or fstat error bits for fail.
   return ret_val;
}


