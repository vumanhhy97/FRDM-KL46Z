/****************************************************************************************************/
/*
\file       LCD_HAL.h
\author     Freescale Semiconductor
\author     Technical Information Center (TIC)
\version    1.0
\date       Sep, 2011
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
* Module definition against multiple inclusion
*****************************************************************************************************/

#include "common.h"

/* Configuration for  TWRPI-GD6363p.h*/
#define _LCDFRONTPLANES   (7)          // # of frontPlanes  
#define _LCDBACKPLANES    (4)           // # of backplanes
/*
   LCD logical organization definition
    This section indicate how the LCD is distributed  how many characteres of (7-seg, 14,seg, 16 seg, or colums in case of Dot Matrix) does it contain
   First character is forced only one can be written
*/
#define _CHARNUM     (4)  //number of Character (7seg, 14-seg, 16-seg, Columns(dot matrix)) present in the LCD
#define _CHAR_SIZE   (2)  // Inidicate How many LCD_Wfs are used to print a single Character or a column in case of Dot Matriz LCD
#define _LCDTYPE     (2)  //indicate how many LCD_WF are required to write a single Character / or Colum in case of DOT matrix LCD

/* indicate if interrupts are in use */
#define _LCDINTENABLE     0         //***MGR clear - sLCD frame interrupt enabled
#define _LCDFDCINTENABLE  0         //LCD Fault detection complete enable
#define _LCDCPSEL         (1)         // 0 or 1 
#define _LCDCLKSOURCE     (1)         //***MGR 0 -- External clock  1 --  Alternate clock
#define _LCDALRCLKSOURCE  (0)         // 0 -- External clock       1 --  Alternate clock

#define _LCDSUPPLY        (1)         // 0 Divide input VIREG=1.0v     1 Do not divide the input VIREG=1.67v
#define _LCDLOADADJUST    (3)         // CPSEL = 1     0 -- 8000 pf 1 -- 6000 pf  2 -- 4000 pf  3 -- 2000 pf
/* LCD_AR initialization */
#define _LCDBLINKRATE     (3)         //Any number between 0 and 7 

#define vfnLCD_GoTo(Place)	gu8LCD_CharPosition =(UINT8)((Place-1)*_LCDTYPE)  //gu8LCD_CharPosition  //bLCD_CharPosition



#include "TWRPI-GD6363P.h"
#include "LCDConfig.h"    // indicates how LCD module is configured and what type of LCD is going to be used 7seg, 14-seg, 16 seg, DotMatrix etc
