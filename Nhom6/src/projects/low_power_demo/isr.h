/******************************************************************************
* File:    isr.h
* Purpose: Define interrupt service routines referenced by the vector table.
* Note: Only "vectors.c" should include this header file.
******************************************************************************/

#ifndef __ISR_H
#define __ISR_H 1

#include "nmi_isr_handler.h"
#include "llwu.h"
#include "low_power_demo.h"
#include "lptmr.h"
#ifndef CMSIS
#undef VECTOR_002
#define VECTOR_002 nmi_isr

#undef  VECTOR_015
#define VECTOR_015 systick_isr

#undef VECTOR_044
#define VECTOR_044 demo_lptmr_isr

#undef VECTOR_046
#define VECTOR_046 porta_isr

#undef VECTOR_023
#define VECTOR_023 llwu_isr
#else
#define systick_isr			SysTick_Handler
#define demo_lptmr_isr	LPTimer_IRQHandler
#define porta_isr				PORTA_IRQHandler
#define llwu_isr				LLW_IRQHandler
#endif
#endif  //__ISR_H

/* End of "isr.h" */
