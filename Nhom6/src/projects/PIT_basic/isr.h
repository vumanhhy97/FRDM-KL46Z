/******************************************************************************
* File:    isr.h
* Purpose: Define interrupt service routines referenced by the vector table.
* Note: Only "vectors.c" should include this header file.
******************************************************************************/

#ifndef __ISR_H
#define __ISR_H 1

extern void Pit1_isrv(void);


#undef  VECTOR_038
#define VECTOR_038 Pit1_isrv


#endif  //__ISR_H

/* End of "isr.h" */
