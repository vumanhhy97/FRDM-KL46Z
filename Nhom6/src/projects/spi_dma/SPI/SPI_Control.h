/****************************************************************************************************/
/**
Copyright (c) 2008 Freescale Semiconductor
Freescale Confidential Proprietary
\file       SPI_Control.h
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

#ifndef SPI_CONTROL_H_
#define SPI_CONTROL_H_
/*****************************************************************************************************
* Include files
*****************************************************************************************************/
#include "ProjectTypes.h"
/*****************************************************************************************************
* Declaration of project wide TYPES
*****************************************************************************************************/
typedef enum
{
	SPI_CONTROL_TX_DONE = 0,
	SPI_CONTROL_TX_IN_PROGRESS,
	SPI_CONTROL_RX_DONE,
	SPI_CONTROL_RX_IN_PROGRESS
}eSPIControlStatus;
/*****************************************************************************************************
* Definition of project wide VARIABLES
*****************************************************************************************************/
extern volatile u8 gu8SPIControlStatus;
/*****************************************************************************************************
* Definition of project wide MACROS / #DEFINE-CONSTANTS
*****************************************************************************************************/
#define SPI_TX_DMA_CHANNEL	(0)
#define SPI_RX_DMA_CHANNEL	(1)

#define SPI_CONTROL_CHECK_STATUS(x) 			(gu8SPIControlStatus & (1 << (x)) )
#define SPI_CONTROL_SET_STATUS(x)		(gu8SPIControlStatus |= (1 << (x)) )
#define SPI_CONTROL_CLEAR_STATUS(x) 	(gu8SPIControlStatus &= ~(1 << (x)) )
/*****************************************************************************************************
* Declaration of project wide FUNCTIONS
*****************************************************************************************************/

/*****************************************************************************************************
* Declaration of module wide FUNCTIONs - NOT for use in other modules
*****************************************************************************************************/
void vfnSPI8Control_Init(void);
void vfnSPI8Control_TxBuffer(u8 * pu8TxBuffer, u16 u16DataSize);
void vfnSPI8Control_RxBuffer(u8 * pu8RxBuffer, u16 u16DataSize);

void vfnSPI16Control_Init(void);
void vfnSPI16Control_TxBuffer(u16 * pu16TxBuffer, u16 u16DataSize);
void vfnSPI16Control_RxBuffer(u16 * pu16RxBuffer, u16 u16DataSize);
#endif /*SPI_CONTROL_H_*/

