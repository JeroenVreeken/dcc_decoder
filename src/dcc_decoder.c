/*
	dcc decoder
	
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
/*
				min	nom	max
				(us)	(us)	(us)
	half bit (1)		55	58	61
	received half bit (1)	52	58	64
	half bit (0)		95	100	9900
	received half bit (0)	90	100	10000
*/
#include "dcc_decoder.h"
#include <string.h>

#include <avr/interrupt.h>
#include <avr/io.h>

#include "tick.h"


#define DCC_HBIT_1_MIN_US	52
#define DCC_HBIT_1_MAX_US	64

#define DCC_HBIT_0_MIN_US	90
#define DCC_HBIT_0_MAX_US	10000


// substract 1 for minimal edge case
#define DCC_HBIT_1_MIN_TICKS	((DCC_HBIT_1_MIN_US / TICK_US) -1)
#define DCC_HBIT_0_MIN_TICKS	((DCC_HBIT_0_MIN_US / TICK_US) -1)

#define DCC_HBIT_1_MAX_TICKS	(DCC_HBIT_1_MAX_US / TICK_US)
#define DCC_HBIT_0_MAX_TICKS	(DCC_HBIT_0_MAX_US / TICK_US)


enum dcc_decoder_state {
	DCC_DECODER_STATE_PREAMBLE_WAIT,	// Waiting to receive preamble
	DCC_DECODER_STATE_PREAMBLE_0H,		// First half bit of preample received, keep preample order!!!
	DCC_DECODER_STATE_PREAMBLE_1,
	DCC_DECODER_STATE_PREAMBLE_1H,
	DCC_DECODER_STATE_PREAMBLE_2,
	DCC_DECODER_STATE_PREAMBLE_2H,
	DCC_DECODER_STATE_PREAMBLE_3,
	DCC_DECODER_STATE_PREAMBLE_3H,
	DCC_DECODER_STATE_PREAMBLE_4,
	DCC_DECODER_STATE_PREAMBLE_4H,
	DCC_DECODER_STATE_PREAMBLE_5,
	DCC_DECODER_STATE_PREAMBLE_5H,
	DCC_DECODER_STATE_PREAMBLE_6,
	DCC_DECODER_STATE_PREAMBLE_6H,
	DCC_DECODER_STATE_PREAMBLE_7,
	DCC_DECODER_STATE_PREAMBLE_7H,
	DCC_DECODER_STATE_PREAMBLE_8,
	DCC_DECODER_STATE_PREAMBLE_8H,
	DCC_DECODER_STATE_PREAMBLE_9,
	DCC_DECODER_STATE_PREAMBLE_9H,
	DCC_DECODER_STATE_PREAMBLE_END,		// Minimal preamble received
	DCC_DECODER_STATE_START_H,		// Half of start bit received
	DCC_DECODER_STATE_BIT0_WAIT,		// Keep bit order intact!!!
	DCC_DECODER_STATE_BIT0_1,
	DCC_DECODER_STATE_BIT0_0,
	DCC_DECODER_STATE_BIT1_WAIT,
	DCC_DECODER_STATE_BIT1_1,
	DCC_DECODER_STATE_BIT1_0,
	DCC_DECODER_STATE_BIT2_WAIT,
	DCC_DECODER_STATE_BIT2_1,
	DCC_DECODER_STATE_BIT2_0,
	DCC_DECODER_STATE_BIT3_WAIT,
	DCC_DECODER_STATE_BIT3_1,
	DCC_DECODER_STATE_BIT3_0,
	DCC_DECODER_STATE_BIT4_WAIT,
	DCC_DECODER_STATE_BIT4_1,
	DCC_DECODER_STATE_BIT4_0,
	DCC_DECODER_STATE_BIT5_WAIT,
	DCC_DECODER_STATE_BIT5_1,
	DCC_DECODER_STATE_BIT5_0,
	DCC_DECODER_STATE_BIT6_WAIT,
	DCC_DECODER_STATE_BIT6_1,
	DCC_DECODER_STATE_BIT6_0,
	DCC_DECODER_STATE_BIT7_WAIT,
	DCC_DECODER_STATE_BIT7_1,
	DCC_DECODER_STATE_BIT7_0,
	DCC_DECODER_STATE_START_WAIT,		// Expect next start bit (KEEP AFTER BIT7 !!!)
};

static uint8_t dcc_decoder_state = DCC_DECODER_STATE_PREAMBLE_WAIT;



#define DCC_PACKET_MAX	6

static volatile uint8_t dcc_packet[DCC_PACKET_MAX];
static volatile int8_t dcc_packet_len;
static volatile bool dcc_packet_received = false;


static void dcc_toggle(tick_t t_tick);



ISR(INT0_vect)
{
	dcc_toggle(tick());
}



// DCC input signal
static void dcc_toggle(tick_t t_tick)
{
	static tick_t prev_tick = 0;
	tick_t diff_tick = t_tick - prev_tick;
	
	bool is_1 = false;
	bool is_0 = false;
	
	if (diff_tick <= 1) {
		// ignore fast inversions as 'noise'
		return;
	}
	prev_tick = t_tick;

	if (diff_tick >= DCC_HBIT_1_MIN_TICKS && diff_tick <= DCC_HBIT_1_MAX_TICKS) {
		is_1 = true;
	} else if (diff_tick >= DCC_HBIT_0_MIN_TICKS && diff_tick <= DCC_HBIT_0_MAX_TICKS) {
		is_0 = true;
	}
	
	switch (dcc_decoder_state) {
		case DCC_DECODER_STATE_PREAMBLE_WAIT:
		case DCC_DECODER_STATE_PREAMBLE_0H:
		case DCC_DECODER_STATE_PREAMBLE_1:
		case DCC_DECODER_STATE_PREAMBLE_1H:
		case DCC_DECODER_STATE_PREAMBLE_2:
		case DCC_DECODER_STATE_PREAMBLE_2H:
		case DCC_DECODER_STATE_PREAMBLE_3:
		case DCC_DECODER_STATE_PREAMBLE_3H:
		case DCC_DECODER_STATE_PREAMBLE_4:
		case DCC_DECODER_STATE_PREAMBLE_4H:
		case DCC_DECODER_STATE_PREAMBLE_5:
		case DCC_DECODER_STATE_PREAMBLE_5H:
		case DCC_DECODER_STATE_PREAMBLE_6:
		case DCC_DECODER_STATE_PREAMBLE_6H:
		case DCC_DECODER_STATE_PREAMBLE_7:
		case DCC_DECODER_STATE_PREAMBLE_7H:
		case DCC_DECODER_STATE_PREAMBLE_8:
		case DCC_DECODER_STATE_PREAMBLE_8H:
		case DCC_DECODER_STATE_PREAMBLE_9:
		case DCC_DECODER_STATE_PREAMBLE_9H:
			if (is_1) {
				dcc_decoder_state++;
			} else {
				dcc_decoder_state = DCC_DECODER_STATE_PREAMBLE_WAIT;
			}
			break;
		case DCC_DECODER_STATE_PREAMBLE_END:
			if (is_1) {
				// Stay in state;
			} else if (is_0  && !dcc_packet_received) {
				dcc_packet_len = 0;
				dcc_packet[0] = 0;
				dcc_decoder_state = DCC_DECODER_STATE_START_H;
			} else {
				// junk or last packet not handled yet
				dcc_decoder_state = DCC_DECODER_STATE_PREAMBLE_WAIT;
			}
			break;
		case DCC_DECODER_STATE_START_WAIT:
			dcc_packet_len++;
			if (is_0) {
				if (dcc_packet_len == DCC_PACKET_MAX) {
					dcc_decoder_state = DCC_DECODER_STATE_PREAMBLE_WAIT;
				} else {
					dcc_decoder_state = DCC_DECODER_STATE_START_H;
					dcc_packet[dcc_packet_len] = 0;
				}
			} else {
				if (is_1) {
					dcc_decoder_state = DCC_DECODER_STATE_PREAMBLE_0H;
					dcc_packet_received = true;
				} else {
					dcc_decoder_state = DCC_DECODER_STATE_PREAMBLE_WAIT;
				}
			}
			break;
		case DCC_DECODER_STATE_START_H:
			if (is_0) {
				dcc_decoder_state = DCC_DECODER_STATE_BIT0_WAIT;
			} else {
				dcc_decoder_state = DCC_DECODER_STATE_PREAMBLE_WAIT;
			}
			break;
		case DCC_DECODER_STATE_BIT0_WAIT:
		case DCC_DECODER_STATE_BIT1_WAIT:
		case DCC_DECODER_STATE_BIT2_WAIT:
		case DCC_DECODER_STATE_BIT3_WAIT:
		case DCC_DECODER_STATE_BIT4_WAIT:
		case DCC_DECODER_STATE_BIT5_WAIT:
		case DCC_DECODER_STATE_BIT6_WAIT:
		case DCC_DECODER_STATE_BIT7_WAIT: {
			uint8_t cur_byte = dcc_packet[dcc_packet_len];
			cur_byte <<= 1;

			if (is_1) {
				cur_byte |= 1;
				dcc_decoder_state += DCC_DECODER_STATE_BIT0_1 - DCC_DECODER_STATE_BIT0_WAIT;
			} else if (is_0) {
				dcc_decoder_state += DCC_DECODER_STATE_BIT0_0 - DCC_DECODER_STATE_BIT0_WAIT;
			} else {
				dcc_decoder_state = DCC_DECODER_STATE_PREAMBLE_WAIT;
			}
			
			dcc_packet[dcc_packet_len] = cur_byte;
			break;
		}
		case DCC_DECODER_STATE_BIT0_1:
		case DCC_DECODER_STATE_BIT1_1:
		case DCC_DECODER_STATE_BIT2_1:
		case DCC_DECODER_STATE_BIT3_1:
		case DCC_DECODER_STATE_BIT4_1:
		case DCC_DECODER_STATE_BIT5_1:
		case DCC_DECODER_STATE_BIT6_1:
		case DCC_DECODER_STATE_BIT7_1:
			if (is_1) {
				dcc_decoder_state += DCC_DECODER_STATE_BIT1_WAIT - DCC_DECODER_STATE_BIT0_1;
			} else {
				dcc_decoder_state = DCC_DECODER_STATE_PREAMBLE_WAIT;
			}
			break;
		case DCC_DECODER_STATE_BIT0_0:
		case DCC_DECODER_STATE_BIT1_0:
		case DCC_DECODER_STATE_BIT2_0:
		case DCC_DECODER_STATE_BIT3_0:
		case DCC_DECODER_STATE_BIT4_0:
		case DCC_DECODER_STATE_BIT5_0:
		case DCC_DECODER_STATE_BIT6_0:
		case DCC_DECODER_STATE_BIT7_0:
			if (is_0) {
				dcc_decoder_state += DCC_DECODER_STATE_BIT1_WAIT - DCC_DECODER_STATE_BIT0_0;
			} else {
				dcc_decoder_state = DCC_DECODER_STATE_PREAMBLE_WAIT;
			}
			break;
		default:
			dcc_decoder_state = DCC_DECODER_STATE_PREAMBLE_WAIT;
	}
}


// Copy of the received packet
static uint8_t dcc_packet_handle[DCC_PACKET_MAX];


void dcc_decoder_handle(void)
{
	if (dcc_packet_received) {
		int8_t len = dcc_packet_len;
		
		// check bytes
		uint8_t check = 0;
		int8_t i;
		for (i = 0; i < len; i++) {
			uint8_t packet_byte = dcc_packet[i];
			
			check ^= packet_byte;
			
			dcc_packet_handle[i] = packet_byte;
		}
		// Release receive buffer
		dcc_packet_received = false;

		if (check == 0x00) { 
			// valid packet
			
			uint8_t address = dcc_packet_handle[0];
			
			uint8_t address_type = address & DCC_ADDRESS_BASE_MASK;
			uint8_t pos = 0;
			uint8_t address_h = 0;
			uint8_t address_l;
			
			switch (address_type) {
				case DCC_ADDRESS_BASE_ACCESSORY: {
					uint16_t add = address & ~DCC_ADDRESS_BASE_MASK;
					// combine address bits
					uint8_t add_h = (~dcc_packet_handle[1]) & DCC_ACCESSORY_ADDH_MASK;
					add_h >>= DCC_ACCESSORY_ADDH_SHIFT;
					add |= add_h * 64;
					uint8_t pair = dcc_packet_handle[1] & DCC_ACCESSORY_PAIR_MASK;
					pair >>= DCC_ACCESSORY_PAIR_SHIFT;

					if ((dcc_packet_handle[1] & DCC_ACCESSORY_SIGN) && (len == 3)) {
						uint8_t output = dcc_packet_handle[1] & DCC_ACCESSORY_OUTPUT_MASK;
						bool value = dcc_packet_handle[1] & DCC_ACCESSORY_ACTIVATE;

						dcc_handle_accessory_basic(add, pair, output, value);

					} else if ((dcc_packet_handle[1] & DCC_ACCESSORY_EXTENDED_MASK) == DCC_ACCESSORY_EXTENDED_ASPECT) {
						if (add == 0)
							add = 512;
						uint16_t output_address = add * 4 - 3 + pair;
						if (len == 4) {
							dcc_handle_accessory_extended(output_address, dcc_packet_handle[2]);
						} else if ((len == 6) && ((dcc_packet_handle[2] & DCC_ACCESSORY_EXTENDED_CV_MASK) == DCC_ACCESSORY_EXTENDED_CV_OMP)) {
							//   2         3         4
							//  (1110-CCVV VVVV-VVVV DDDD-DDDD)

							uint16_t cv = 1 + dcc_packet_handle[3] + ((dcc_packet_handle[2] & DCC_CV_VARIABLE_HIGH_MASK) << 8);
							uint8_t type = dcc_packet_handle[2] & DCC_CV_INSTRUCTION_TYPE_MASK;
							uint8_t data = dcc_packet_handle[4];

								
							switch (type) {
								case DCC_CV_VERIFY_BYTE:
									break;
								case DCC_CV_WRITE_BYTE:
									dcc_handle_accessory_extended_cv_write(output_address, cv, data);
									break;
								case DCC_CV_BIT_MANIPULATE: {
									uint8_t bit = data & DCC_CV_BIT_MANIPULATE_BIT_MASK;
									bool value = data & DCC_CV_BIT_MANIPULATE_VAL_MASK;
									uint8_t bittype = data & DCC_CV_BIT_MANIPULATE_MASK;
									switch (bittype) {
										case DCC_CV_BIT_MANIPULATE_VERIFY:
											break;
										case DCC_CV_BIT_MANIPULATE_WRITE:
											dcc_handle_accessory_extended_cv_writebit(output_address, cv, bit, value);
										break;
									}
									break;
								}
							}
						}
					}
					break;
				}
				case DCC_ADDRESS_BASE_LONG:
					address_h = address;
					pos++;
					// fallthrough
				case DCC_ADDRESS_BASE_SHORT: {
					address_l = dcc_packet_handle[pos];
					uint8_t instruction = dcc_packet_handle[pos+1] & DCC_INSTRUCTION_MASK;
					

					switch (instruction) {
						case DCC_INSTRUCTION_CTRL: {
							uint8_t ctrl = dcc_packet_handle[pos] & DCC_CTRL_MASK;
							
							switch (ctrl) {
								case DCC_CTRL_RESET:
									dcc_handle_reset(address_h, address_l);
							}
							break;
						}

						case DCC_INSTRUCTION_SPEED_F: {
							uint8_t speed = dcc_packet_handle[pos+1] & DCC_SPEED_MASK;
							dcc_handle_multifunction_speed(address_h, address_l, speed, true);
							break;
						}
						case DCC_INSTRUCTION_SPEED_R: {
							uint8_t speed = dcc_packet_handle[pos+1] & DCC_SPEED_MASK;
							dcc_handle_multifunction_speed(address_h, address_l, speed, false);
							break;
						}

						case DCC_INSTRUCTION_ADVANCED: {
							uint8_t sub = dcc_packet_handle[pos+1] & DCC_ADVANCED_SUB_MASK;
							
							switch (sub) {
								case DCC_ADVANCED_SPEEDSTEP: {
									uint8_t speed = dcc_packet_handle[pos+2] & DCC_SPEEDSTEP_SPEED_MASK;
									uint8_t direction = dcc_packet_handle[pos+2] & DCC_SPEEDSTEP_DIRECTION_MASK;
									dcc_handle_multifunction_speedstep(address_h, address_l, speed, direction);
									break;
								}
							}
							break;
						}
						
						case DCC_INSTRUCTION_FG1: {
							bool fl = dcc_packet_handle[pos+1] & DCC_FG1_FL;
							bool f1 = dcc_packet_handle[pos+1] & DCC_FG1_F1;
							bool f2 = dcc_packet_handle[pos+1] & DCC_FG1_F2;
							bool f3 = dcc_packet_handle[pos+1] & DCC_FG1_F3;
							bool f4 = dcc_packet_handle[pos+1] & DCC_FG1_F4;
							dcc_handle_multifunction_fg1(address_h, address_l, fl, f1, f2, f3, f4);
							break;
						}
						
						case DCC_INSTRUCTION_CV: {
							// Only support long form
							if ((dcc_packet_handle[pos+1] & DCC_CV_SHORT_FORM) == 0) {
								uint16_t cv = 1 + dcc_packet_handle[pos+2] + ((dcc_packet_handle[pos+1] & DCC_CV_VARIABLE_HIGH_MASK) << 8);
								uint8_t type = dcc_packet_handle[pos+1] & DCC_CV_INSTRUCTION_TYPE_MASK;
								uint8_t data = dcc_packet_handle[pos+3];
								
								switch (type) {
									case DCC_CV_VERIFY_BYTE:
										break;
									case DCC_CV_WRITE_BYTE:
										dcc_handle_multifunction_cv_write(address_h, address_l, cv, data);
										break;
									case DCC_CV_BIT_MANIPULATE: {
										uint8_t bit = data & DCC_CV_BIT_MANIPULATE_BIT_MASK;
										bool value = data & DCC_CV_BIT_MANIPULATE_VAL_MASK;
										uint8_t bittype = data & DCC_CV_BIT_MANIPULATE_MASK;
										switch (bittype) {
											case DCC_CV_BIT_MANIPULATE_VERIFY:
												break;
											case DCC_CV_BIT_MANIPULATE_WRITE:
												dcc_handle_multifunction_cv_writebit(address_h, address_l, cv, bit, value);
												break;
										}
										break;
									}
								}
							}
							break;
						}
					}
				}
			}
		}
	}
}




#if defined (__AVR_ATmega328P__) || defined (__AVR_ATmega328__) || defined(UNITTEST)



void dcc_decoder_init(void)
{
	// PD2 as input
	DDRD &= ~(1 << PD2);
	// pull up
	PORTD |= (1 << PD2); 
	
	// INT0 on any edge
	EICRA |= (1<<ISC00);
	
	// enable INT0
	EIMSK |= (1<<INT0);

	// note: still needs sei()!!!
}


#elif defined (__AVR_ATtiny25__) || defined (__AVR_ATtiny45__) || defined (__AVR_ATtiny85__)


void dcc_decoder_init(void)
{
	// PB2 as input
	DDRB &= ~(1 << PB2);
	// pull up
	//PORTB |= (1 << PB2); 
	// no pull up
	PORTB &= ~(1 << PB2);
	
	// INT0 on any edge
	MCUCR |= (1<<ISC00);
	
	// enable INT0
	GIMSK |= (1<<INT0);

	// note: still needs sei()!!!
}


#endif
