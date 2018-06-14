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

#define XMIT_4800     0x00
#define XMIT_9600     0x01
#define XMIT_19200    0x02
#define XMIT_57600    0x03
#define XMIT_115200   0x04
#define XMIT_230400   0x05
#define XMIT_460800   0x06
#define XMIT_500000   0x07
#define XMIT_1000000  0x08
#define XMIT_2000000  0x09


#define PARITY_ERROR_Flag  1
#define FRAMING_ERROR_Flag 2
#define NOISE_ERROR_Flag 3
#define RX_OVERRUN_Flag 4

#define OSC_ER_CLK 0
#define FAST_IRC_CLK 1
#define PLL_CLK 2
#define FLL_CLK 3

char get_test_num(void) ;
char get_clk_src(void);
void uart0_test_init (UART0_MemMapPtr uartch, int sysclk, int baud);
void send_receive_verify(void);
void init_uart0_baud(int baud_rate, unsigned char clk_src);
