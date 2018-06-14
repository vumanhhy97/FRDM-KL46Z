/****************************************************************************************************/
/**
Copyright (c) 2008 Freescale Semiconductor
Freescale Confidential Proprietary
\file       SPI_Control.c
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
#include "ProjectTypes.h"
#include "MKL46Z4.h"
#include "DMA_spi.h"
#include "SPI.h"
#include "SPI_Control.h"
/*****************************************************************************************************
* Declaration of module wide FUNCTIONs - NOT for use in other modules
*****************************************************************************************************/
void vfnSPIControl_DMATxCallback(void);
void vfnSPIControl_DMARxCallback(void);
/*****************************************************************************************************
* Definition of module wide MACROs / #DEFINE-CONSTANTs - NOT for use in other modules
*****************************************************************************************************/
#define SPI8_TX_DMA_CONFIG		(DMA_DCR_EINT_MASK|DMA_DCR_SINC_MASK|DMA_DCR_SSIZE(0x01)|DMA_DCR_DSIZE(0x01)|DMA_DCR_D_REQ_MASK|DMA_DCR_CS_MASK);
#define SPI8_TX_DUMMY_DMA_CONFIG	(DMA_DCR_EINT_MASK|DMA_DCR_SSIZE(0x01)|DMA_DCR_DSIZE(0x01)|DMA_DCR_D_REQ_MASK|!DMA_DCR_CS_MASK);

#define SPI16_TX_DMA_CONFIG		(DMA_DCR_EINT_MASK|DMA_DCR_SINC_MASK|DMA_DCR_SSIZE(0x02)|DMA_DCR_DSIZE(0x02)|DMA_DCR_D_REQ_MASK|DMA_DCR_CS_MASK);
#define SPI16_TX_DUMMY_DMA_CONFIG	(DMA_DCR_EINT_MASK|DMA_DCR_SSIZE(0x02)|DMA_DCR_DSIZE(0x02)|DMA_DCR_D_REQ_MASK|!DMA_DCR_CS_MASK);

/*****************************************************************************************************
* Declaration of module wide TYPEs - NOT for use in other modules
*****************************************************************************************************/
volatile u8 gu8SPIControlStatus = 0;
const u8 gu8DummyTx = 0x55;
const u8 gu8DummyTx2 = 0x11;
const u16 gu16DummyTx = 0x1515;
const u16 gu16DummyTx2 = 0x1111;

volatile u16 gu16TestSPIData=0;
/*****************************************************************************************************
* Definition of module wide VARIABLEs - NOT for use in other modules
*****************************************************************************************************/
sDMAConfig sSPITxDMAConfig;

sDMAConfig sSPIRxDMAConfig;
/*****************************************************************************************************
* Definition of module wide (CONST-) CONSTANTs - NOT for use in other modules
*****************************************************************************************************/

/*****************************************************************************************************
* Code of project wide FUNCTIONS
*****************************************************************************************************/

void vfnSPI8Control_Init(void)
{	
	vfnSPI8_Init();
	
	sSPITxDMAConfig.u32Channel = SPI_TX_DMA_CHANNEL;
	sSPITxDMAConfig.u32SourceAddres = 0UL;
	sSPITxDMAConfig.u32DestinationAddress = (u32)(&SPI_DL);
	sSPITxDMAConfig.u32StatusByteCount = 0UL;
	sSPITxDMAConfig.u32Control = SPI8_TX_DMA_CONFIG;
	sSPITxDMAConfig.u32TriggerSource = DMA_TRIGGER_SPI1_TX;
	
	vfnDMA_Init(&sSPITxDMAConfig,vfnSPIControl_DMATxCallback);
	
	
	sSPIRxDMAConfig.u32Channel = SPI_RX_DMA_CHANNEL;
	sSPIRxDMAConfig.u32SourceAddres = (u32)(&SPI_DL);
	sSPIRxDMAConfig.u32DestinationAddress = 0UL;
	sSPIRxDMAConfig.u32StatusByteCount = 0UL;
	sSPIRxDMAConfig.u32Control = DMA_DCR_EINT_MASK|DMA_DCR_DINC_MASK|DMA_DCR_SSIZE(0x01)|DMA_DCR_DSIZE(0x01)|DMA_DCR_D_REQ_MASK|!DMA_DCR_CS_MASK;
	sSPIRxDMAConfig.u32TriggerSource = DMA_TRIGGER_SPI1_RX;
	
	vfnDMA_Init(&sSPIRxDMAConfig,vfnSPIControl_DMARxCallback);
	
}

void vfnSPI8Control_TxBuffer(u8 * pu8TxBuffer, u16 u16DataSize)
{
	
	if(!(SPI_CONTROL_CHECK_STATUS(SPI_CONTROL_TX_IN_PROGRESS)))
	{
		SPI_CONTROL_SET_STATUS(SPI_CONTROL_TX_IN_PROGRESS);
				
		sSPITxDMAConfig.u32Control = SPI8_TX_DMA_CONFIG;
		DMA_UPDATE_CONTROL(sSPITxDMAConfig.u32Channel,sSPITxDMAConfig.u32Control);
		
		sSPITxDMAConfig.u32StatusByteCount = (u32)(u16DataSize-1);
		DMA_UPDATE_BYTE_COUNT(sSPITxDMAConfig.u32Channel,sSPITxDMAConfig.u32StatusByteCount);
		
		SPI_DL = *pu8TxBuffer;
		pu8TxBuffer++;		
		sSPITxDMAConfig.u32SourceAddres = (u32)pu8TxBuffer;
		DMA_UPDATE_SOURCE_ADDRESS(sSPITxDMAConfig.u32Channel,sSPITxDMAConfig.u32SourceAddres );
		
		DMA_ENABLE_ERQ(sSPITxDMAConfig.u32Channel);
		
		SPI1_ENABLE;
	}
	
	
}

void vfnSPI8Control_RxBuffer(u8 * pu8RxBuffer, u16 u16DataSize)
{
	
	if(!(SPI_CONTROL_CHECK_STATUS(SPI_CONTROL_RX_IN_PROGRESS)))
	{
		SPI_CONTROL_SET_STATUS(SPI_CONTROL_RX_IN_PROGRESS);		
		
		sSPITxDMAConfig.u32Control = SPI8_TX_DUMMY_DMA_CONFIG;
		DMA_UPDATE_CONTROL(sSPITxDMAConfig.u32Channel,sSPITxDMAConfig.u32Control);
		
		sSPITxDMAConfig.u32StatusByteCount = (u32)(u16DataSize);
		DMA_UPDATE_BYTE_COUNT(sSPITxDMAConfig.u32Channel,sSPITxDMAConfig.u32StatusByteCount);
				
		sSPITxDMAConfig.u32SourceAddres = (u32)&gu8DummyTx;
		DMA_UPDATE_SOURCE_ADDRESS(sSPITxDMAConfig.u32Channel,sSPITxDMAConfig.u32SourceAddres );
		
		
		sSPIRxDMAConfig.u32StatusByteCount = (u32)(u16DataSize);
		DMA_UPDATE_BYTE_COUNT(sSPIRxDMAConfig.u32Channel,sSPIRxDMAConfig.u32StatusByteCount);
		
		
		sSPIRxDMAConfig.u32DestinationAddress = (u32)pu8RxBuffer;
		DMA_UPDATE_DESTINATION_ADDRESS(sSPIRxDMAConfig.u32Channel,sSPIRxDMAConfig.u32DestinationAddress );
		
		DMA_ENABLE_ERQ(sSPIRxDMAConfig.u32Channel);
		DMA_ENABLE_ERQ(sSPITxDMAConfig.u32Channel);
		SPI1_ENABLE;
	}
	
	
}

void vfnSPI16Control_Init(void)
{	
	vfnSPI16_Init();
	
	sSPITxDMAConfig.u32Channel = SPI_TX_DMA_CHANNEL;
	sSPITxDMAConfig.u32SourceAddres = 0UL;
	sSPITxDMAConfig.u32DestinationAddress = (u32)(&SPI_DL);
	sSPITxDMAConfig.u32StatusByteCount = 0UL;
	sSPITxDMAConfig.u32Control = SPI16_TX_DMA_CONFIG;
	sSPITxDMAConfig.u32TriggerSource = DMA_TRIGGER_SPI1_TX;
	
	vfnDMA_Init(&sSPITxDMAConfig,vfnSPIControl_DMATxCallback);
	
	
	sSPIRxDMAConfig.u32Channel = SPI_RX_DMA_CHANNEL;
	sSPIRxDMAConfig.u32SourceAddres = (u32)(&SPI_DL);
	sSPIRxDMAConfig.u32DestinationAddress = 0UL;
	sSPIRxDMAConfig.u32StatusByteCount = 0UL;
	sSPIRxDMAConfig.u32Control = DMA_DCR_EINT_MASK|DMA_DCR_DINC_MASK|DMA_DCR_SSIZE(0x02)|DMA_DCR_DSIZE(0x02)|DMA_DCR_D_REQ_MASK|!DMA_DCR_CS_MASK;
	sSPIRxDMAConfig.u32TriggerSource = DMA_TRIGGER_SPI1_RX;
	
	vfnDMA_Init(&sSPIRxDMAConfig,vfnSPIControl_DMARxCallback);
	
}

void vfnSPI16Control_TxBuffer(u16 * pu16TxBuffer, u16 u16DataSize)
{
        
	if(!(SPI_CONTROL_CHECK_STATUS(SPI_CONTROL_TX_IN_PROGRESS)))
	{
		SPI_CONTROL_SET_STATUS(SPI_CONTROL_TX_IN_PROGRESS);
				
		sSPITxDMAConfig.u32Control = SPI16_TX_DMA_CONFIG;
		DMA_UPDATE_CONTROL(sSPITxDMAConfig.u32Channel,sSPITxDMAConfig.u32Control);
		
		sSPITxDMAConfig.u32StatusByteCount = (u32)((u16DataSize*2)-1);
		DMA_UPDATE_BYTE_COUNT(sSPITxDMAConfig.u32Channel,sSPITxDMAConfig.u32StatusByteCount);
		
		SPI_DL = *pu16TxBuffer;
                SPI_DH = *pu16TxBuffer;
                  
		pu16TxBuffer++;		
		sSPITxDMAConfig.u32SourceAddres = (u32)pu16TxBuffer;
		DMA_UPDATE_SOURCE_ADDRESS(sSPITxDMAConfig.u32Channel,sSPITxDMAConfig.u32SourceAddres );
		
		DMA_ENABLE_ERQ(sSPITxDMAConfig.u32Channel);
		
		SPI1_ENABLE;
	}
	
	
}

void vfnSPI16Control_RxBuffer(u16 * pu16RxBuffer, u16 u16DataSize)
{
	
	if(!(SPI_CONTROL_CHECK_STATUS(SPI_CONTROL_RX_IN_PROGRESS)))
	{
		SPI_CONTROL_SET_STATUS(SPI_CONTROL_RX_IN_PROGRESS);
                
		sSPITxDMAConfig.u32Control = SPI16_TX_DUMMY_DMA_CONFIG;
		DMA_UPDATE_CONTROL(sSPITxDMAConfig.u32Channel,sSPITxDMAConfig.u32Control);
		
		sSPITxDMAConfig.u32StatusByteCount = (u32)(u16DataSize*2);
		DMA_UPDATE_BYTE_COUNT(sSPITxDMAConfig.u32Channel,sSPITxDMAConfig.u32StatusByteCount);
				
		sSPITxDMAConfig.u32SourceAddres = (u32)&gu16DummyTx;
		DMA_UPDATE_SOURCE_ADDRESS(sSPITxDMAConfig.u32Channel,sSPITxDMAConfig.u32SourceAddres );
		
		
		sSPIRxDMAConfig.u32StatusByteCount = (u32)(u16DataSize*2);
		DMA_UPDATE_BYTE_COUNT(sSPIRxDMAConfig.u32Channel,sSPIRxDMAConfig.u32StatusByteCount);
		
		
		sSPIRxDMAConfig.u32DestinationAddress = (u32)pu16RxBuffer;
		DMA_UPDATE_DESTINATION_ADDRESS(sSPIRxDMAConfig.u32Channel,sSPIRxDMAConfig.u32DestinationAddress );
		
		DMA_ENABLE_ERQ(sSPIRxDMAConfig.u32Channel);
		DMA_ENABLE_ERQ(sSPITxDMAConfig.u32Channel);
		SPI1_ENABLE;
	}
	
	
}

void vfnSPIControl_DMATxCallback(void)
{
	SPI_CONTROL_CLEAR_STATUS(SPI_CONTROL_TX_IN_PROGRESS);
	SPI_CONTROL_SET_STATUS(SPI_CONTROL_TX_DONE);
	DMA_DISABLE_ERQ(sSPITxDMAConfig.u32Channel);

	
}

void vfnSPIControl_DMARxCallback(void)
{
	SPI_CONTROL_CLEAR_STATUS(SPI_CONTROL_RX_IN_PROGRESS);
	SPI_CONTROL_SET_STATUS(SPI_CONTROL_RX_DONE);
	DMA_CLEAR_DONE(sSPITxDMAConfig.u32Channel);
	DMA_DISABLE_ERQ(sSPITxDMAConfig.u32Channel);
	DMA_DISABLE_ERQ(sSPIRxDMAConfig.u32Channel);

	
}
