#ifndef __Definitions__
  #define __Definitions__

/** Macros definitions for flag */
#define EVENT(bit)			gu8StatusFlag |= (1<<bit)
#define COMPARE(bit)		gu8StatusFlag & (1<<bit)
#define CLEAR(bit)			gu8StatusFlag &=  ~((UINT32)(1<<bit))

#define RRTC_EVENT						EVENT(0)		
#define RRTC									COMPARE(0)					
#define RRTC_CLEAR						CLEAR(0)		

#define KBI_EVENT							EVENT(1)		
#define KEY										COMPARE(1)					
#define KEY_CLEAR							CLEAR(1)		

#define FALSE_KEY_EVENT				EVENT(2)
#define FALSE_KEY							COMPARE(2)
#define FALSE_KEY_CLEAR				CLEAR(2)

#define END_MENU_EVENT			EVENT(3)
#define END_MENU							COMPARE(3)
#define END_MENU_CLEAR				CLEAR(3)

#define	MIN_INT_EVENT					EVENT(6)
#define MIN_INT								COMPARE(6)
#define MIN_INT_CLEAR					CLEAR(6)

#define CNTDN_ENABLE_EVENT		EVENT(7)
#define CNTDN_ENABLE					COMPARE(7)
#define	 CNTDN_ENABLE_CLEAR		CLEAR(7)


  /* Typedefs */
  /* Standard Types */
  typedef unsigned char   UINT8;  		/*unsigned 8 bit definition */
  typedef unsigned short  UINT16; 		/*unsigned 16 bit definition*/
  typedef unsigned long   UINT32; 		/*unsigned 32 bit definition*/
  typedef signed char       INT8;   		/*signed 8 bit definition */
  typedef short            INT16;  		/*signed 16 bit definition*/
  typedef long int         INT32;  		/*signed 32 bit definition*/

 /* State Machine */
  typedef struct
  {
    UINT8 ActualState;
    UINT8 PrevState;
    UINT8 NextState;
    UINT8 ErrorState;
  }sSM;

typedef struct 
{
  UINT16	u16Years;
  UINT8	u8Months;
  UINT8	u8Days;
  UINT8 	u8Hours;
  UINT8	u8DEndMonths;
  UINT8	u8DEndDays;
  UINT8 	u8DEndHours;
  UINT8 	u8Minutes;
  UINT8 	u8CntDown;
  UINT16 u16Interrupts;	
}ClockStatus;


#endif /* __Definitions__ */