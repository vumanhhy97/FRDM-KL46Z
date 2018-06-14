/****************************************************************************************************/
/**
Copyright (c) 2008 Freescale Semiconductor
Freescale Confidential Proprietary
\file       DMA.c
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
#include "DMA_spi.h"
/*****************************************************************************************************
* Declaration of module wide FUNCTIONs - NOT for use in other modules
*****************************************************************************************************/
static u32 u32fnDMA_MUXInit(u32 u32Channel, u32 u32Source);
static void ( * vfnDMA_Callback[DMA_MAX_CHANNELS])(void);
/*****************************************************************************************************
* Definition of module wide MACROs / #DEFINE-CONSTANTs - NOT for use in other modules
*****************************************************************************************************/
/*!
    \def DMA_FIRST_IRQ
    \brief First DMA IRQ number
*/
#define DMA_FIRST_IRQ	(INT_DMA0)
/*!
    \def DMA_MAX_CHANNELS
    \brief Max supported DMA channels
*/
#define DMA_MAX_CHANNELS	(DMA_MAX_CHANNELS)
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
void vfnDMA_Init(sDMAConfig * spDMAChannel, void(* vfnDMAChCallback)(void))
{
	u32 u32Channel;
	u16 u16DMA_IRQ;
	
	SIM_SCGC7 = SIM_SCGC7_DMA_MASK;
	SIM_SCGC6 |= SIM_SCGC6_DMAMUX_MASK;
	
	u32Channel = spDMAChannel->u32Channel;
	
	if(DMA_MAX_CHANNELS > u32Channel)
	{
	
		DMA_SAR(u32Channel) = spDMAChannel->u32SourceAddres;
		DMA_DAR(u32Channel) = spDMAChannel->u32DestinationAddress;
		DMA_DSR_BCR(u32Channel) = spDMAChannel->u32StatusByteCount;
		DMA_DCR(u32Channel) = spDMAChannel->u32Control;
		
		if(spDMAChannel->u32Control & DMA_DCR_EINT_MASK)
		{
			if(vfnDMAChCallback != NULL)
			{
				vfnDMA_Callback[u32Channel] = vfnDMAChCallback;
				u16DMA_IRQ = DMA_FIRST_IRQ + (u16)u32Channel;
				enable_irq(u16DMA_IRQ-16);
			}
		}
		
		u32fnDMA_MUXInit(u32Channel,spDMAChannel->u32TriggerSource);
	}	
	
}

void vfnDMA_UpdateSAR(sDMAConfig * spDMAChannel)
{
	u32 u32Channel;
	u32Channel = spDMAChannel->u32Channel;
	
	if(DMA_MAX_CHANNELS > u32Channel)
	{
	
		DMA_SAR(u32Channel) = spDMAChannel->u32SourceAddres;
	}
}

void vfnDMA_UpdateDAR(sDMAConfig * spDMAChannel)
{
	u32 u32Channel;
	u32Channel = spDMAChannel->u32Channel;
	
	if(DMA_MAX_CHANNELS > u32Channel)
	{
	
		DMA_DAR(u32Channel) = spDMAChannel->u32DestinationAddress;
	}
}

void vfnDMA_UpdateDSRBSR(sDMAConfig * spDMAChannel)
{
	u32 u32Channel;
	u32Channel = spDMAChannel->u32Channel;
	
	if(DMA_MAX_CHANNELS > u32Channel)
	{
	
		DMA_DSR_BCR(u32Channel) = spDMAChannel->u32StatusByteCount;
	}
}

void vfnDMA_UpdateDCR(sDMAConfig * spDMAChannel)
{
	u32 u32Channel;
	u32Channel = spDMAChannel->u32Channel;
	
	if(DMA_MAX_CHANNELS > u32Channel)
	{
	
		DMA_DCR(u32Channel) = spDMAChannel->u32Control;
	}
}
/*!
      \fn static u32 u32fnDMA_MUXInit(u32 u32Channel, u32 u32Source)
      \brief  Initializes the DMA MUX with the desired source
      \param  u32Channel: DMA channel to be configured
      \param  u32Source: Trigger source
      \return OK if configurations was successful, ERROR otherwise
*/
static u32 u32fnDMA_MUXInit(u32 u32Channel, u32 u32Source)
{
	u32 u32Status = ERROR;
	/* Make sure the trigger selected is in range */
	if(DMA_TRIGGER_UNSUPPORT_TRIGGER > u32Source)
	{
		
		DMAMUX0_CHCFG(u32Channel) = DMAMUX_CHCFG_ENBL_MASK|DMAMUX_CHCFG_SOURCE(u32Source); 
		u32Status = OK;
	}

	return(u32Status);
}
/*!
      \fn void DMA0_IRQHandler (void)
      \brief  DMA Channel interrupt with user callback
      \return None
*/
void DMA0_IRQHandler (void)
{
	
	DMA_DSR_BCR(0) |= DMA_DSR_BCR_DONE_MASK;
	vfnDMA_Callback[0]();
}
/*!
      \fn void DMA1_IRQHandler (void)
      \brief  DMA Channel interrupt with user callback
      \return None
*/
void DMA1_IRQHandler (void)
{
	
	DMA_DSR_BCR(1) |= DMA_DSR_BCR_DONE_MASK;
	vfnDMA_Callback[1]();
}
/*!
      \fn void DMA2_IRQHandler (void)
      \brief  DMA Channel interrupt with user callback
      \return None
*/
void DMA2_IRQHandler (void)
{
	
	DMA_DSR_BCR(2) |= DMA_DSR_BCR_DONE_MASK;
	vfnDMA_Callback[2]();
}
/*!
      \fn void DMA3_IRQHandler (void)
      \brief  DMA Channel interrupt with user callback
      \return None
*/
void DMA3_IRQHandler (void)
{
	
	DMA_DSR_BCR(3) |= DMA_DSR_BCR_DONE_MASK;
	vfnDMA_Callback[3]();
}
