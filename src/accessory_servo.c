/*
	accessory decoder
	
	accessory implementation

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

#include <avr/wdt.h>
#include <avr/eeprom.h>

#include "tick.h"
#include "pwm.h"
#include "dcc_decoder.h"


#define LED_BIT		PB5
#define LED_DDR 	DDRB
#define LED_PORT 	PORTB

#define LEARNING_BIT	PB4
#define LEARNING_DDR	DDRB
#define LEARNING_PORT	PORTB
#define LEARNING_PIN	PINB



static uint16_t * const config_decoder_address = 0;

uint16_t decoder_address;
static bool learning = false;

#define PAIR_NR 4

enum pair_state {
	PAIR_IDLE = 0,
	PAIR_OUT0 = 1,
	PAIR_OUT1 = 2,
	PAIR_MASK = 3, PAIR_CONFLICT = 3, // both selected -> conflict
	PAIR_GOTO0 = 4,
	PAIR_GOTO1 = 8,
	PAIR_GOTO = 12,
};



struct pair {
	uint8_t state;
	uint8_t pwm[2];
	uint8_t pwm_cur;
	uint8_t pwm_prev;
	tick_ms_t t;
} pairs[PAIR_NR];

#define GOTO_T_MS	500


void dcc_handle_reset(uint8_t address_h, uint8_t address_l)
{
	if (address_h != DCC_ADDRESS_H_BROADCAST)
		return;
	if (address_l != DCC_ADDRESS_BROADCAST)
		return;
	
	int i;
	for (i = 0; i < PAIR_NR; i++) {
		pairs[i].state = PAIR_IDLE;
	}
}

void dcc_handle_accessory_basic(uint16_t add, uint8_t pair, uint8_t output, bool value)
{
	if (learning && value) {
		decoder_address = add;
		eeprom_write_word(config_decoder_address, decoder_address);
		learning = false;
	}
	if (add == decoder_address || add == DCC_ACCESSORY_BROADCAST_ADDRESS) {
		uint8_t state = pairs[pair].state;
		uint8_t state_masked = state & PAIR_MASK;
		if (output == 0) {
			if (value) {
				if (state_masked != PAIR_OUT0) {
					state |= PAIR_GOTO0;
					uint8_t pwm = pairs[pair].pwm_cur;
					if (!pwm)
						pwm = pairs[pair].pwm[0];
					pairs[pair].pwm_prev = pwm;
					pairs[pair].t = tick_ms();
				}
				state |= PAIR_OUT0;
				state &= ~PAIR_GOTO1;
			} else {
				state &= ~PAIR_OUT0;
			}
		}
		if (output == 1) {
			if (value) {
				if (state_masked != PAIR_OUT1) {
					state |= PAIR_GOTO1;
					uint8_t pwm = pairs[pair].pwm_cur;
					if (!pwm)
						pwm = pairs[pair].pwm[1];
					pairs[pair].pwm_prev = pwm;
					pairs[pair].t = tick_ms();
				}
				state |= PAIR_OUT1;
				state &= ~PAIR_GOTO0;
			} else {
				state &= ~PAIR_OUT1;
			}
		}
		pairs[pair].state = state;
	}
}



static void led_status(void)
{
	if (learning || (pairs[0].state & PAIR_GOTO)) {
		// Turn LED on
		LED_PORT |= (1 << LED_BIT);
	} else {
		// Turn LED off
		LED_PORT &= ~(1 << LED_BIT);
	}
}


static void led_init(void)
{
	// Set the LED_BIT as output
	LED_DDR |= (1 << LED_BIT);

	// Turn LED off
	LED_PORT &= ~(1 << LED_BIT);
}

static void learning_init(void)
{
	// Learning pin as input
	LEARNING_DDR &= ~(1 << LEARNING_BIT);
	// pull up
	LEARNING_PORT |= (1 << LEARNING_BIT);

}

static void adc_init(void)
{
	// Vref = AVcc
	ADMUX |= (1 << REFS0);
	// prescater to 128 (16MHz / 128 = 125kHz ADC clock)
	// must be between 50kHz and 200kHz
	ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0) | (1 << ADEN);
}

static void adc_start_read(int8_t channel)
{
	// select ADC channel
	ADMUX = (ADMUX & 0xF0) | (channel & 0x0f);
	
	// single conversion
	ADCSRA |= (1 << ADSC);
}

static bool adc_done(void)
{
	return !(ADCSRA & (1 << ADSC));
}


static void learning_status(void)
{
	learning |= !(LEARNING_PIN & (1 << LEARNING_BIT));
}


int main(void)
{
	uint8_t adc_channel = 0;
	
	wdt_reset();
	
	led_init();
	learning_init();
	tick_init();
	dcc_decoder_init();
	pwm_init();
	adc_init();
	
	decoder_address = eeprom_read_word(config_decoder_address);
	
	sei();
	
	while (1) {
		wdt_reset();
		tick_handle();
		learning_status();
		dcc_decoder_handle();
		led_status();

		// Check adc status
		if (adc_done()) {
			uint8_t pair = adc_channel / 2;
			uint8_t output = adc_channel & 1;
			
			uint8_t pwm = ADC >> 2;
			pairs[pair].pwm[output] = pwm;

			adc_channel++;
			adc_channel &= 7;
			adc_start_read(adc_channel);
		}
		
		int i;
		for (i = 0; i < PAIR_NR; i++) {
			uint8_t state = pairs[i].state;
			uint8_t state_masked = state & PAIR_MASK;
			
			uint8_t pwm = 0;
			
			if (state_masked == PAIR_OUT0) {
				pwm = pairs[i].pwm[0];
			}
			if (state_masked == PAIR_OUT1) {
				pwm = pairs[i].pwm[1];
			}
			if (state == PAIR_IDLE) {
				pwm = 0;
			}
			if (state_masked == PAIR_CONFLICT) {
				pwm = 0;
			} else if (state & PAIR_GOTO) {
				long diff = pwm - pairs[i].pwm_prev;
				int16_t t = tick_ms() - pairs[i].t;
				if (t <= GOTO_T_MS) {
					pwm = pairs[i].pwm_prev + ((diff * t) / GOTO_T_MS);
				} else {
					pairs[i].state = state_masked;
				}
			}

			if (pwm)
				pairs[i].pwm_cur = pwm;
			pwm_set(i, pwm);
		}
	}
	
	return 0;
}
