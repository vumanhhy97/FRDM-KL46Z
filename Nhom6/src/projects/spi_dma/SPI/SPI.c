/****************************************************************************************************/
/**
Copyright (c) 2008 Freescale Semiconductor
Freescale Confidential Proprietary
\file       SPI.c
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
* Include files
*****************************************************************************************************/
#include "MKL46Z4.h"
#include "ProjectTypes.h"
#include "SPI.h"
/*****************************************************************************************************
* Declaration of module wide FUNCTIONs - NOT for use in other modules
*****************************************************************************************************/

/*****************************************************************************************************
* Definition of module wide MACROs / #DEFINE-CONSTANTs - NOT for use in other modules
*****************************************************************************************************/
/*****************************************************************************************************
* Declaration of module wide TYPEs - NOT for use in other modules
*****************************************************************************************************/

/*****************************************************************************************************
* Definition of module wide VARIABLEs - NOT for use in other modules
*****************************************************************************************************/

/*****************************************************************************************************
* Definition of module wide (CONST-) CONSTANTs - NOT for use in other modules
*****************************************************************************************************/

/*****************************************************************************************************
* Code of project wide FUNCTIONS
*****************************************************************************************************/

void vfnSPI8_Init(void)
{
	SIM_SCGC4 |= SIM_SCGC4_SPI1_MASK;
	SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;
	
	/* SPI1_CLK = PTE2 = ALT2 */
	PORTE_PCR2 = PORT_PCR_MUX(0x02);
	/* SPI1_CS0 = PTE4 = ALT2 */
	PORTE_PCR4 = PORT_PCR_MUX(0x02);
	/* SPI1_MOSI = PTE1 = ALT2 */
	PORTE_PCR1 = PORT_PCR_MUX(0x02)|PORT_PCR_DSE_MASK;
	/* SPI1_MISO = PTE3 = ALT2*/
	PORTE_PCR3 = PORT_PCR_MUX(0x02);
	
	
	SPI_C1 |= SPI_C1_MSTR_MASK | SPI_C1_SSOE_MASK;
	
	SPI_C2 |= SPI_C2_MODFEN_MASK|SPI_C2_TXDMAE_MASK|SPI_C2_RXDMAE_MASK;
               
        SPI_C3 |= SPI_C3_FIFOMODE_MASK;

	/* SPI_BAUD = BUS_CLK / ((SPPR+1)x 2^(SPR+1)) */
	/* SPI_BAUD = 24Mhz / (6x(2^(1)))*/
	/* SPI_BAUD = 1Mhz */
	SPI_BR = SPI_BR_SPPR(SPI_BAUDRATE_PRESCALER) | SPI_BR_SPR(SPI_BAUDRATE_DIVISOR);
	
}

void vfnSPI16_Init(void)
{
	SIM_SCGC4 |= SIM_SCGC4_SPI1_MASK;
	SIM_SCGC5 |= SIM_SCGC5_PORTE_MASK;
	
	/* SPI1_CLK = PTE2 = ALT2 */
	PORTE_PCR2 = PORT_PCR_MUX(0x02);
	/* SPI1_CS0 = PTE4 = ALT2 */
	PORTE_PCR4 = PORT_PCR_MUX(0x02);
	/* SPI1_MOSI = PTE1 = ALT2 */
	PORTE_PCR1 = PORT_PCR_MUX(0x02)|PORT_PCR_DSE_MASK;
	/* SPI1_MISO = PTE3 = ALT2*/
	PORTE_PCR3 = PORT_PCR_MUX(0x02);
	
	
	SPI_C1 |= SPI_C1_MSTR_MASK | SPI_C1_SSOE_MASK;
	
        SPI_C2 |= SPI_C2_MODFEN_MASK|SPI_C2_TXDMAE_MASK|SPI_C2_RXDMAE_MASK|SPI_C2_SPIMODE_MASK;
        
        SPI_C3 |= SPI_C3_FIFOMODE_MASK;

	/* SPI_BAUD = BUS_CLK / ((SPPR+1)x 2^(SPR+1)) */
	/* SPI_BAUD = 24Mhz / (6x(2^(1)))*/
	/* SPI_BAUD = 1Mhz */
	SPI_BR = SPI_BR_SPPR(SPI_BAUDRATE_PRESCALER) | SPI_BR_SPR(SPI_BAUDRATE_DIVISOR);
	
}