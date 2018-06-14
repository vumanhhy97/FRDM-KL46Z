/****************************************************************************************************/
/*
\file       LCD.h
\author     Freescale Semiconductor
\author     Technical Information Center (TIC)
\version    1.0
\date      September 2011
 // LCD call samples
 //  vfnLCD_Home();
 //  vfnLCD_Write_Char ('0');
 //  vfnLCD_All_Segments_ON();
 //  vfnLCD_All_Segments_OFF();
 //  vfnLCD_Write_Msg("@@@@@@@@@");  // TURN ON all characters
 //  vfnLCD_All_Segments_OFF();
*/
/****************************************************************************************************/
/* Services performed by FREESCALE in this matter are performed AS IS and without any warranty.		*/
/* CUSTOMER retains the final decision relative to the total design and functionality of the end	*/
/* product.                                                                                     	*/
/* FREESCALE neither guarantees nor will be held liable by CUSTOMER for the success of this project.*/
/*                                                                                                  */
/* FREESCALE disclaims all warranties, express, implied or statutory including, but not limited to, */
/* implied warranty of merchantability or fitness for a particular purpose on any hardware,         */
/* software ore advise supplied to the project by FREESCALE, and or any product resulting from      */
/* FREESCALE services.                                                                              */
/* In no event shall FREESCALE be liable for incidental or consequential damages arising out of     */
/* this agreement. CUSTOMER agrees to hold FREESCALE harmless against any and all claims demands or */
/* actions by anyone on account of any damage,or injury, whether commercial, contractual, or        */
/* tortuous, rising directly or indirectly as a result of the advise or assistance supplied CUSTOMER*/ 
/* in connectionwith product, services or goods supplied under this Agreement.                      */
/*                                                                                                  */
/****************************************************************************************************/


/*****************************************************************************************************
* Include files
*****************************************************************************************************/
#include "common.h"
#include "LCD_HAL.h"   // indicates how LCD module is configured and what type of LCD is going to be used 7seg, 14-seg, 16 seg, DotMatrix etc

/** Variable types and common definitions */

#define LCD_ALTERNATE_MODE    4   // Write message to Alternate Backplanes  Only when BACKPLANES < 5
#define LCD_NORMAL_MODE       0   // Write message to Original BackPlanes 

extern const uint32 *LCD_TO_PORT[]; //This table relates LCD_Px to PORT Control Pin 


/* variables */

#if _LCDINTENABLE == 1
 //extern void vfnLcdInterruptCallBack(void);
#endif

#if _LCDFDCINTENABLE ==1
 //extern void vfnLcdFDInterruptCallBack(void);
#endif

/*||||||||||||||||||||||       vfnLCD_Init            ||||||||||||||||||||||
brief:  Initialize all the registers on the mcu module 
param:  void                            return:  void    
*/  
void vfnLCD_Init  (void);



void vfnLCD_Home (void);
 /*||||||||||||||||||||||       vfnLCD_Write_Char   ||||||||||||||||||||||
 brief:  Writes on char after the last character was write
 param:  uint8 --- Ascii to write                            return:  void    
*/
void vfnLCD_Write_Char (uint8 lbValue);



/*
   Send a message until a end of char or max number of characters
   if the message size is greater than LCD character the message is cut ad the size of LCD Characters
   If the message lenght is minor than Display character is filled with Blanks
 */
void  vfnLCD_Write_Msg (char *lbpMessage);


 /*||||||||||||||||||||||       vfnLCD_Scroll    ||||||||||||||||||||||
brief:  Moves the message on the LCD 
param:  uint8 pointer --- the first character  on the array to write
param: uint8 --- the size of the character  
return:  void    
*/

void vfnLCD_Scroll (uint8 *lbpMessage,uint8 lbSize);

 /*||||||||||||||||||||||       vfnLCD_Write_Msg    ||||||||||||||||||||||
brief:  Writes on message on the LCD   if the message is longer than the number of character  is send to the Scroll function
param:  uint8 pointer --- the first character  on the array to write
param: uint8 --- the numbers of characterts to write      
return:  void    
*/
void  vfnLCD_Write_MsgPLace (uint8 *lbpMessage, uint8 lbSize );

 /*||||||||||||||||||||||       vfnLCD_Write_Msg_GoTo    ||||||||||||||||||||||
brief:  Writes on message on the LCD  in a specific location
param: int8 pointer --- array of characters to write
param: uint8 --- the numbers of characterts to write 
param: uint8 --- location to write
return:  void    
*/
void  vfnLCD_Write_Msg_GoTo (int8 *lbpMessage, uint8 lbSize, uint8 lbStartWrite);



void vfnLCD_Scroll (uint8 *lbpMessage,uint8 lbSize);
 
/*||||||||||||||||||||||       vfnLCD_All_Segments_ON   ||||||||||||||||||||||
brief: Turns on all the segments on the LCD
param: none
param: none
return:  void    
*/
void vfnLCD_Set_Display (void);//vfnLCD_All_Segments_ON

/*||||||||||||||||||||||       vfnLCD_All_Segments_ON   ||||||||||||||||||||||
brief: Turns oFF all the segments on the LCD
param:  none
param:  none
return:  void    
*/
void vfnLCD_Clear_Display(void); 


void vfnlcd_pinmux(uint8  mux_val);  //only 0 or 7 is allowed
void lcd_fault_detect_init(void);
void vfnBP_VScroll(int8 scroll_count);
void vfnLCD_Contrast (uint8 lbContrast);



/* functions for DOT matrix LCD panel only*/
void vfnLCD_All_Segments_Char (uint8 val);
void PutPoint(uint8 x, uint8 y);
void ClrPoint(uint8 x, uint8 y);
void SetX(uint8 x, uint8 value);


/*
 LCD.c functions
*/

//void LCD_vector_interrupt_init(void);
void vfnLCD_EnablePins(void);  // Pin Enable/ BPEN / COM configurations
void vfnLCD_ConfigureBackplanes(void);
void vfnBP_VScroll(int8 scroll_count); // chage Backplane configuration( Rotates Backplanes scroll_count units)
//void vfnLCD_interrupt_init(void);
void vfnLCD_isrv(void);

/* kinetis specific fucntions */
void vfnlcd_pinmux(uint8  mux_val);  // Configure LCD Port pins to 7 or 0 used for fault detection
void vfnLCD_Write_Char (uint8 lbValue);  // This function is dependent on the LCD panel

extern uint8 lcd_alternate_mode;




/*LCD Fault Detections Consts*/
#define  FP_TYPE  0x0         // pin is a Front Plane
#define  BP_TYPE  0x1         // pin is Back Plane


/*||||||||||||||||||||||       lcd_measure_pin_cap   ||||||||||||||||||||||
brief: measure capacitance of the pin  (pulling method)
param:  pinid             (LCDWF number  0-47)
        pin_type           FP_TYPE=0 or BP_TYPE =1
        fdprs_val         Fault detect preescaler value  0-7
        fdsww_val         Fault detect windows width     0-7
param: none
return:  capacitance value  (FDSR_FDCNT)   
*/
uint8 u8fnlcd_measure_pin_cap(uint8 pinid, uint8 pin_type, uint8 fdprs_val, uint8 fdsww_val);


void vfnLCD_disable_module();
void vfnLCD_enable_module();
