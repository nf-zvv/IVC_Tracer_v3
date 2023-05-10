#include <avr/io.h>
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/delay.h>
#include "uart.h"
#include "spi.h"
#include "ST7586S.h"

#define LED_PORT PORTB
#define LED_DDR  DDRB
#define LED      PB7

ISR(TIMER1_COMPB_vect)
{
	// сбрасываем счетчик
	TCNT1 = 0;
	// инвертируем бит (Светодиод)
	LED_PORT ^= (1 << LED);
}

void Timer1_Init(void)
{
	// Период срабатывания прерывания таймера 500 мс
	// 15625 = 500 ms /(1000/(F_CPU/256))
	OCR1B = 15625;
	// Разрешаем прерывание таймера по совпадению
	TIMSK1 = (1 << OCIE1B);
	// Предделитель 256
	TCCR1B = (1 << CS12);
}

int main(void)
{
	// Светодиод
	LED_DDR = 1 << LED;
	LED_PORT = 0;

	Timer1_Init();
	UART_Init();

	// monochrome mode
	ST7586S_Initalize(ST7586S_GRAYSCALE_MODE);
	ST7586S_Fill(ST7586S_COLOR_WHITE);
	
	/* init hardware pins */
    spi_init();

	ST7586S_WriteString(0,0,"Hello world!\0",FONT_NORMAL,ST7586S_COLOR_WHITE,ST7586S_COLOR_BLACK);

    sei();

    while (1)
    {
        
    }
 
    return 0;
}