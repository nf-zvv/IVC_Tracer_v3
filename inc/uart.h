#ifndef _UART_H_
#define _UART_H_

//#define F_CPU 8000000L
#define HI(x) ((x)>>8)
#define LO(x) ((x)&0xFF)
#define baudrate 19200L
#define bauddivider (F_CPU/(16*baudrate)-1)

void UART_Init(void);
void send_UART(unsigned char c);
void send_UART_str(unsigned char *s);
void send_UART_str_P(const unsigned char *s);

#endif