/****************************************************************************************************/
/*
 \file       LCD.c
 \author     Freescale Semiconductor
 \author     Technical Information Center (TIC)
 \version    1.0
 \date       Sep 5, 2011 
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
#include "LCD.h"
#include "LCDConfig.h"

/*used to indicate in which position of the LCD is the next charater to write */
uint8 gu8LCD_CharPosition; //indicate which is the next position of character to be written
uint8 lcd_alternate_mode; //This variable must be 0 or 4  for LCD with less than 5 backplanes (write in normal backplanes of alterante backplanes)

extern const uint32 *LCD_TO_PORT[];
extern const uint32 MASK_BIT[32];

uint32 memLCD_PENL; 
uint32 memLCD_PENH;
uint32 memLCD_BPENL;
uint32 memLCD_BPENH;

void vfnLCD_Scroll (uint8 *lbpMessage,uint8 lbSize)
{};

/*
 LCD module initialization according to LCDConfig.h
 it is used as default configuration of the LCD 
 */
void vfnLCD_Init(void) {

     // enable IRCLK
     /*Enable IRCLK LCD source and disconnect from FLL by clearing IREFS */
     MCG_C1= 0x02;
     
     SIM_SCGC5 |= SIM_SCGC5_SLCD_MASK | SIM_SCGC5_PORTB_MASK | SIM_SCGC5_PORTC_MASK | SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTE_MASK;
     
     // Enable IRCLK 
     MCG_C1 |= MCG_C1_IRCLKEN_MASK | MCG_C1_IREFSTEN_MASK;
     MCG_C2 |= !MCG_C2_IRCS_MASK ;  //0 32KHZ internal reference clock; 1= 4MHz irc

     
       
     //vfnLCD_interrupt_init();       
     
     LCD_GCR = 0x0;
     LCD_AR  = 0x0;

 //    lcd_pinmux(0); 
     
/* LCD configurartion according to */     
      LCD_GCR =  (  LCD_GCR_RVEN_MASK*_LCDRVEN  
                   | LCD_GCR_RVTRIM(_LCDRVTRIM)    //0-15
                   | LCD_GCR_CPSEL_MASK*_LCDCPSEL 
                 /*  | LCD_GCR_HREFSEL_MASK*_LCDHREF  */
                    |LCD_GCR_LADJ(_LCDLOADADJUST)     //0-3*/
                 /*  | LCD_GCR_VSUPPLY(_LCDSUPPLY)  //0-3*/
                  /* |!LCD_GCR_LCDIEN_MASK */
                   |!LCD_GCR_FDCIEN_MASK
                   | LCD_GCR_ALTDIV(_LCDALTDIV)  //0-3
               /*    |!LCD_GCR_LCDWAIT_MASK  */
                   |!LCD_GCR_LCDSTP_MASK
                   |!LCD_GCR_LCDEN_MASK 
                   | LCD_GCR_SOURCE_MASK*_LCDCLKSOURCE
                   | LCD_GCR_ALTSOURCE_MASK*_LCDALRCLKSOURCE  
                   | LCD_GCR_LCLK(_LCDLCK)   //0-7
                   | LCD_GCR_DUTY(_LCDDUTY)   //0-7
                 );    
     
      lcd_alternate_mode = LCD_NORMAL_MODE;          //Message will be written to default backplanes  if = 4
     
      vfnLCD_EnablePins();         // Enable LCD pins and **Configure BackPlanes**
      
      LCD_GCR |= LCD_GCR_LCDEN_MASK;
      
      
   /* Configure LCD Auxiliar Register*/   
      LCD_AR  = LCD_AR_BRATE(_LCDBLINKRATE); // all other flags set as zero
      vfnLCD_Write_Msg("1235");

}

/* Enable pins according  WF_ORDERING_TABLE, BP_ORDERING_TABLE
 //Enable the LCD module frontplane waveform output (FP[39:0])
 LCD_PENH = 0xFFFFFFFF;
 LCD_PENL = 0xFFFFFFFF;
 */

void vfnLCD_EnablePins(void) {
	uint8 i;
	uint32 *p_pen;
	uint8 pen_offset; // 0 or 1   
	uint8 pen_bit; //0 to 31

	LCD_PENL = 0x0;
	LCD_PENH = 0x0;
	LCD_BPENL = 0x0;
	LCD_BPENH = 0x0;

	p_pen = (uint32 *) &LCD_PENL;

	for (i = 0; i < _LCDUSEDPINS; i++) {
		pen_offset = WF_ORDERING_TABLE[i]/32;
		pen_bit = WF_ORDERING_TABLE[i]%32;
		p_pen[pen_offset] |= (1<<pen_bit);
		if (i>= _LCDFRONTPLANES) // Pin is a backplane
		{
			p_pen[pen_offset+2] |= (1<<pen_bit); // Enable  BPEN
                        LCD_WF8B(WF_ORDERING_TABLE[i]) = MASK_BIT[i - _LCDFRONTPLANES];   // fill with 0x01, 0x02, etc 
		}
	}
}

/*
 Fill Backplanes with normal mask for  0x01, 0x02,  according to BP_ORDERING_TABLE;
 0 Normal MOde
 1 Alternate Mode (If BackPlanes <4   fill with 0x11
 */
void vfnLCD_ConfigureBackplanes(void) {
	uint8 i;
	for (i = 0; i < _LCDBACKPLANES; i++) {
		if (_LCDBACKPLANES<5)
		LCD_WF8B(WF_ORDERING_TABLE[i+_LCDFRONTPLANES]) = 0x11<<i; // Prepate for alternate Mode
		else
		LCD_WF8B(WF_ORDERING_TABLE[i+_LCDFRONTPLANES]) = 0x01<<i; // Normal Mode
	}
}

/*
 
 rotate Backplanes  scroll_y units
 if scroll_y>0  rotate to the right
 if scroll_y<0  rotate to the left
 if scroll_y=0  set normal configuration

 */
void vfnBP_VScroll(int8 scroll_count) {
	uint8 bp_count;
	if (scroll_count != 0) {
		for (bp_count = 0; bp_count < _LCDBACKPLANES; bp_count++) {
			if (scroll_count>0)
			LCD_WF8B(WF_ORDERING_TABLE[_LCDFRONTPLANES +bp_count]) = 0x01<<(scroll_count+bp_count);
			else //(scroll_count<0)
			{
				LCD_WF8B(WF_ORDERING_TABLE[_LCDFRONTPLANES +bp_count]) = 0x01>>(-scroll_count+bp_count);
			}
		}
	} else
		vfnLCD_ConfigureBackplanes();
}

void vfnLCD_Home(void) {
	gu8LCD_CharPosition = 0;
}

void vfnLCD_Contrast(uint8 lbContrast) {
	lbContrast &= 0x0F; //Forced to the only values accepted 
	LCD_GCR |= LCD_GCR_RVTRIM(lbContrast);
}

void vfnLCD_Write_MsgPlace(uint8 *lbpMessage, uint8 lbSize) {

	if (lbSize > _CHARNUM) {
		vfnLCD_Scroll(lbpMessage, lbSize);
	} else {
		do{
		vfnLCD_Write_Char (*lbpMessage);
		lbpMessage ++;
	}while (--lbSize);
}

}

void  vfnLCD_Write_Msg_GoTo (int8 *lbpMessage, uint8 lbSize, uint8 lbStartWrite)
{ 
          
   gu8LCD_CharPosition = lbStartWrite;  //Home display
   
   if (lbSize<=_CHARNUM && *lbpMessage)
   {
     while (lbSize--) 
     {
       vfnLCD_Write_Char (*lbpMessage++);
     }  
   }            
}


/*
 vfnLCD_Write_Msg.  Display a Message starting at the fisrt character of display
 until _CHARNUM  or End of string.
 */

void vfnLCD_Write_Msg(char *lbpMessage) {

	uint8 lbSize = 0;
	gu8LCD_CharPosition = 0; //Home display
	while (lbSize < _CHARNUM && *lbpMessage) {
		vfnLCD_Write_Char (*lbpMessage++);
		lbSize++;
	}

	if (lbSize < _CHARNUM) {

		while (lbSize++ < _CHARNUM)
			vfnLCD_Write_Char(BLANK_CHARACTER); // complete data with blanks
	}

}


void vfnLCD_disable_module(void)
 {
   
   
   memLCD_PENL  = LCD_PENL ;
   memLCD_PENH  = LCD_PENH ;
   memLCD_BPENL = LCD_BPENL;
   memLCD_BPENH = LCD_BPENH;
   
   LCD_PENL = 0x0;
   LCD_PENH = 0x0;
   LCD_BPENL = 0x0;
   LCD_BPENH = 0x0;
   
 }


void vfnLCD_enable_module(void)
 {
   //LCD_GCR |= LCD_GCR_LCDEN_MASK;

   LCD_PENL  = memLCD_PENL ;
   LCD_PENH  = memLCD_PENH ;
   LCD_BPENL = memLCD_BPENL;
   LCD_BPENH = memLCD_BPENH;

   
   
 }

 
void vfnLCD_Set_Display(void) {

	vfnLCD_Write_Msg("1888");

	// Symbols ON
	_FREESCALE_ON();
	_ONE_ON();
	_COL_ON();
	_GRADE_ON();
	_PERCENTAGE_ON();
	_AM_ON();
	_PM_ON();

}

void vfnLCD_Clear_Display(void) {
	vfnLCD_Write_Msg("    ");
	// Symbols OFF
	_FREESCALE_OFF();
	_ONE_OFF();
	_COL_OFF();
	_GRADE_OFF();
	_PERCENTAGE_OFF();
	_AM_OFF();
	_PM_OFF();

}

void vfnLCD_All_Segments_OFF(void) {
	uint8 lbTotalBytes = _CHARNUM * _LCDTYPE;
	uint8 lbCounter = 0;
	while (lbCounter < lbTotalBytes) {
		LCD_WF8B(WF_ORDERING_TABLE[lbCounter++])=0;
	}
}

/*
 Dot Matrix functions
 */
void PutPoint(uint8 x, uint8 y) {

	if (x >= _CHARNUM || y > 7)
		return;
	LCD_WF8B(WF_ORDERING_TABLE[x]) |= (1 << y);
}

void ClrPoint(uint8 x, uint8 y) {
	if (x >= _CHARNUM || y > 7)
		return;
	LCD_WF8B(WF_ORDERING_TABLE[x]) &= ~(1 << y);
}

void SetX(uint8 x, uint8 value) {
	if (x >= _CHARNUM)
		return;
	LCD_WF8B(WF_ORDERING_TABLE[x]) = value;

}

/*
 Write for GD6363P.h
 vfnLCD_Write_Char   for GD6363P.h only 
 NOte this function is hardly dependent on hardwarem, usually should be adjusted 
 from one LCD panel to another one.
 */
void vfnLCD_Write_Char(uint8 lbValue) {
	uint8 char_val; //for test only
	uint8 lbCounter;
	uint16 arrayOffset;
	uint8 position;

	/*ascci character outside defined range or value not writeable, force to BLANK_CHARACTER*/

	if (gu8LCD_CharPosition == 0) // ensure bLCD position is in valid limit
	{
		if (lbValue == '1' || lbValue == 'l')
			_ONE_ON();
		else
			_ONE_OFF();
	} else {
		if (lbValue >= 'a' && lbValue <= 'z')
			lbValue -= 32; // UpperCase
		if (lbValue < ASCCI_TABLE_START || lbValue > ASCCI_TABLE_END)
			lbValue = BLANK_CHARACTER;

		lbValue -= ASCCI_TABLE_START; // Remove the offset to search in the ascci table        
		arrayOffset = (lbValue * _CHAR_SIZE); // Compensate matrix offset

		lbCounter = 0; //counts how many LCD_WFx need to be written in order to complete one char
		while (lbCounter < _CHAR_SIZE && gu8LCD_CharPosition < _CHARNUM) {
			position = (gu8LCD_CharPosition - 1) *_CHAR_SIZE + lbCounter; //-1 to compensate first incomplete character

			// Cleare previous bit
			if (lbCounter==0)
			LCD_WF8B(WF_ORDERING_TABLE[position])&= (~ (0x0F<<lcd_alternate_mode));
			else
			LCD_WF8B(WF_ORDERING_TABLE[position])&= (~(0x7<<lcd_alternate_mode));

			char_val = ASCII_TO_WF_CODIFICATION_TABLE[arrayOffset + lbCounter];
			LCD_WF8B(WF_ORDERING_TABLE[position]) |= (char_val<<lcd_alternate_mode);

			lbCounter++;
		}
	}
	gu8LCD_CharPosition++;

}

/********************
 Kinetis specific functions
 **********/

/*
 Configure LCD used pins as MUX=0  for LCD normal operation (analog operation)
 Configure LCD used pins as MUX=7  for LCD Fault detectionoperation (pull resistor  control enabled)
 only 0 or 7 is allowed for LCD operation
 */

void vfnlcd_pinmux(uint8 mux_val) {
	uint8 i;
	for (i = 0; i < _LCDUSEDPINS; i++) {
		*(uint32 *)LCD_TO_PORT[WF_ORDERING_TABLE[i]] &= ~PORT_PCR_MUX_MASK; //Clear previos value
		*(uint32 *)LCD_TO_PORT[WF_ORDERING_TABLE[i]] |= PORT_PCR_MUX(mux_val); // Set new value
	}
}

/*
 This table relates LCD_Px to PORTx_n Control Pin, it is used for pinmux function
 applyes for K40, and K53 devices
 */
const uint32 *LCD_TO_PORT[60] = { 
    (uint32 *) &PORTB_PCR0,  //LCD_P00
    (uint32 *) &PORTB_PCR1,  //LCD_P01
    (uint32 *) &PORTB_PCR2,  //LCD_P02
    (uint32 *) &PORTB_PCR3,  //LCD_P03
    (uint32 *) &PORTC_PCR20,  //LCD_P04
    (uint32 *) &PORTC_PCR21,  //LCD_P05
    //(uint32 *) &PORTC_PCR22,  //LCD_P06
    (uint32 *) &PORTB_PCR0,  //LCD_P00  test

    (uint32 *) &PORTB_PCR7,  //LCD_P07
    (uint32 *) &PORTB_PCR8,  //LCD_P08
    (uint32 *) &PORTB_PCR9,  //LCD_P09
    (uint32 *) &PORTB_PCR10,  //LCD_P10
    (uint32 *) &PORTB_PCR11,  //LCD_P11
    (uint32 *) &PORTB_PCR16,  //LCD_P12
    (uint32 *) &PORTB_PCR17,  //LCD_P13
    (uint32 *) &PORTB_PCR18,  //LCD_P14
    (uint32 *) &PORTB_PCR19,  //LCD_P15
    (uint32 *) &PORTB_PCR20,  //LCD_P16
    (uint32 *) &PORTB_PCR21,  //LCD_P17
    (uint32 *) &PORTB_PCR22,  //LCD_P18
    (uint32 *) &PORTB_PCR23,  //LCD_P19
    (uint32 *) &PORTC_PCR0,  //LCD_P20
    (uint32 *) &PORTC_PCR1,  //LCD_P21
    (uint32 *) &PORTC_PCR2,  //LCD_P22
    (uint32 *) &PORTC_PCR3,  //LCD_P23
    (uint32 *) &PORTC_PCR4,  //LCD_P24
    (uint32 *) &PORTC_PCR5,  //LCD_P25
    (uint32 *) &PORTC_PCR6,  //LCD_P26
    (uint32 *) &PORTC_PCR7,  //LCD_P27
    (uint32 *) &PORTC_PCR8,  //LCD_P28
    (uint32 *) &PORTC_PCR9,  //LCD_P29
    (uint32 *) &PORTC_PCR10,  //LCD_P30
    (uint32 *) &PORTC_PCR11,  //LCD_P31
    (uint32 *) &PORTC_PCR12,  //LCD_P32
    (uint32 *) &PORTC_PCR13,  //LCD_P33
    (uint32 *) &PORTC_PCR16,  //LCD_P36
    (uint32 *) &PORTC_PCR17,  //LCD_P37
    (uint32 *) &PORTC_PCR18,  //LCD_P38
    (uint32 *) &PORTC_PCR23,  //LCD_P39
    (uint32 *) &PORTD_PCR0,  //LCD_P40
    (uint32 *) &PORTD_PCR1,  //LCD_P41
    (uint32 *) &PORTD_PCR2,  //LCD_P42
    (uint32 *) &PORTD_PCR3,  //LCD_P43
    (uint32 *) &PORTD_PCR4,  //LCD_P44
    (uint32 *) &PORTD_PCR5,  //LCD_P45
    (uint32 *) &PORTD_PCR6,  //LCD_P46
    (uint32 *) &PORTD_PCR7,  //LCD_P47
    (uint32 *) &PORTE_PCR0,  //LCD_P48
    (uint32 *) &PORTE_PCR1,  //LCD_P49
    (uint32 *) &PORTE_PCR2,  //LCD_P50
    (uint32 *) &PORTE_PCR3,  //LCD_P51
    (uint32 *) &PORTE_PCR4,  //LCD_P52
    (uint32 *) &PORTE_PCR5,  //LCD_P53
    (uint32 *) &PORTE_PCR6,  //LCD_P54
    (uint32 *) &PORTE_PCR16,  //LCD_P55
    (uint32 *) &PORTE_PCR17,  //LCD_P56
    (uint32 *) &PORTE_PCR18,  //LCD_P57
    (uint32 *) &PORTE_PCR19,  //LCD_P58
    (uint32 *) &PORTE_PCR20,  //LCD_P59
    (uint32 *) &PORTE_PCR21,  //LCD_P60
		};

const uint32 MASK_BIT[32] = 
{
       0x00000001 ,
       0x00000002 ,
       0x00000004 ,
       0x00000008 ,
       0x00000010 ,
       0x00000020 ,
       0x00000040 ,
       0x00000080 ,
       0x00000100 ,
       0x00000200 ,
       0x00000400 ,
       0x00000800 ,
       0x00001000 ,
       0x00002000 ,
       0x00004000 ,
       0x00008000 ,
       0x00010000 ,
       0x00020000 ,
       0x00040000 ,
       0x00080000 ,
       0x00100000 ,
       0x00200000 ,
       0x00400000 ,
       0x00800000 ,
       0x01000000 ,
       0x02000000 ,
       0x04000000 ,
       0x08000000 ,
       0x10000000 ,
       0x20000000 ,
       0x40000000 ,
       0x80000000 ,
};

/* Fault detection   functions  */

/*||||||||||||||||||||||       u8fnlcd_measure_pin_cap   ||||||||||||||||||||||
 brief: measure capacitance of the pin  (pulling method)
 param:  pinid             (LCDWF number  0-47)
 pin_type           FP_TYPE=0 or BP_TYPE =1
 fdprs_val         Fault detect preescaler value  0-7
 fdsww_val         Fault detect windows width     0-7
 param: none
 return:  capacitance value  (FDSR_FDCNT)   
 */
uint8 u8fnlcd_measure_pin_cap(uint8 pinid, uint8 pin_type, uint8 fdprs_val,
		uint8 fdsww_val) {
	uint8 cap_val;

	if (pin_type == BP_TYPE)
		LCD_FDCR = LCD_FDCR_FDPRS(fdprs_val) | LCD_FDCR_FDSWW(fdsww_val)
				| LCD_FDCR_FDBPEN_MASK | LCD_FDCR_FDPINID(pinid);
	else
		LCD_FDCR = LCD_FDCR_FDPRS(fdprs_val) | LCD_FDCR_FDSWW(fdsww_val)
				| !LCD_FDCR_FDBPEN_MASK | LCD_FDCR_FDPINID(pinid);

	LCD_FDCR |= LCD_FDCR_FDEN_MASK; // insert a FRAME TEST  // This bit is cleared automatically when FDCF is set.


	while (!(LCD_FDSR & LCD_FDSR_FDCF_MASK)) {}; //* wait for a frame-measuerment cycle

	cap_val = (LCD_FDSR & LCD_FDSR_FDCNT_MASK);

	LCD_FDSR |= LCD_FDSR_FDCF_MASK; // Clear flag

	return (cap_val);
}

