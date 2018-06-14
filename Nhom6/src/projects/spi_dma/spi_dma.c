/*
 * File:		spi_dma.c
 * Purpose:		This code exercises SPI tranfers with DMA module enabled
 *                      at this time code is able to transfer at 8 or 16 bit
 *                      data length. FIFO enabled.
 *                      HW used: TWR-KL46Z48M.
 *
 */


#include "common.h"
#include "SPI_Control.h"

#ifdef CMSIS
#include "start.h"
#endif

/************************* Instructions****************************************
Use "#define SPI_TEST (1)"  to select 8-bit data transmission length.
Use "#define SPI_TEST (2)"  to select 16-bit data transmission length.

Use only 1 definition at time.
*******************************************************************************/
//#define SPI_TEST	(1)
#define SPI_TEST	(2)


/*****************************************************************************************************
* Definition of module wide MACROs / #DEFINE-CONSTANTs - NOT for use in other modules
*****************************************************************************************************/
#define BUFFER_SIZE	(10)

/*****************************************************************************************************
* Definition of module wide VARIABLEs - NOT for use in other modules
*****************************************************************************************************/

u8 gau8TestArray1[BUFFER_SIZE] =
{
	0xAA,
	0xFF,
	0x00,
	0xAA,
	0xFF,
	0x00,
	0xAA,
	0xFF,
	0x00,
	0xAA
};

u8 gau8RxTest[BUFFER_SIZE] = 
{
	0xFF,
	0x01,
	0x02,
	0x03,
	0x04,
	0x05,
	0x06,
	0x07,
	0x08,
	0x09,
};

u16 gau16TestArray1[BUFFER_SIZE] =
{
	0xAA55,
	0x55AA,
	0x0000,
	0xAAAA,
	0xFFFF,
	0x0000,
	0xAAAA,
	0xFFFF,
	0x0000,
	0xAAAA
};

u16 gau16RxTest[BUFFER_SIZE] = 
{
	0x0000,
	0x0001,
	0x0002,
	0x0003,
	0x0004,
	0x0005,
	0x0006,
	0x0007,
	0x0008,
	0x0009,
};



/********************************************************************/
int main (void)
{
    	char ch;
        u32 u32FillBuffer; //SPI
        
#ifdef CMSIS  // If we are conforming to CMSIS, we need to call start here
    start();
#endif
        
        #if (SPI_TEST == 1)
        vfnSPI8Control_Init();	
        #endif
        #if (SPI_TEST == 2)
        vfnSPI16Control_Init();	
        #endif	

        printf("\n\rRunning the spi_dma project.\n\r");

	while(1)
	{
	  
          #if (SPI_TEST == 1)
	  if(!(SPI_CONTROL_CHECK_STATUS(SPI_CONTROL_RX_IN_PROGRESS)))
	  {
            u32FillBuffer = 0;
			
   	    while(u32FillBuffer <BUFFER_SIZE)
   	    {
              gau8RxTest[u32FillBuffer] = 0x7;
              u32FillBuffer++;
	    }
			
   	    vfnSPI8Control_RxBuffer(&gau8RxTest[0],8);		
	  }
	  #endif
          
          #if (SPI_TEST == 2)
	  if(!(SPI_CONTROL_CHECK_STATUS(SPI_CONTROL_RX_IN_PROGRESS)))
	  {
            u32FillBuffer = 0;
			
   	    while(u32FillBuffer <BUFFER_SIZE)
   	    {
              gau16RxTest[u32FillBuffer] = 0x7777;
              u32FillBuffer++;
	    }
			
   	    vfnSPI16Control_RxBuffer(&gau16RxTest[0],4);		
	  }
	  #endif 
	} 
}
/********************************************************************/