#include <avr/io.h>
#include <avr/pgmspace.h>
#include "uart.h"


void UART_Init(void)
{
	// baud rate 19200
	UBRR0H = HI(bauddivider);
	UBRR0L = LO(bauddivider);

	UCSR0A = 0x00;
	// разрешаем прием и передачу
	UCSR0B = (1 << TXEN0)|(1 << RXEN0);
	// 8-битовая посылка
	UCSR0C = (1 << UCSZ00)|(1 << UCSZ01);
}

void send_UART(unsigned char c)
{
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = c;
}

void send_UART_str(unsigned char *s)
{
	while (*s != 0x00)
		send_UART(*s++);
}

void send_UART_str_P(const unsigned char *s)
{
	while (pgm_read_byte(s) != 0x00)
		send_UART(pgm_read_byte(s++));
}
