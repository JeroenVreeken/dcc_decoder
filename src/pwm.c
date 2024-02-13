/*
	pwm

	Copyright Jeroen Vreeken (jeroen@vreeken.net), 2023

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.

 */

#include <avr/io.h>

#include "pwm.h"

#define OC0A_PIN PB0
#define OC0B_PIN PB1
#define OC1B_PIN PB4
#define OC0A_DDR DDRB
#define OC0B_DDR DDRB
#define OC1B_DDR DDRB

#define T0_PRESCALER_1		(                        (1 << CS00))
#define T0_PRESCALER_8		(            (1 << CS01)            )
#define T0_PRESCALER_64		(            (1 << CS01)|(1 << CS00))
#define T0_PRESCALER_256	((1 << CS02)                        )
#define T0_PRESCALER_1024	((1 << CS02)|            (1 << CS00))

#define T0_A_FAST_PWM		((1 << COM0A1)                )
#define T0_A_FAST_PWM_INV	((1 << COM0A1) | (1 << COM0A0))
#define T0_B_FAST_PWM		((1 << COM0B1)                )
#define T0_B_FAST_PWM_INV	((1 << COM0B1) | (1 << COM0B0))

#define T1_A_FAST_PWM		((1 << COM1A1)                )
#define T1_A_FAST_PWM_INV	((1 << COM1A1) | (1 << COM1A0))
#define T1_B_FAST_PWM		((1 << COM1B1)                )
#define T1_B_FAST_PWM_INV	((1 << COM1B1) | (1 << COM1B0))



void pwm_init(void)
{

	// set timer0 at 62.5kHz
	
	TCCR0A = (1 << WGM00) | (1 << WGM01) | T0_A_FAST_PWM_INV | T0_B_FAST_PWM_INV;
	TCCR0B = T0_PRESCALER_1;
	OCR0A = 255;
	OCR0B = 255; // duty cycle 0


	// Pin config
	OC0A_DDR |= (1 << OC0A_PIN);
	OC0B_DDR |= (1 << OC0B_PIN);

#if defined(CONFIG_OC1B_PWM)
	// Do not set TCCR1 prescaler (used by tick)
	// Do set COM1A due to an attiny85 bug
	TCCR1 |= T1_A_FAST_PWM_INV;
	
	GTCCR |= (1 << PWM1B) | T1_B_FAST_PWM_INV;
	OCR1B = 255;
	OCR1C = 255;

	OC1B_DDR |= (1 << OC1B_PIN);
#endif
}


void pwm_set(uint8_t output, uint8_t pwm)
{
	switch (output) {
		case 0:
			OCR0A = ~pwm;
			break;
		case 1:
			OCR0B = ~pwm;
			break;
#if defined(CONFIG_OC1B_PWM)
		case 3:
			OCR1B = ~pwm;
			break;
#endif
	}
}
