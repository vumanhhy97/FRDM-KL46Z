/* 
\file       GD-6363P.C
\author     Freescale Semiconductor
\author     Technical Information Center (TIC)
\version    1.0
\date       Sep 5, 2011 

This file depends specifically on the LCD Panel to be controlled, and is independent of pin assigment
NOTE: xxx-GD-6363p.h have the information on how LCD pins are connected 
        1) How FP and BP are organized      WF_ORDERING_TABLE[]      
        2) Ascii to Segment coficiation    ASCII_TO_WF_CODIFICATION_TABLE[]
Note Char_1x, and CHARCOM are defined in the TWRPI-GD6363p.h  This file is dependent on the specific hardware connections
*/

/*****************************************************************************************************
* Include files
*****************************************************************************************************/
#include "LCDConfig.h"

/*
  This table have following functions
   1) indicate  LCD_Px used by the application
   2) Indicate  LCD_Px used as Front Planes and which are used as BackPlanes (BP_TYPE)
   Characters must be ordered in a logical manner, and backplanes must be always at the end of the list,  first backplane defined will be configured as COM0,
   following as COM1, and so on. 
*/


const char WF_ORDERING_TABLE[ ] =
{
     CHAR_1A,   // LCD22 --- GD-Pin:10
     CHAR_1F,   // LCD24 --- GD-Pin:11
     CHAR_2A,   // LCD3 --- GD-Pin:8
     CHAR_2F,   // LCD20 --- GD-Pin:9
     CHAR_3A,   // LCD23 --- GD-Pin:5
     CHAR_3F,   // LCD1 --- GD-Pin:7
     CHAR_S1,   // LCD0 --- GD-Pin:6
     CHARCOM0,   // LCD12 --- GD-Pin:1
     CHARCOM1,   // LCD13 --- GD-Pin:2
     CHARCOM2,   // LCD14 --- GD-Pin:3
     CHARCOM3,   // LCD15 --- GD-Pin:4


};

/*
   Ascii decodification table
   Inidicate what segements must be turned on on respective waveform to create a character
*/
const char ASCII_TO_WF_CODIFICATION_TABLE [ ] =
{
   (!SEGA|!SEGB|!SEGC|!SEGD) ,	( !SEGE|!SEGF|!SEGG), //+
   (!SEGA|!SEGB| SEGC|!SEGD) ,	( !SEGE|!SEGF|!SEGG), //,
   (!SEGA|!SEGB|!SEGC|!SEGD) ,	( !SEGE|!SEGF| SEGG), //-
   (!SEGA|!SEGB|!SEGC|!SEGD) ,	( !SEGE|!SEGF|!SEGG), //.
   (!SEGA|!SEGB|!SEGC|!SEGD) ,	( !SEGE|!SEGF|!SEGG), ///
   ( SEGA| SEGB| SEGC| SEGD) ,	(  SEGE| SEGF|!SEGG), //0
   (!SEGA| SEGB| SEGC|!SEGD) ,	( !SEGE|!SEGF|!SEGG), //1
   ( SEGA| SEGB|!SEGC| SEGD) ,	(  SEGE|!SEGF| SEGG), //2
   ( SEGA| SEGB| SEGC| SEGD) ,	( !SEGE|!SEGF| SEGG), //3
   (!SEGA| SEGB| SEGC|!SEGD) ,	( !SEGE| SEGF| SEGG), //4
   ( SEGA|!SEGB| SEGC| SEGD) ,	( !SEGE| SEGF| SEGG), //5
   ( SEGA|!SEGB| SEGC| SEGD) ,	(  SEGE| SEGF| SEGG), //6
   ( SEGA| SEGB| SEGC|!SEGD) ,	( !SEGE|!SEGF|!SEGG), //7
   ( SEGA| SEGB| SEGC| SEGD) ,	(  SEGE| SEGF| SEGG), //8
   ( SEGA| SEGB| SEGC| SEGD) ,	( !SEGE| SEGF| SEGG), //9
   (!SEGA|!SEGB|!SEGC|!SEGD) ,	( !SEGE|!SEGF|!SEGG), //:
   (!SEGA|!SEGB|!SEGC|!SEGD) ,	( !SEGE|!SEGF|!SEGG), //;
   (!SEGA|!SEGB|!SEGC|!SEGD) ,	( !SEGE|!SEGF|!SEGG), //<
   (!SEGA|!SEGB|!SEGC|!SEGD) ,	( !SEGE|!SEGF|!SEGG), //=
   (!SEGA|!SEGB|!SEGC|!SEGD) ,	( !SEGE|!SEGF|!SEGG), //>
   (!SEGA|!SEGB|!SEGC|!SEGD) ,	( !SEGE|!SEGF|!SEGG), //?
   (!SEGA|!SEGB|!SEGC|!SEGD) ,	( !SEGE|!SEGF|!SEGG), //@
   ( SEGA| SEGB| SEGC|!SEGD) ,	(  SEGE| SEGF| SEGG), //A
   (!SEGA|!SEGB| SEGC| SEGD) ,	(  SEGE| SEGF| SEGG), //B
   ( SEGA|!SEGB|!SEGC| SEGD) ,	(  SEGE| SEGF|!SEGG), //C
   (!SEGA| SEGB| SEGC| SEGD) ,	(  SEGE|!SEGF| SEGG), //D
   ( SEGA|!SEGB|!SEGC| SEGD) ,	(  SEGE| SEGF| SEGG), //E
   ( SEGA|!SEGB|!SEGC|!SEGD) ,	(  SEGE| SEGF| SEGG), //F
   ( SEGA|!SEGB| SEGC| SEGD) ,	(  SEGE| SEGF| SEGG), //G
   (!SEGA| SEGB| SEGC|!SEGD) ,	(  SEGE| SEGF| SEGG), //H
   (!SEGA| SEGB| SEGC|!SEGD) ,	( !SEGE|!SEGF|!SEGG), //I
   (!SEGA| SEGB| SEGC| SEGD) ,	( !SEGE|!SEGF|!SEGG), //J
   (!SEGA|!SEGB|!SEGC|!SEGD) ,	(  SEGE| SEGF| SEGG), //K
   (!SEGA|!SEGB|!SEGC| SEGD) ,	(  SEGE| SEGF|!SEGG), //L
   (!SEGA|!SEGB|!SEGC|!SEGD) ,	( !SEGE|!SEGF|!SEGG), //M
   (!SEGA|!SEGB| SEGC|!SEGD) ,	(  SEGE|!SEGF| SEGG), //N
   (!SEGA|!SEGB| SEGC| SEGD) ,	(  SEGE|!SEGF| SEGG), //O
   ( SEGA| SEGB|!SEGC|!SEGD) ,	(  SEGE| SEGF| SEGG), //P
   ( SEGA| SEGB| SEGC|!SEGD) ,	( !SEGE| SEGF| SEGG), //Q
   (!SEGA|!SEGB|!SEGC|!SEGD) ,	(  SEGE|!SEGF| SEGG), //R
   ( SEGA|!SEGB| SEGC| SEGD) ,	( !SEGE| SEGF| SEGG), //S
   (!SEGA|!SEGB|!SEGC| SEGD) ,	(  SEGE| SEGF| SEGG), //T
   (!SEGA|!SEGB| SEGC| SEGD) ,	(  SEGE|!SEGF|!SEGG), //U
   (!SEGA|!SEGB| SEGC| SEGD) ,	(  SEGE|!SEGF|!SEGG), //V
   (!SEGA|!SEGB|!SEGC|!SEGD) ,	( !SEGE|!SEGF|!SEGG), //W
   (!SEGA| SEGB| SEGC| SEGD) ,	(  SEGE| SEGF| SEGG), //X
   (!SEGA| SEGB| SEGC| SEGD) ,	( !SEGE| SEGF| SEGG), //Y
   ( SEGA| SEGB|!SEGC| SEGD) ,	(  SEGE|!SEGF| SEGG), //Z
};


