/******************************************************************************
* File:    isr.h
* Purpose: Define interrupt service routines referenced by the vector table.
* Note: Only "vectors.c" should include this header file.
******************************************************************************/

#ifndef __ISR_H
#define __ISR_H 1


/* Example */
/*
#undef  VECTOR_101
#define VECTOR_101 lpt_isr


// ISR(s) are defined in your project directory.
extern void lpt_isr(void);
*/

#undef VECTOR_040
#define VECTOR_040 USB_ISR

#undef VECTOR_044
#define VECTOR_044 Timer_ISR

extern void USB_ISR (void);
extern void Timer_ISR (void);

#endif  //__ISR_H

/* End of "isr.h" */
