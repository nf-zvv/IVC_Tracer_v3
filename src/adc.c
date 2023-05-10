#include <avr/io.h>
#include <avr/delay.h>
#include "adc.h"

void ADC_Init(void)
{
	// MUX0  = 3 - выбор канала ADC3 (кнопки)
	// REFS0 = 1 - выбор ИОН AVcc с конденсатором на AREF
	// ADLAR = 1 - выравнивание результата по левому краю,
	// это значит что ADCH содержит биты 9...2, а ADCL - биты 1, 0
	ADMUX = (1 << REFS0);

	// ADTS0 = 0 - свободный запуск
	//       = 4 - запуск по переполнению Таймера/Счетчика0
	//       = 5 - запуск по совпадению Таймера/Счетчика1 B
	//       = 6 - запуск по переполнению Таймера/Счетчика1
	//ADCSRB = (0<<ADTS0);

	// ADC3D = 1 - отключить цифровые входы, соответствующие входам ADC0,1,2,3
	DIDR0 = (1<<ADC0D)|(1<<ADC1D)|(1<<ADC2D)|(1<<ADC3D);

	// ADEN  = 1 - включение АЦП
	// ADIE  = 1 - разрешение прерывания от АЦП
	// ADSC  = 1 - запуск преобразования
	// ADATE = 1 - автозапуск включен (непрерывные последовательные преобразования, одно за другим)
	// ADPS0 = 6 - предделитель тактовой частоты (CLK/64). Итого частота АЦП 125 кГц 
	ADCSRA = (1<<ADEN)|(6<<ADPS0);
}

uint16_t readVcc(void)
{
    // Read 1.1V reference against AVcc
    // set the reference to Vcc and the measurement to the internal 1.1V reference
    ADMUX = (1 << REFS0)|(1 << MUX3)|(1 << MUX2)|(1 << MUX1);

    // Wait for Vref to settle
    _delay_ms(75);

    ADCSRA |= (1 << ADSC); // Start single conversion

    // wait for conversion to complete
    // ADSC becomes ’0′ again
    // till then, run loop continuously
    while(ADCSRA & (1<<ADSC));

    uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH  
    uint8_t high = ADCH; // unlocks both

    uint16_t result = (high<<8) | low;

    result = 1125300UL / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
    return result; // Vcc in millivolts
}

uint16_t readTemp(void)
{
    // Set internal 1.1V reference and ADC channel No.3
    ADMUX = (1 << REFS1)|(1 << REFS0)|(1 << MUX1)|(1 << MUX0);

    // Wait for Vref to settle (нужно ли? проверить)
    _delay_ms(75);

    ADCSRA |= (1 << ADSC); // Start single conversion

    // wait for conversion to complete
    // ADSC becomes ’0′ again
    // till then, run loop continuously
    while(ADCSRA & (1<<ADSC));

    //uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH  
    //uint8_t high = ADCH; // unlocks both
    //uint16_t result = (high<<8) | low;
    // Или всего одной строкой:
    uint16_t result = ADC;

    int8_t temperature = (result * 107L - 50000L) / 1000L; // Calculate Temperature
    return temperature; // temperature in celsius
}

// read adc value
uint16_t adc_read(uint8_t ch)
{
    // select the corresponding channel 0~7
    // ANDing with '7' will always keep the value
    // of 'ch' between 0 and 7
    ch &= 0b00000111;  // AND operation with 7
    ADMUX = (ADMUX & 0xF8)|ch;     // clears the bottom 3 bits before ORing
 
    // start single conversion
    // write '1' to ADSC
    ADCSRA |= (1<<ADSC);
 
    // wait for conversion to complete
    // ADSC becomes '0' again
    // till then, run loop continuously
    while(ADCSRA & (1<<ADSC));
 
    return (ADC);
}

/* Not used
ISR(ADC_vect) {
    //считываем старший регистр АЦП
    uint8_t AdcVal = ADCH;

}
*/