/****************************************************************************************************/
/**
Copyright (c) 2008 Freescale Semiconductor
Freescale Confidential Proprietary
\file       SPI.h
\brief
\author     Freescale Semiconductor
\author     Guadalajara Applications Laboratory RTAC Americas
\author     B22385
\version    0.1
\date       Sep 13, 2012
*/
/****************************************************************************************************/
/*                                                                                                  */
/* All software, source code, included documentation, and any implied know-how are property of      */
/* Freescale Semiconductor and therefore considered CONFIDENTIAL INFORMATION.                       */
/* This confidential information is disclosed FOR DEMONSTRATION PURPOSES ONLY.                      */
/*                                                                                                  */
/* All Confidential Information remains the property of Freescale Semiconductor and will not be     */
/* copied or reproduced without the express written permission of the Discloser, except for copies  */
/* that are absolutely necessary in order to fulfill the Purpose.                                   */
/*                                                                                                  */
/* Services performed by FREESCALE in this matter are performed AS IS and without any warranty.     */
/* CUSTOMER retains the final decision relative to the total design and functionality of the end    */
/* product.                                                                                         */
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
* Module definition against multiple inclusion
*****************************************************************************************************/

#ifndef SPI_H_
#define SPI_H_
/*****************************************************************************************************
* Include files
*****************************************************************************************************/

/*****************************************************************************************************
* Declaration of project wide TYPES
*****************************************************************************************************/

/*****************************************************************************************************
* Definition of project wide VARIABLES
*****************************************************************************************************/

/*****************************************************************************************************
* Definition of project wide MACROS / #DEFINE-CONSTANTS
*****************************************************************************************************/
#define SPI_C1	(SPI1_BASE_PTR->C1)
#define SPI_C2	(SPI1_BASE_PTR->C2)
#define SPI_C3	(SPI1_BASE_PTR->C3)
#define SPI_BR	(SPI1_BASE_PTR->BR)
#define SPI_DL	(SPI1_BASE_PTR->DL)
#define SPI_DH	(SPI1_BASE_PTR->DH)

#define SPI1_ENABLE	(SPI_C1 |= SPI_C1_SPE_MASK)
#define SPI1_DISABLE	(SPI_C1 &= ~SPI_C1_SPE_MASK)

#define SPI_BAUDRATE_PRESCALER	(0x00)
#define SPI_BAUDRATE_DIVISOR	(0x00)
/*****************************************************************************************************
* Declaration of project wide FUNCTIONS
*****************************************************************************************************/
void vfnSPI8_Init(void);
void vfnSPI16_Init(void);
/*****************************************************************************************************
* Declaration of module wide FUNCTIONs - NOT for use in other modules
*****************************************************************************************************/

#endif /*SPI_H_*/

