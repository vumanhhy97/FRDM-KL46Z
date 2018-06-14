/*
 * File:		pdb0_ch1_ftm.c
 * Purpose:		Main process
 *
 */

#include "common.h"

#ifdef CMSIS
#include "start.h"
#endif


/********************************************************************/
int main (void)
{
    	char ch;
        
#ifdef CMSIS  // If we are conforming to CMSIS, we need to call start here
    start();
#endif
        
  	printf("\n\rRunning the pdb0_ch1_ftm project.\n\r");

	while(1)
	{
		ch = in_char();
		out_char(ch);
	} 
}
/********************************************************************/
