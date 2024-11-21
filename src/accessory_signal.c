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
#include <string.h>

#include "tick.h"
#include "dcc_decoder.h"
#include "cv.h"


#define LED_BIT		PB5
#define LED_DDR 	DDRB
#define LED_PORT 	PORTB

#define LEARNING_BIT	PB4
#define LEARNING_DDR	DDRB
#define LEARNING_PORT	PORTB
#define LEARNING_PIN	PINB

// PD2 is used for dcc
// PC6 is used for reset

struct signal_led {
	volatile uint8_t *port_r;
	volatile uint8_t *port_g;
	volatile uint8_t *ddr_r;
	volatile uint8_t *ddr_g;
	uint8_t bit_r;
	uint8_t bit_g;
};

static const struct signal_led signal_leds[] = {
	{ .bit_r = 1 << PC0, .bit_g = 1 << PC1, .port_r = &PORTC, .port_g = &PORTC, .ddr_r = &DDRC, .ddr_g = &DDRC },
	{ .bit_r = 1 << PC2, .bit_g = 1 << PC3, .port_r = &PORTC, .port_g = &PORTC, .ddr_r = &DDRC, .ddr_g = &DDRC },
	{ .bit_r = 1 << PC4, .bit_g = 1 << PC5, .port_r = &PORTC, .port_g = &PORTC, .ddr_r = &DDRC, .ddr_g = &DDRC },
	{ .bit_r = 1 << PD4, .bit_g = 1 << PD5, .port_r = &PORTD, .port_g = &PORTD, .ddr_r = &DDRD, .ddr_g = &DDRD },
	{ .bit_r = 1 << PD6, .bit_g = 1 << PD7, .port_r = &PORTD, .port_g = &PORTD, .ddr_r = &DDRD, .ddr_g = &DDRD },
	{ .bit_r = 1 << PB0, .bit_g = 1 << PB1, .port_r = &PORTB, .port_g = &PORTB, .ddr_r = &DDRB, .ddr_g = &DDRB },
};

#define NR_SIGNALS	(sizeof(signal_leds)/sizeof(signal_leds[0]))

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
		case DCC_ASPECT_STOP:
			return LED_RED;
		case DCC_ASPECT_EXPECT_STOP:
			return LED_YELLOW;
		case DCC_ASPECT_BRAKE_INDICATED:
			return LED_YELLOW_WHITE;
		case DCC_ASPECT_PASS_40KMH:
			return LED_GREEN_BLINK;
		case DCC_ASPECT_PASS_INDICATED:
			return LED_GREEN_BLINK_WHITE;
		case DCC_ASPECT_PASS:
			return LED_GREEN;
		case DCC_ASPECT_PROCEED_ON_SIGHT:
			return LED_YELLOW_BLINK;
		case DCC_ASPECT_INITIATE_BRAKING:
			return LED_YELLOW_WHITE_BLINK;
	}
}


uint16_t decoder_address;
static bool learning = false;

uint8_t state_aspect[NR_SIGNALS] = {0};


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
		cv_address_accessory_extended_set(add);
		learning = false;
	}
	if (add == DCC_ACCESSORY_BROADCAST_ADDRESS) {
		memset(state_aspect, aspect, NR_SIGNALS);
	}
	if (add >= decoder_address && add < decoder_address + NR_SIGNALS) {
		uint8_t nr = add - decoder_address;
		
		state_aspect[nr] = aspect;
	}
}

#define BLINK_ON_MS	400
#define BLINK_MS	800
static tick_ms_t prev_ms;



static void led_status(void)
{
	tick_ms_t ms_now = tick_ms();
	tick_ms_t blink_ms = ms_now - prev_ms;
	uint8_t pwm = tick() >> 2;

	if (blink_ms >= BLINK_MS) {
		prev_ms += BLINK_MS;
		blink_ms -= BLINK_MS;
	}

	bool blink;
	
	if (blink_ms >= BLINK_ON_MS) {
		blink = false;
	} else {
		blink = true;
	}


	if (learning) {
		// Turn LED on
		LED_PORT |= (1 << LED_BIT);
	} else {
		// Turn LED off
		LED_PORT &= ~(1 << LED_BIT);
	}
	
	enum led_state leds[NR_SIGNALS];

	int i;
	for (i = 0; i < NR_SIGNALS; i++) {
		enum led_state state = aspect2state(state_aspect[i]);
		if (blink)
			state ^= state >> 4;
		uint8_t pwm1 = cv_read_byte(CV_MANUFACTURER + i * 2 + 0);
		uint8_t pwm2 = cv_read_byte(CV_MANUFACTURER + i * 2 + 1);
		state &= (pwm <= pwm1 ? 1 : 0) | (pwm <= pwm2 ? 2 : 0);
		leds[i] = state;

		if (leds[i] & 1) {
			*signal_leds[i].port_r |= signal_leds[i].bit_r;
		} else {
			*signal_leds[i].port_r &= ~signal_leds[i].bit_r;
		}
		if (leds[i] & 2) {
			*signal_leds[i].port_g |= signal_leds[i].bit_g;
		} else {
			*signal_leds[i].port_g &= ~signal_leds[i].bit_g;
		}
	}
	
}


static void led_init(void)
{
	// Set the LED_BIT as output
	LED_DDR |= (1 << LED_BIT);

	// Turn LED off
	LED_PORT &= ~(1 << LED_BIT);

	int i;

	for (i = 0; i < NR_SIGNALS; i++) {
		// R and G leds
		*signal_leds[i].ddr_r |= signal_leds[i].bit_r;
		*signal_leds[i].ddr_g |= signal_leds[i].bit_g;
		*signal_leds[i].port_r &= ~signal_leds[i].bit_r;
		*signal_leds[i].port_g &= ~signal_leds[i].bit_g;
	}
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
	
	decoder_address = cv_address_accessory_extended_get();
	
	sei();
	
	while (1) {
		wdt_reset();
		tick_handle();
		learning_status();
		dcc_decoder_handle();
		led_status();

	}
	
	return 0;
}
