/*
	accessory decoder
	
	accessory implementation

	Copyright Jeroen Vreeken (jeroen@vreeken.net), 2024

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
#include <string.h>

#include "tick.h"
#include "dcc_decoder.h"


#define LED_BIT		PB5
#define LED_DDR 	DDRB
#define LED_PORT 	PORTB

#define LEARNING_BIT	PB4
#define LEARNING_DDR	DDRB
#define LEARNING_PORT	PORTB
#define LEARNING_PIN	PINB

// PD2 is used for dcc
// PC6 is used for reset

#define LED_R0_BIT	PC0
#define LED_R0_DDR	DDRC
#define LED_R0_PORT	PORTC
#define LED_G0_BIT	PC1
#define LED_G0_DDR	DDRC
#define LED_G0_PORT	PORTC

#define LED_R1_BIT	PC2
#define LED_R1_DDR	DDRC
#define LED_R1_PORT	PORTC
#define LED_G1_BIT	PC3
#define LED_G1_DDR	DDRC
#define LED_G1_PORT	PORTC

#define LED_R2_BIT	PC4
#define LED_R2_DDR	DDRC
#define LED_R2_PORT	PORTC
#define LED_G2_BIT	PC5
#define LED_G2_DDR	DDRC
#define LED_G2_PORT	PORTC

#define LED_R3_BIT	PD4
#define LED_R3_DDR	DDRD
#define LED_R3_PORT	PORTD
#define LED_G3_BIT	PD5
#define LED_G3_DDR	DDRD
#define LED_G3_PORT	PORTD

#define LED_R4_BIT	PD6
#define LED_R4_DDR	DDRD
#define LED_R4_PORT	PORTD
#define LED_G4_BIT	PD7
#define LED_G4_DDR	DDRD
#define LED_G4_PORT	PORTD



enum led_state {
	LED_OFF = 0x00,
	LED_RED = 0x01,
	LED_GREEN = 0x02,
	LED_GREEN_BLINK = 0x22,
	LED_GREEN_BLINK_WHITE = 0x26,
	LED_YELLOW = 0x03,
	LED_WHITE = 0x04,
	LED_YELLOW_WHITE = 0x07,
	LED_YELLOW_BLINK = 0x33,
	LED_YELLOW_WHITE_BLINK = 0x47,
};

static enum led_state aspect2state(uint8_t aspect)
{
	switch (aspect) {
		default:
		case 0:
			return LED_RED;
		case 1:
			return LED_YELLOW;
		case 2:
			return LED_YELLOW_WHITE;
		case 4:
			return LED_GREEN_BLINK;
		case 6:
			return LED_GREEN_BLINK_WHITE;
		case 16:
			return LED_GREEN;
		case 33:
			return LED_YELLOW_BLINK;
		case 34:
			return LED_YELLOW_WHITE_BLINK;
	}
}


static uint16_t * const config_decoder_address = 0;

uint16_t decoder_address;
static bool learning = false;

uint8_t state_aspect[5] = {0};


void dcc_handle_reset(uint8_t address_h, uint8_t address_l)
{
	if (address_h != DCC_ADDRESS_H_BROADCAST)
		return;
	if (address_l != DCC_ADDRESS_BROADCAST)
		return;
	
}

void dcc_handle_accessory_extended(uint16_t add, uint8_t aspect)
{
	if (learning && aspect) {
		decoder_address = add;
		eeprom_write_word(config_decoder_address, decoder_address);
		learning = false;
	}
	if (add == DCC_ACCESSORY_BROADCAST_ADDRESS) {
		memset(state_aspect, aspect, sizeof(aspect));
	}
	if (add >= decoder_address && add < decoder_address + sizeof(aspect)) {
		uint8_t nr = add - decoder_address;
		
		state_aspect[nr] = aspect;
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
	
	enum led_state leds;
	
	leds = aspect2state(state_aspect[0]);
	if (leds & 1) {
		LED_R0_PORT |= (1 << LED_R0_BIT);
	} else {
		LED_R0_PORT &= ~(1 << LED_R0_BIT);
	}
	if (leds & 2) {
		LED_G0_PORT |= (1 << LED_G0_BIT);
	} else {
		LED_G0_PORT &= ~(1 << LED_G0_BIT);
	}
	
	leds = aspect2state(state_aspect[1]);
	if (leds & 1) {
		LED_R1_PORT |= (1 << LED_R1_BIT);
	} else {
		LED_R1_PORT &= ~(1 << LED_R1_BIT);
	}
	if (leds & 2) {
		LED_G1_PORT |= (1 << LED_G1_BIT);
	} else {
		LED_G1_PORT &= ~(1 << LED_G1_BIT);
	}
	
	leds = aspect2state(state_aspect[2]);
	if (leds & 1) {
		LED_R2_PORT |= (1 << LED_R2_BIT);
	} else {
		LED_R2_PORT &= ~(1 << LED_R2_BIT);
	}
	if (leds & 2) {
		LED_G2_PORT |= (1 << LED_G2_BIT);
	} else {
		LED_G2_PORT &= ~(1 << LED_G2_BIT);
	}
	
	leds = aspect2state(state_aspect[3]);
	if (leds & 1) {
		LED_R3_PORT |= (1 << LED_R3_BIT);
	} else {
		LED_R3_PORT &= ~(1 << LED_R3_BIT);
	}
	if (leds & 2) {
		LED_G3_PORT |= (1 << LED_G3_BIT);
	} else {
		LED_G3_PORT &= ~(1 << LED_G3_BIT);
	}
	
	leds = aspect2state(state_aspect[4]);
	if (leds & 1) {
		LED_R4_PORT |= (1 << LED_R4_BIT);
	} else {
		LED_R4_PORT &= ~(1 << LED_R4_BIT);
	}
	if (leds & 2) {
		LED_G4_PORT |= (1 << LED_G4_BIT);
	} else {
		LED_G4_PORT &= ~(1 << LED_G4_BIT);
	}
}


static void led_init(void)
{
	// Set the LED_BIT as output
	LED_DDR |= (1 << LED_BIT);

	// Turn LED off
	LED_PORT &= ~(1 << LED_BIT);


	// R and G leds
	LED_R0_DDR |= (1 << LED_R0_BIT);
	LED_G0_DDR |= (1 << LED_G0_BIT);

	LED_R1_DDR |= (1 << LED_R1_BIT);
	LED_G1_DDR |= (1 << LED_G1_BIT);

	LED_R2_DDR |= (1 << LED_R2_BIT);
	LED_G2_DDR |= (1 << LED_G2_BIT);

	LED_R3_DDR |= (1 << LED_R3_BIT);
	LED_G3_DDR |= (1 << LED_G3_BIT);

	LED_R4_DDR |= (1 << LED_R4_BIT);
	LED_G4_DDR |= (1 << LED_G4_BIT);

	LED_R0_PORT &= ~(1 << LED_R0_BIT);
	LED_G0_PORT &= ~(1 << LED_G0_BIT);

	LED_R1_PORT &= ~(1 << LED_R1_BIT);
	LED_G1_PORT &= ~(1 << LED_G1_BIT);

	LED_R2_PORT &= ~(1 << LED_R2_BIT);
	LED_G2_PORT &= ~(1 << LED_G2_BIT);

	LED_R3_PORT &= ~(1 << LED_R3_BIT);
	LED_G3_PORT &= ~(1 << LED_G3_BIT);

	LED_R4_PORT &= ~(1 << LED_R4_BIT);
	LED_G4_PORT &= ~(1 << LED_G4_BIT);
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


int main(void)
{
	wdt_reset();
	
	led_init();
	learning_init();
	tick_init();
	dcc_decoder_init();
	
	decoder_address = eeprom_read_word(config_decoder_address);
	
	sei();
	
	while (1) {
		wdt_reset();
		learning_status();
		dcc_decoder_handle();
		led_status();

	}
	
	return 0;
}
