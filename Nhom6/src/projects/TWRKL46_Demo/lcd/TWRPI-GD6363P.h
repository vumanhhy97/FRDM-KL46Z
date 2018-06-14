/*^^^^^^^^^^^^      LCD HARDWARE 
 
    TWRPI-GD6363P.h   

    TWRPI_SLCD
         3x 7SegLCD + special symbols
        In this case LCD is configured as 4 x 7SegLCD and only fir character is written when.
****/
#include "common.h"


/*
  Following definitons indicate how Charaters are associated to LCD_WFx
*/

/*
 Hardware configuration  
 Indicate what LCD_Px is connected to a Logical Character,  this definitions are used in  WF_ORDERING_TABLE[ ];
*/
#define   CHAR_1A        22  // LCD Pin 10
#define   CHAR_1F        24  // LCD Pin 11
#define   CHAR_2A        3  // LCD Pin 8
#define   CHAR_2F        20  // LCD Pin 9
#define   CHAR_3A        23  // LCD Pin 5
#define   CHAR_3F        1  // LCD Pin 7
#define   CHAR_S1        0  // LCD Pin 6
#define   CHARCOM0       12  // LCD Pin 1   shared with Electrode 1 TWR-RevB
#define   CHARCOM1       13  // LCD Pin 2   shared with Electrode 2 TWR-RevB
#define   CHARCOM2       14  // LCD Pin 3
#define   CHARCOM3       15  // LCD Pin 4


/*
  Special symbols definitions
    Indicate which FrontPlane and What bit Com is associated with the special symbol
*/
#define SPECIAL_SYMBOL_COUNT 7

#define   _FREESCALE_ON()   SymbolON(CHAR_1F,3)
#define   _ONE_ON()         SymbolON(CHAR_3F,3)
#define   _COL_ON()         SymbolON(CHAR_2F,3)
#define   _GRADE_ON()       SymbolON(CHAR_S1,0)
#define   _PERCENTAGE_ON()  SymbolON(CHAR_S1,1)
#define   _AM_ON()          SymbolON(CHAR_S1,2)
#define   _PM_ON()          SymbolON(CHAR_S1,3)

#define   _FREESCALE_OFF()  SymbolOFF(CHAR_1F,3)
#define   _ONE_OFF()        SymbolOFF(CHAR_3F,3)
#define   _COL_OFF()        SymbolOFF(CHAR_2F,3)
#define   _GRADE_OFF()      SymbolOFF(CHAR_S1,0)
#define   _PERCENTAGE_OFF() SymbolOFF(CHAR_S1,1)
#define   _AM_OFF()         SymbolOFF(CHAR_S1,2)
#define   _PM_OFF()         SymbolOFF(CHAR_S1,3)


/*Indicate how Segements are aligned with COMs*/
#define  SEGA   0x01
#define  SEGB   0x02
#define  SEGC   0x04
#define  SEGD   0x08

#define  SEGF   0x01
#define  SEGG   0x02
#define  SEGE   0x04

#define _ALLON  0x0F;


/*ASCCI codification table specifications */
#define ASCCI_TABLE_START '+'   // indicates which is the first Ascii character in the table
#define ASCCI_TABLE_END   'Z'   // indicates which is the End Ascii character in the table
#define BLANK_CHARACTER   '.'   // Inidicates which character can be treated as BLANK or space,  any ASCCI_TABLE_START< ascii <ASCCI_TABLE_END  will be replaced for this character


/* Fault detect initial parameters and limits */
#define FAULTD_FP_FDPRS  FDPRS_32
#define FAULTD_FP_FDSWW  FDSWW_128
#define FAULTD_BP_FDPRS  FDPRS_64
#define FAULTD_BP_FDSWW  FDSWW_128

#define FAULTD_FP_HI  127
#define FAULTD_FP_LO  110
#define FAULTD_BP_HI  127
#define FAULTD_BP_LO  110
#define FAULTD_TIME   6

extern const char  WF_ORDERING_TABLE[];              //   Logical Front plane N and backplanes to LCD_WFx,      Defined in LCDMODULE.c file
extern const char  ASCII_TO_WF_CODIFICATION_TABLE[]; //   ASCII to Segments decodification table                Defined in LCDMODULE.c file


