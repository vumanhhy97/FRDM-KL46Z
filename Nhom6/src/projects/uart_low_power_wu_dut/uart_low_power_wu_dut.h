/*
 * File:		uart_low_power_wu_dut.h
 * Purpose:		KL46 testing of uart receive
 *                      where we wake up from VLPS or LLS with a received
 *                      character and catch 1st character.
 * I/O:                  Uart 0 UART0_RX(pin 11) and UART0_TX (pin 9) J2  
 *                      Terminal - on REVA KL46 FRDM Board UART1
 */

#define LP_MODE_LLS 0
#define LP_MODE_VLPS 1

#define FAST_IRC_FREQ 4000000
//#define SLOW_IRC_FREQ 39062
#define SLOW_IRC_FREQ 32768
#define PLL_FREQ 48000000
#define UNDEF_VALUE 0xFF
#define VLPS_EN_OSCERCLK_57600      0x00
#define VLPS_EN_OSCERCLK_115200     0x01
#define VLPS_EN_OSCERCLK_230400     0x02
#define VLPS_EN_OSCERCLK_460800     0x03
#define VLPS_EN_OSCERCLK_1000000    0x04
#define VLPS_EN_OSCERCLK_2000000    0x05
#define VLPS_EN_FASTIRC_CLK_4800    0x06
#define VLPS_EN_FASTIRC_CLK_9600    0x07
#define VLPS_EN_FASTIRC_CLK_19200   0x08
#define VLPS_EN_FASTIRC_CLK_57600   0x09
#define VLPS_EN_FASTIRC_CLK_115200  0x0A
#define VLPS_EN_FASTIRC_CLK_230400  0x0B
#define VLPS_EN_FASTIRC_CLK_500000  0x0C
#define VLPS_EN_FASTIRC_CLK_1000000 0x0D
#define VLPS_FEI_9600   0x0E
#define LLS_FEI_19200  0x0F
#define LLS_FEI_57600  0x10
#define LLS_FEI_115200 0x11
#define LLS_FEI_230400 0x12
#define LLS_FEI_460800 0x13
#define LLS_FEI_921600 0x14
#define LLS_FEI_4800   0x15

#define PARITY_ERROR_Flag  1
#define FRAMING_ERROR_Flag 2
#define NOISE_ERROR_Flag 3
#define RX_OVERRUN_Flag 4
char get_test_num(void) ;
void uart0_test_init (UART0_MemMapPtr uartch, int sysclk, int baud);
