/* ADC
 *
 *
 */

#ifndef __ADC_H__
#define __ADC_H__

void ADC_Init(void);

/** Measurement voltage power
 *
 *  \return The voltage in millivolts.
 */
uint16_t readVcc(void);

uint16_t readTemp(void);

uint16_t adc_read(uint8_t ch);

#endif