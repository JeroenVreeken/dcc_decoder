/*
	dcc decoder unittests
	
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
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define F_CPU 16000000

uint32_t DDRD;
uint32_t EICRA;
uint32_t EIMSK;
uint32_t PORTD;
uint32_t TCCR1A;
uint32_t TCCR1B;
uint32_t TCCR1C;
uint32_t TCNT1;


int tick_us = 1;
#define TICK_US tick_us

#define ISR(name) void (name)(void)


/************************************/
#include "dcc_decoder.c"
/************************************/



struct test_step {
	enum dcc_decoder_state state_in;
	enum dcc_decoder_state state_out;
	uint16_t tick;

	uint8_t packet[DCC_PACKET_MAX];
	int8_t len;
	bool received_in;
	bool received_out;
};

struct test {
	char *name;
	int nr;
	int tick_us;
	struct test_step *steps;
};

struct test tests[] = {
	{ "Not a preamble", 1, 1, (struct test_step[1]){
			{ DCC_DECODER_STATE_PREAMBLE_WAIT, 	DCC_DECODER_STATE_PREAMBLE_WAIT, 3 }
		}
	
	},
	{ "Not a preamble min", 1, 1, (struct test_step[1]){
			{ DCC_DECODER_STATE_PREAMBLE_WAIT, 	DCC_DECODER_STATE_PREAMBLE_WAIT, 50 }
		}
	
	},
	{ "Not a preamble 4us min", 1, 4, (struct test_step[1]){
			{ DCC_DECODER_STATE_PREAMBLE_WAIT, 	DCC_DECODER_STATE_PREAMBLE_WAIT, 11 }
		}
	
	},
	{ "Not a preamble max", 1, 1, (struct test_step[1]){
			{ DCC_DECODER_STATE_PREAMBLE_WAIT, 	DCC_DECODER_STATE_PREAMBLE_WAIT, 65 }
		}
	
	},
	{ "Not a preamble 4us max", 1, 4, (struct test_step[1]){
			{ DCC_DECODER_STATE_PREAMBLE_WAIT, 	DCC_DECODER_STATE_PREAMBLE_WAIT, 17 }
		}
	
	},
	{ "Preamble", 20, 1, (struct test_step[20]){
			{ DCC_DECODER_STATE_PREAMBLE_WAIT, 	DCC_DECODER_STATE_PREAMBLE_0H, 	52 },
			{ DCC_DECODER_STATE_PREAMBLE_0H,	DCC_DECODER_STATE_PREAMBLE_1, 	55 },
			{ DCC_DECODER_STATE_PREAMBLE_1,		DCC_DECODER_STATE_PREAMBLE_1H, 	58 },
			{ DCC_DECODER_STATE_PREAMBLE_1H,	DCC_DECODER_STATE_PREAMBLE_2, 	61 },
			{ DCC_DECODER_STATE_PREAMBLE_2,		DCC_DECODER_STATE_PREAMBLE_2H, 	64 },
			{ DCC_DECODER_STATE_PREAMBLE_2H,	DCC_DECODER_STATE_PREAMBLE_3, 	61 },
			{ DCC_DECODER_STATE_PREAMBLE_3,		DCC_DECODER_STATE_PREAMBLE_3H, 	58 },
			{ DCC_DECODER_STATE_PREAMBLE_3H,	DCC_DECODER_STATE_PREAMBLE_4, 	58 },
			{ DCC_DECODER_STATE_PREAMBLE_4,		DCC_DECODER_STATE_PREAMBLE_4H, 	58 },
			{ DCC_DECODER_STATE_PREAMBLE_4H,	DCC_DECODER_STATE_PREAMBLE_5, 	58 },
			{ DCC_DECODER_STATE_PREAMBLE_5,		DCC_DECODER_STATE_PREAMBLE_5H, 	58 },
			{ DCC_DECODER_STATE_PREAMBLE_5H,	DCC_DECODER_STATE_PREAMBLE_6, 	58 },
			{ DCC_DECODER_STATE_PREAMBLE_6,		DCC_DECODER_STATE_PREAMBLE_6H, 	58 },
			{ DCC_DECODER_STATE_PREAMBLE_6H,	DCC_DECODER_STATE_PREAMBLE_7, 	58 },
			{ DCC_DECODER_STATE_PREAMBLE_7,		DCC_DECODER_STATE_PREAMBLE_7H, 	58 },
			{ DCC_DECODER_STATE_PREAMBLE_7H,	DCC_DECODER_STATE_PREAMBLE_8, 	58 },
			{ DCC_DECODER_STATE_PREAMBLE_8,		DCC_DECODER_STATE_PREAMBLE_8H, 	58 },
			{ DCC_DECODER_STATE_PREAMBLE_8H,	DCC_DECODER_STATE_PREAMBLE_9, 	58 },
			{ DCC_DECODER_STATE_PREAMBLE_9,		DCC_DECODER_STATE_PREAMBLE_9H, 	58 },
			{ DCC_DECODER_STATE_PREAMBLE_9H,	DCC_DECODER_STATE_PREAMBLE_END,	58 },
		}
	
	},
	{ "Preamble 4us", 20, 4, (struct test_step[20]){
			{ DCC_DECODER_STATE_PREAMBLE_WAIT, 	DCC_DECODER_STATE_PREAMBLE_0H, 	12 },
			{ DCC_DECODER_STATE_PREAMBLE_0H,	DCC_DECODER_STATE_PREAMBLE_1, 	13 },
			{ DCC_DECODER_STATE_PREAMBLE_1,		DCC_DECODER_STATE_PREAMBLE_1H, 	14 },
			{ DCC_DECODER_STATE_PREAMBLE_1H,	DCC_DECODER_STATE_PREAMBLE_2, 	15 },
			{ DCC_DECODER_STATE_PREAMBLE_2,		DCC_DECODER_STATE_PREAMBLE_2H, 	16 },
			{ DCC_DECODER_STATE_PREAMBLE_2H,	DCC_DECODER_STATE_PREAMBLE_3, 	15 },
			{ DCC_DECODER_STATE_PREAMBLE_3,		DCC_DECODER_STATE_PREAMBLE_3H, 	15 },
			{ DCC_DECODER_STATE_PREAMBLE_3H,	DCC_DECODER_STATE_PREAMBLE_4, 	15 },
			{ DCC_DECODER_STATE_PREAMBLE_4,		DCC_DECODER_STATE_PREAMBLE_4H, 	15 },
			{ DCC_DECODER_STATE_PREAMBLE_4H,	DCC_DECODER_STATE_PREAMBLE_5, 	15 },
			{ DCC_DECODER_STATE_PREAMBLE_5,		DCC_DECODER_STATE_PREAMBLE_5H, 	15 },
			{ DCC_DECODER_STATE_PREAMBLE_5H,	DCC_DECODER_STATE_PREAMBLE_6, 	15 },
			{ DCC_DECODER_STATE_PREAMBLE_6,		DCC_DECODER_STATE_PREAMBLE_6H, 	15 },
			{ DCC_DECODER_STATE_PREAMBLE_6H,	DCC_DECODER_STATE_PREAMBLE_7, 	15 },
			{ DCC_DECODER_STATE_PREAMBLE_7,		DCC_DECODER_STATE_PREAMBLE_7H, 	15 },
			{ DCC_DECODER_STATE_PREAMBLE_7H,	DCC_DECODER_STATE_PREAMBLE_8, 	14 },
			{ DCC_DECODER_STATE_PREAMBLE_8,		DCC_DECODER_STATE_PREAMBLE_8H, 	14 },
			{ DCC_DECODER_STATE_PREAMBLE_8H,	DCC_DECODER_STATE_PREAMBLE_9, 	14 },
			{ DCC_DECODER_STATE_PREAMBLE_9,		DCC_DECODER_STATE_PREAMBLE_9H, 	14 },
			{ DCC_DECODER_STATE_PREAMBLE_9H,	DCC_DECODER_STATE_PREAMBLE_END,	14 },
		}
	
	},
	{ "Preamble end, half start", 2, 1, (struct test_step[2]){
			{ DCC_DECODER_STATE_PREAMBLE_END, 	DCC_DECODER_STATE_START_H, 		100 },
			{ DCC_DECODER_STATE_START_H, 		DCC_DECODER_STATE_PREAMBLE_WAIT,	58 },
		}
	
	},
	{ "Preamble end, previous not handled", 1, 1, (struct test_step[1]){
			{ DCC_DECODER_STATE_PREAMBLE_END, 	DCC_DECODER_STATE_PREAMBLE_WAIT, 	100, .received_in = true },
		}
	
	},
	{ "Preamble end, start", 2, 1, (struct test_step[2]){
			{ DCC_DECODER_STATE_PREAMBLE_END, 	DCC_DECODER_STATE_START_H, 		100 },
			{ DCC_DECODER_STATE_START_H, 		DCC_DECODER_STATE_BIT0_WAIT,		100 },
		}
	
	},
	{ "Preamble end, start minimal", 2, 1, (struct test_step[2]){
			{ DCC_DECODER_STATE_PREAMBLE_END, 	DCC_DECODER_STATE_START_H, 		90 },
			{ DCC_DECODER_STATE_START_H, 		DCC_DECODER_STATE_BIT0_WAIT,		90 },
		}
	
	},
	{ "Preamble end, start maximal odd", 2, 1, (struct test_step[2]){
			{ DCC_DECODER_STATE_PREAMBLE_END, 	DCC_DECODER_STATE_START_H, 		10000 },
			{ DCC_DECODER_STATE_START_H, 		DCC_DECODER_STATE_BIT0_WAIT,		 2000 },
		}
	
	},
	{ "Preamble end, start maximal even", 2, 1, (struct test_step[2]){
			{ DCC_DECODER_STATE_PREAMBLE_END, 	DCC_DECODER_STATE_START_H, 		 2000 },
			{ DCC_DECODER_STATE_START_H, 		DCC_DECODER_STATE_BIT0_WAIT,		10000 },
		}
	
	},
	{ "Idle", 54, 1, (struct test_step[54]){
			{ DCC_DECODER_STATE_BIT0_WAIT, 		DCC_DECODER_STATE_BIT0_1, 		58  },
			{ DCC_DECODER_STATE_BIT0_1, 		DCC_DECODER_STATE_BIT1_WAIT,		58  },
			{ DCC_DECODER_STATE_BIT1_WAIT, 		DCC_DECODER_STATE_BIT1_1, 		58  },
			{ DCC_DECODER_STATE_BIT1_1, 		DCC_DECODER_STATE_BIT2_WAIT,		58  },
			{ DCC_DECODER_STATE_BIT2_WAIT, 		DCC_DECODER_STATE_BIT2_1, 		58  },
			{ DCC_DECODER_STATE_BIT2_1, 		DCC_DECODER_STATE_BIT3_WAIT,		58  },
			{ DCC_DECODER_STATE_BIT3_WAIT, 		DCC_DECODER_STATE_BIT3_1, 		58  },
			{ DCC_DECODER_STATE_BIT3_1, 		DCC_DECODER_STATE_BIT4_WAIT,		58  },
			{ DCC_DECODER_STATE_BIT4_WAIT, 		DCC_DECODER_STATE_BIT4_1, 		58  },
			{ DCC_DECODER_STATE_BIT4_1, 		DCC_DECODER_STATE_BIT5_WAIT,		58  },
			{ DCC_DECODER_STATE_BIT5_WAIT, 		DCC_DECODER_STATE_BIT5_1, 		58  },
			{ DCC_DECODER_STATE_BIT5_1, 		DCC_DECODER_STATE_BIT6_WAIT,		58  },
			{ DCC_DECODER_STATE_BIT6_WAIT, 		DCC_DECODER_STATE_BIT6_1, 		58  },
			{ DCC_DECODER_STATE_BIT6_1, 		DCC_DECODER_STATE_BIT7_WAIT,		58  },
			{ DCC_DECODER_STATE_BIT7_WAIT, 		DCC_DECODER_STATE_BIT7_1, 		58  },
			{ DCC_DECODER_STATE_BIT7_1, 		DCC_DECODER_STATE_START_WAIT,		58  },
			{ DCC_DECODER_STATE_START_WAIT, 	DCC_DECODER_STATE_START_H, 		100, { 0xff }, 1, false },
			{ DCC_DECODER_STATE_START_H, 		DCC_DECODER_STATE_BIT0_WAIT,		100, { 0xff }, 1, false },
			{ DCC_DECODER_STATE_BIT0_WAIT, 		DCC_DECODER_STATE_BIT0_0, 		100 },
			{ DCC_DECODER_STATE_BIT0_0, 		DCC_DECODER_STATE_BIT1_WAIT,		100 },
			{ DCC_DECODER_STATE_BIT1_WAIT, 		DCC_DECODER_STATE_BIT1_0, 		100 },
			{ DCC_DECODER_STATE_BIT1_0, 		DCC_DECODER_STATE_BIT2_WAIT,		100 },
			{ DCC_DECODER_STATE_BIT2_WAIT, 		DCC_DECODER_STATE_BIT2_0, 		100 },
			{ DCC_DECODER_STATE_BIT2_0, 		DCC_DECODER_STATE_BIT3_WAIT,		100 },
			{ DCC_DECODER_STATE_BIT3_WAIT, 		DCC_DECODER_STATE_BIT3_0, 		100 },
			{ DCC_DECODER_STATE_BIT3_0, 		DCC_DECODER_STATE_BIT4_WAIT,		100 },
			{ DCC_DECODER_STATE_BIT4_WAIT, 		DCC_DECODER_STATE_BIT4_0, 		100 },
			{ DCC_DECODER_STATE_BIT4_0, 		DCC_DECODER_STATE_BIT5_WAIT,		100 },
			{ DCC_DECODER_STATE_BIT5_WAIT, 		DCC_DECODER_STATE_BIT5_0, 		100 },
			{ DCC_DECODER_STATE_BIT5_0, 		DCC_DECODER_STATE_BIT6_WAIT,		100 },
			{ DCC_DECODER_STATE_BIT6_WAIT, 		DCC_DECODER_STATE_BIT6_0, 		100 },
			{ DCC_DECODER_STATE_BIT6_0, 		DCC_DECODER_STATE_BIT7_WAIT,		100 },
			{ DCC_DECODER_STATE_BIT7_WAIT, 		DCC_DECODER_STATE_BIT7_0, 		100 },
			{ DCC_DECODER_STATE_BIT7_0, 		DCC_DECODER_STATE_START_WAIT,		100 },
			{ DCC_DECODER_STATE_START_WAIT, 	DCC_DECODER_STATE_START_H, 		100, { 0xff, 0x00 }, 2, false },
			{ DCC_DECODER_STATE_START_H, 		DCC_DECODER_STATE_BIT0_WAIT,		100, { 0xff, 0x00 }, 2, false },
			{ DCC_DECODER_STATE_BIT0_WAIT, 		DCC_DECODER_STATE_BIT0_1, 		58  },
			{ DCC_DECODER_STATE_BIT0_1, 		DCC_DECODER_STATE_BIT1_WAIT,		58  },
			{ DCC_DECODER_STATE_BIT1_WAIT, 		DCC_DECODER_STATE_BIT1_1, 		58  },
			{ DCC_DECODER_STATE_BIT1_1, 		DCC_DECODER_STATE_BIT2_WAIT,		58  },
			{ DCC_DECODER_STATE_BIT2_WAIT, 		DCC_DECODER_STATE_BIT2_1, 		58  },
			{ DCC_DECODER_STATE_BIT2_1, 		DCC_DECODER_STATE_BIT3_WAIT,		58  },
			{ DCC_DECODER_STATE_BIT3_WAIT, 		DCC_DECODER_STATE_BIT3_1, 		58  },
			{ DCC_DECODER_STATE_BIT3_1, 		DCC_DECODER_STATE_BIT4_WAIT,		58  },
			{ DCC_DECODER_STATE_BIT4_WAIT, 		DCC_DECODER_STATE_BIT4_1, 		58  },
			{ DCC_DECODER_STATE_BIT4_1, 		DCC_DECODER_STATE_BIT5_WAIT,		58  },
			{ DCC_DECODER_STATE_BIT5_WAIT, 		DCC_DECODER_STATE_BIT5_1, 		58  },
			{ DCC_DECODER_STATE_BIT5_1, 		DCC_DECODER_STATE_BIT6_WAIT,		58  },
			{ DCC_DECODER_STATE_BIT6_WAIT, 		DCC_DECODER_STATE_BIT6_1, 		58  },
			{ DCC_DECODER_STATE_BIT6_1, 		DCC_DECODER_STATE_BIT7_WAIT,		58  },
			{ DCC_DECODER_STATE_BIT7_WAIT, 		DCC_DECODER_STATE_BIT7_1, 		58  },
			{ DCC_DECODER_STATE_BIT7_1, 		DCC_DECODER_STATE_START_WAIT,		58  },
			{ DCC_DECODER_STATE_START_WAIT, 	DCC_DECODER_STATE_END_H,		58, { 0xff, 0x00, 0xff }, 3, false, true },
			{ DCC_DECODER_STATE_END_H,		DCC_DECODER_STATE_PREAMBLE_WAIT,	58, { 0xff, 0x00, 0xff }, 3, true, true },
		}
	
	},
};



struct test_handle {
	char *name;
	uint8_t packet[DCC_PACKET_MAX];
	int8_t len;
	bool received;

	bool reset_called;

	bool accessory_basic_called;
	uint16_t accessory_basic_add;
	uint8_t accessory_basic_pair;
	uint8_t accessory_basic_output;
	bool accessory_basic_value;
	
	bool accessory_extended_called;
	uint16_t accessory_extended_add;
	uint8_t accessory_extended_aspect;
};

struct test_handle test_handles[] = {
	{ "not", 	{ 0xff, 0x00, 0xff }, 3, false },
	{ "idle",	{ 0xff, 0x00, 0xff }, 3, true },
	{ "reset",	{ 0x00, 0x00, 0x00 }, 3, true, .reset_called = true },
	{ "basic acc min",  { 0x81, 0xf0, 0x71 }, 3, true,
		.accessory_basic_called = true,
		.accessory_basic_add = 1,
		.accessory_basic_pair = 0,
		.accessory_basic_output = 0,
		.accessory_basic_value = 0,
	},
	{ "basic acc bcast 0", { 0xbf, 0x88, 0x37 }, 3, true,
		.accessory_basic_called = true,
		.accessory_basic_add = 511,
		.accessory_basic_pair = 0,
		.accessory_basic_output = 0,
		.accessory_basic_value = 1,
	},
	{ "basic acc max (bcast 3)", { 0xbf, 0x87, 0x38 }, 3, true,
		.accessory_basic_called = true,
		.accessory_basic_add = 511,
		.accessory_basic_pair = 3,
		.accessory_basic_output = 1,
		.accessory_basic_value = 0,
	},
	{ "basic acc 1 pair 3 pin 1",  { 0x81, 0xff, 0x7e }, 3, true,
		.accessory_basic_called = true,
		.accessory_basic_add = 1,
		.accessory_basic_pair = 3,
		.accessory_basic_output = 1,
		.accessory_basic_value = 1,
	},
	{ "basic acc 2 pair 1 pin 0",  { 0x82, 0xfa, 0x78 }, 3, true,
		.accessory_basic_called = true,
		.accessory_basic_add = 2,
		.accessory_basic_pair = 1,
		.accessory_basic_output = 0,
		.accessory_basic_value = 1,
	},
	{ "basic acc cv",  { 0x81, 0xf0, 0xec, 0x00, 0x00, 0x9d }, 6, true,
		.accessory_basic_called = false,
	},
	{ "extended acc 1",  { 0x81, 0x71, 0x00, 0xf0 }, 4, true,
		.accessory_extended_called = true,
		.accessory_extended_add = 1,
		.accessory_extended_aspect = 0,
	},
	{ "extended acc 1",  { 0xbf, 0x07, 0xff, 0x47 }, 4, true,
		.accessory_extended_called = true,
		.accessory_extended_add = 2044,
		.accessory_extended_aspect = 255,
	},
};

bool dcc_handle_reset_called;
void dcc_handle_reset(void)
{
	dcc_handle_reset_called = true;
}

bool dcc_handle_accessory_basic_called;
uint16_t dcc_handle_accessory_basic_add;
uint8_t dcc_handle_accessory_basic_pair;
uint8_t dcc_handle_accessory_basic_output;
bool dcc_handle_accessory_basic_value;
void dcc_handle_accessory_basic(uint16_t add, uint8_t pair, uint8_t output, bool value)
{
	dcc_handle_accessory_basic_called = true;
	dcc_handle_accessory_basic_add = add;
	dcc_handle_accessory_basic_pair = pair;
	dcc_handle_accessory_basic_output = output;
	dcc_handle_accessory_basic_value = value;
}

bool dcc_handle_accessory_extended_called;
uint16_t dcc_handle_accessory_extended_output_address;
uint8_t dcc_handle_accessory_extended_aspect;
void dcc_handle_accessory_extended(uint16_t output_address, uint8_t aspect)
{
	dcc_handle_accessory_extended_called = true;
	dcc_handle_accessory_extended_output_address = output_address;
	dcc_handle_accessory_extended_aspect = aspect;
}

int main(int argc, char **argv)
{
	int i;
	int r = 0;
	uint16_t ticks = 0;
	
	for (i = 0; i < sizeof(tests)/sizeof(tests[0]); i ++) {
		int r_test = 0;
		TICK_US = tests[i].tick_us;
		printf("Test: %s: (TICK_US=%d) ", tests[i].name, TICK_US);
		
		dcc_packet_len = 0;
		dcc_packet_received = 0;
		
		int step;
		for (step = 0; step < tests[i].nr; step++) {
			dcc_decoder_state = tests[i].steps[step].state_in;
			dcc_packet_received = tests[i].steps[step].received_in;
			
			ticks += tests[i].steps[step].tick;

			/******************
			    Perform test
			 *****************/
		
			dcc_toggle(ticks);

			/******************
			    Check
			 *****************/
			
			if (dcc_decoder_state != tests[i].steps[step].state_out) {
				r_test++;
				printf("Step %d: expected %d -> %d, got %d\n", step, tests[i].steps[step].state_in, tests[i].steps[step].state_out, dcc_decoder_state);
			}
			
			if (tests[i].steps[step].len) {
				if (tests[i].steps[step].len != dcc_packet_len) {
					r_test++;
					printf("Step %d: len expected %d, got %d\n", step, tests[i].steps[step].len, dcc_packet_len);
				}
				if (tests[i].steps[step].received_out != dcc_packet_received) {
					r_test++;
					printf("Step %d: received expected %d, got %d\n", step, tests[i].steps[step].received_out, dcc_packet_received);
				}
				int j;
				for (j = 0; j < tests[i].steps[step].len; j++) {
					if (tests[i].steps[step].packet[j] != dcc_packet[j]) {
						r_test++;
						printf("Step %d: pos %d expected 0x%02x, got 0x%02x\n", step, j, tests[i].steps[step].packet[j], dcc_packet[j]);
					}
				}
			}
		}
		
		if (r_test == 0) {
			printf("PASS\n");
		} else {
			printf("FAIL\n");
		}
		r += r_test;
	}

	for (i = 0; i < sizeof(test_handles)/sizeof(test_handles[0]); i++) {
		int r_test = 0;
		printf("Test: %s: ", test_handles[i].name);

		memcpy((void*)dcc_packet, test_handles[i].packet, DCC_PACKET_MAX);
		dcc_packet_len = test_handles[i].len;
		dcc_packet_received = test_handles[i].received;
		
		dcc_handle_reset_called = false;
		dcc_handle_accessory_basic_called = false;
		dcc_handle_accessory_extended_called = false;
		
		/******************
		    Perform test
		 *****************/
		
		dcc_decoder_handle();
		
		/******************
		    Check
		 *****************/
		
		if (dcc_packet_received) {
			r_test++;
			printf("received state not reset as expected\n");
		}

		if (test_handles[i].reset_called != dcc_handle_reset_called) {
			r_test++;
			printf("reset handler not/unexpected called\n");
		}

		if (test_handles[i].accessory_basic_called != dcc_handle_accessory_basic_called) {
			r_test++;
			printf("basic accessory handler not/unexpected called\n");
		}
		if (test_handles[i].accessory_basic_called) {
			if (dcc_handle_accessory_basic_add != test_handles[i].accessory_basic_add)
				r_test++;
			if (dcc_handle_accessory_basic_pair != test_handles[i].accessory_basic_pair)
				r_test++;
			if (dcc_handle_accessory_basic_output != test_handles[i].accessory_basic_output)
				r_test++;
			if (dcc_handle_accessory_basic_value != test_handles[i].accessory_basic_value)
				r_test++;
			
			if (r_test) {
				printf("basic accessory handler not correctly called: (%d, %d, %d, %d)\n",
				    dcc_handle_accessory_basic_add,
				    dcc_handle_accessory_basic_pair,
				    dcc_handle_accessory_basic_output,
				    dcc_handle_accessory_basic_value);
			}
		}

		if (test_handles[i].accessory_extended_called != dcc_handle_accessory_extended_called) {
			r_test++;
			printf("extended accessory handler not/unexpected called\n");
		}
		if (test_handles[i].accessory_extended_called) {
			if (dcc_handle_accessory_extended_output_address != test_handles[i].accessory_extended_add)
				r_test++;
			if (dcc_handle_accessory_extended_aspect != test_handles[i].accessory_extended_aspect)
				r_test++;
			if (r_test) {
				printf("extended accessory handler not correctly called: (%d, %d)\n",
				    dcc_handle_accessory_extended_output_address,
				    dcc_handle_accessory_extended_aspect);
			}
		}
		
		
		if (r_test == 0) {
			printf("PASS\n");
		} else {
			printf("FAIL\n");
		}
		r += r_test;
	}

	if (r) {
		printf("Tests failed: %d\n", r);
	} else {
		printf("All tests pass\n");
	}

	return r;
}