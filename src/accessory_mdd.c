/*
	accessory decoder
	
	accessory implementation

	Copyright Jeroen Vreeken (jeroen@vreeken.net), 2026

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
#include "dcc_decoder.h"

/*

				ATTINY2313A
	6N137

	7	EN
	6	DCC		6	PD2, INT0
	
	L2930NE

	1	1,2EN		
	2	1A		19	PB7
	7	2A		18	PB6
	9	3,4EN		
	10	3A		17	PB5
	15	4A		16	PB4
		
	L2930NE	
	
	1	1,2EN		
	2	1A		15	PB3
	7	2A		14	PB2
	9	3,4EN		
	10	3A		13	PB1
	15	4A		12	PB0
	
	button			2	PD0
	led R			
	led Y An		3	PD1

*/


#define LED_BIT		PD1
#define LED_DDR 	DDRD
#define LED_PORT 	PORTD

#define LEARNING_BIT	PD0
#define LEARNING_DDR	DDRD
#define LEARNING_PORT	PORTD
#define LEARNING_PIN	PIND

#define OUT_DDR		DDRB
#define OUT_PORT	PORTB


static uint16_t * const config_decoder_address = 0;

uint16_t decoder_address;
static bool learning = false;

#define PAIR_NR 4

enum {
	PAIR_IDLE = 0,
	PAIR_OUT0 = 1,
	PAIR_OUT1 = 2,
	PAIR_PULSE0 = 4,
	PAIR_ON0 = 5,
	PAIR_PULSE1 = 8,
	PAIR_ON1 = 10,
	PAIR_PULSE = 12,
};

struct pair {
	uint8_t state;
	tick_ms_t t;
} pairs[PAIR_NR];

#define PULSE_T_MS	2500


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
		
		if (value) {
			if (state == PAIR_IDLE) {
				pairs[pair].t = tick_ms();
				if (output == 0)
					state |= PAIR_PULSE0;
				else
					state |= PAIR_PULSE1;
			}
			if (output == 0) {
				state |= PAIR_OUT0;
			} else {
				state |= PAIR_OUT1;
			}
		} else {
			if (output == 0) {
				state &= ~PAIR_OUT0;
			} else {
				state &= ~PAIR_OUT1;
			}
		}
		pairs[pair].state = state;
	}
}



static void led_status(void)
{
	if (learning) {
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


static void learning_status(void)
{
	learning |= !(LEARNING_PIN & (1 << LEARNING_BIT));
}

static void out_init(void)
{
	OUT_DDR = 0xff;
	OUT_PORT = 0xff;
}

int main(void)
{
	wdt_reset();
	
	led_init();
	out_init();
	learning_init();
	tick_init();
	dcc_decoder_init();
	
	decoder_address = eeprom_read_word(config_decoder_address);
	
	sei();
	
	while (1) {
		wdt_reset();
		tick_handle();
		learning_status();
		dcc_decoder_handle();
		led_status();

		uint8_t out0 = 128;
		uint8_t out1 = 64;
		int i;
		for (i = 0; i < PAIR_NR; i++) {
			uint8_t state = pairs[i].state;
			
			if (state & PAIR_ON0) {
				OUT_PORT &= ~out0;
			} else {
				OUT_PORT |= out0;
			}
			if (state & PAIR_ON1) {
				OUT_PORT &= ~out1;
			} else {
				OUT_PORT |= out1;
			}
			
			int16_t t = tick_ms() - pairs[i].t;
			if (t > PULSE_T_MS) {
				state &= ~PAIR_PULSE;
			}

			pairs[i].state = state;
			out0 >>= 2;
			out1 >>= 2;
		}
	}
	
	return 0;
}
