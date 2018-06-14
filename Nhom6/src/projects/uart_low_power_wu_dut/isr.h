/******************************************************************************
* File:    isr.h
* Purpose: Define interrupt service routines referenced by the vector table.
* Note: Only "vectors.c" should include this header file.
******************************************************************************/

#ifndef __ISR_H
#define __ISR_H 1

#include "vectors.h"
/* function prototypes */
void uart0_isr(void);
void porta_isr(void);

/* Example */
/*
#undef  VECTOR_101
#define VECTOR_101 lpt_isr


// ISR(s) are defined in your project directory.
extern void lpt_isr(void);
*/
#undef VECTOR_028
#define VECTOR_028 uart0_isr

#undef VECTOR_046
#define VECTOR_046 porta_isr

#endif  //__ISR_H

/* End of "isr.h" */
