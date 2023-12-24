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


#define OC0A_PIN PD6
#define OC0B_PIN PD5
#define OC2A_PIN PB3
#define OC2B_PIN PD3
#define OC0A_DDR DDRD
#define OC0B_DDR DDRD
#define OC2A_DDR DDRB
#define OC2B_DDR DDRD

#define T0_PRESCALER_1		(                        (1 << CS00))
#define T0_PRESCALER_8		(            (1 << CS01)            )
#define T0_PRESCALER_64		(            (1 << CS01)|(1 << CS00))
#define T0_PRESCALER_256	((1 << CS02)                        )
#define T0_PRESCALER_1024	((1 << CS02)|            (1 << CS00))

#define T0_A_FAST_PWM		((1 << COM0A1)                )
#define T0_A_FAST_PWM_INV	((1 << COM0A1) | (1 << COM0A0))
#define T0_B_FAST_PWM		((1 << COM0B1)                )
#define T0_B_FAST_PWM_INV	((1 << COM0B1) | (1 << COM0B0))

#define T2_PRESCALER_1		(                        (1 << CS20))
#define T2_PRESCALER_8		(            (1 << CS21)            )
#define T2_PRESCALER_32		(            (1 << CS21)|(1 << CS20))
#define T2_PRESCALER_64		((1 << CS22)                        )
#define T2_PRESCALER_128	((1 << CS22)|            (1 << CS20))
#define T2_PRESCALER_256	((1 << CS22)|(1 << CS21)            )
#define T2_PRESCALER_1024	((1 << CS22)|(1 << CS21)|(1 << CS20))

#define T2_A_FAST_PWM		((1 << COM2A1)                )
#define T2_A_FAST_PWM_INV	((1 << COM2A1) | (1 << COM2A0))
#define T2_B_FAST_PWM		((1 << COM2B1)                )
#define T2_B_FAST_PWM_INV	((1 << COM2B1) | (1 << COM2B0))


/*
	@16Mhz
	Prescale	F		F/256		F/512		Step	Steps	Steps
			Hz		Hz		Hz		us	1500us	3000us
	   1		16000000	62500		31250		0.0625	  -	  -
	   8		 2000000	 7812.50	 3906.25	0.5	  -	  -
	  64		  250000	  976.563	  488.281	4	375.00	750
	 256		   62500	  244.141	  122.070	16	 93.75	187.5
	1024		   15625	   61.035	   30.518	64	 23.438	 46.875

 */


void pwm_init(void)
{

	// set timer0 at 244Hz
	
	TCCR0A = (1 << WGM00) | (1 << WGM01) | T0_A_FAST_PWM_INV | T0_B_FAST_PWM_INV;
	TCCR0B = T0_PRESCALER_256;
	OCR0A = 255;
	OCR0B = 255; // duty cycle 0

	// set timer2 at 244Hz
	
	TCCR2A = (1 << WGM20) | (1 << WGM21) | T2_A_FAST_PWM_INV | T2_B_FAST_PWM_INV;
	TCCR2B = T2_PRESCALER_256;
	OCR2A = 255;
	OCR2B = 255; // duty cycle 0


	// Pin config
	OC0A_DDR |= (1 << OC0A_PIN);
	OC0B_DDR |= (1 << OC0B_PIN);
	OC2A_DDR |= (1 << OC2A_PIN);
	OC2B_DDR |= (1 << OC2B_PIN);
}


void pwm_set(uint8_t pair, uint8_t pwm)
{
	switch (pair) {
		case 0:
			OCR0A = ~pwm;
			break;
		case 1:
			OCR0B = ~pwm;
			break;
		case 2:
			OCR2A = ~pwm;
			break;
		case 3: 
			OCR2B = ~pwm;
			break;
	}
}
