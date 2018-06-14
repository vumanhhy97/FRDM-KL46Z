/******************************************************************************
* File:    isr.h
* Purpose: Define interrupt service routines referenced by the vector table.
* Note: Only "vectors.c" should include this header file.
******************************************************************************/

#ifndef __ISR_H
#define __ISR_H 1

void rtc_isrv(void);
void rtc_isrv_seconds(void);
void llwu_isr(void);



#undef  VECTOR_036
#define VECTOR_036 rtc_isrv  //Alarm

#undef  VECTOR_037
#define VECTOR_037 rtc_isrv_seconds


#undef  VECTOR_023
#define VECTOR_023 llwu_isr


/* Example */
/*
#undef  VECTOR_101
#define VECTOR_101 lpt_isr


// ISR(s) are defined in your project directory.
extern void lpt_isr(void);
*/


#endif  //__ISR_H

/* End of "isr.h" */
