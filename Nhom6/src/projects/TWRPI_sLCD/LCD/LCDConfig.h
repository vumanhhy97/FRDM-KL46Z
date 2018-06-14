
/*^^^^^      LCD HARDWARE 
 LCDConfig.h
     This file set the user configuration for the LCD, 
          configure interrupt, 
          Fualt detection interrrupt enable
          Number of Frontplanes/Backplanes

****/
#include "common.h"

#include "TWRPI-GD6363P.h"  // Hardware configuration, indicate how LCD pin is connected to the LCD panel, and special symbol definitions





/* USER defined initialization of LCD*/
#define _LCDRVEN          (0)         // Regulator disable 
#define _LCDRVTRIM        (8)         // CPSEL = 1     0 -- 8000 pf 1 -- 6000 pf  2 -- 4000 pf  3 -- 2000 pf



#define _LCDHREF          (0)         // 0 or 1 
#define _LCDIEN           (0)         // LCD interrupts enabled
#define _LCDFDCIEN        (0)         // LCD Fault Detect Complete Interrupt Enabled
#define _LCDALTDIV        (0)         // CPSEL = 1     0 -- 8000 pf 1 -- 6000 pf  2 -- 4000 pf  3 -- 2000 pf
#define _LCDWAIT          (0)         // 1= LCD do not operate in WAIT mode
#define _LCDSTP           (0)         // 1= LCD do not operate in STOP mode
#define _LCDEN            (1)         // 1 = LCD module enable  0 = Disabled
#define _LCDCLKPSL        (1)         // Clock divider to generate the LCD Waveforms 
#define _LCDLCK           (4)         // LCD clock prescaler - Any number between 0 and 7 




//values derivated from previous values
#define _LCDUSEDPINS   (_LCDFRONTPLANES + _LCDBACKPLANES)  // Defined on "TWRPI-GD6363P.h" 
#define _LCDDUTY       (_LCDBACKPLANES-1)                  //

// General definitions used by the LCD driver
#define  LCD_WF_BASE            LCD_WF3TO0  //initial Waveform
#define  SymbolON(LCDn,bit)     *((uint8 *)&LCD_WF_BASE + LCDn)  |=  (1<<(bit))         
#define  SymbolOFF(LCDn,bit)    *((uint8 *)&LCD_WF_BASE + LCDn)  &= ~(1<<(bit))         

// Fault detect Constants
// FD Preescaler
#define FDPRS_1      0
#define FDPRS_2      1
#define FDPRS_4      2
#define FDPRS_8      3
#define FDPRS_16     4 
#define FDPRS_32     5
#define FDPRS_64     6
#define FDPRS_128    7

// Fault Detect Window Width values
#define FDSWW_4           0
#define FDSWW_8           1
#define FDSWW_16          2
#define FDSWW_32          3
#define FDSWW_64          4
#define FDSWW_128         5
#define FDSWW_256         6
#define FDSWW_512         


/*{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{        Functions        }}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}*/
/*{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{{  ---------------------  }}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}}*/


/*||||||||||||||||||||||       vfnLCD_EnablePins   ||||||||||||||||||||
brief: Enable all the pins of the mcu directly connected to the  LCD on use   
param:  void                            return:  void    
*/  
void vfnLCD_EnablePins (void);

/*||||||||||||||||||||||       vfnLCD_ConfigureBackplanes ||||||||||||||
brief:  Set as a backplane and give the number of COM that corresponds  
param:  void                            return:  void    
*/
void vfnLCD_ConfigureBackplanes (void);  //

/*||||||||||||||||||||||       vfnLCD_Home            ||||||||||||||||||||||
brief:  Reset the counter to the first position
param:  void
return:  void    
*/  
