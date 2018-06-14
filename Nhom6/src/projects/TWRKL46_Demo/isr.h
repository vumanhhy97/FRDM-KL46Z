/******************************************************************************
* File:    isr.h
* Purpose: Define interrupt service routines referenced by the vector table.
* Note: Only "vectors.c" should include this header file.
******************************************************************************/

#ifndef __ISR_H
#define __ISR_H 1
extern void TSI0_IRQHandler(void);
extern void PIT_IRQHandler(void);
extern void USB0_IRQHandler (void);
extern void RTC_IRQHandler(void);
extern void RTC_Seconds_IRQHandler(void);

#undef  VECTOR_038
#define VECTOR_038 PIT_IRQHandler


#undef VECTOR_042
#define VECTOR_042 TSI0_IRQHandler

#undef VECTOR_040
#define VECTOR_040 USB0_IRQHandler



#undef  VECTOR_036
#define VECTOR_036 RTC_IRQHandler

#undef  VECTOR_037
#define VECTOR_037 RTC_Seconds_IRQHandler



#endif  //__ISR_H

/* End of "isr.h" */
