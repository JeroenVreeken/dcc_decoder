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

uint8_t TCCR1;
uint8_t TCNT1;
uint8_t TIFR;


#define F_CPU 16000000
#define __AVR_ATtiny85__
/************************************/
#include "tick.c"
/************************************/




int test_tick(void)
{
	int r = 0;
	
	struct {
		uint8_t cnt;
		tick_t tick;
	} vectors[] = {
		{ 0x00, 0x00},
		{ 0xff, 0xff},
		{ 0x80, 0x80},
		{ 0x7f, 0x7f},
		{ 0x01, 0x01},
	};
	
	int i;
	for (i = 0; i < sizeof(vectors)/sizeof(vectors[0]); i++) {
		TCNT1 = vectors[i].cnt;
		tick_t out = tick();
		printf("Test tick TCNT1=0x%04x tick=0x%04x: result=0x%04x ", vectors[i].cnt, vectors[i].tick, out);

		if (vectors[i].tick != out) {
			printf("FAIL\n");
			r++;
		} else {
			printf("PASS\n");
		}
	}
	
	return r;
}


int test_tick_milli(void)
{
	int r = 0;
	
	struct {
		uint8_t   cnt;
		uint8_t   tifr;
		tick_t    tick_prev;
		tick_ms_t tick_milli;
		tick_t    tick_prev_out;
		tick_ms_t tick_ms;
	} vectors[] = {
		//   cnt       tifr    prev   milli prevout tick_ms
		{      0,         0,      0,      0,      0,      0 },
		{      1,         0,      0,      0,      0,      0 },
		{    250,         0,      0,      0,      0,      0 },
		{    255,         0,      0,      0,      0,      0 },
		{      0, _BV(TOV1),      0,      0,      6,      1 },
		{    243, _BV(TOV1),      0,      0,      6,      1 },
		{    244, _BV(TOV1),      6,      0,     12,      1 },
		{    245, _BV(TOV1),      6,      0,     12,      1 },
		{    245,         0,     12,      2,     12,      2 },
		{    255,         0,     12,      2,     12,      2 },
		{      0, _BV(TOV1),     12,      2,     18,      3 },
		{    255, _BV(TOV1),     12,      2,     18,      3 },
		{      0, _BV(TOV1),    246,     41,      2,     43 },
	};
	
	int i;
	for (i = 0; i < sizeof(vectors)/sizeof(vectors[0]); i++) {
		TCNT1 = vectors[i].cnt;
		TIFR = vectors[i].tifr;
		tick_prev = vectors[i].tick_prev;
		tick_milli = vectors[i].tick_milli;

		tick_handle();
		
		tick_ms_t tick_ms_out = tick_ms();
		
		printf("Test ms cnt=%5u tick_prev=%5u: tick_ms_in=%5u tick_prev_out=%5u tick_ms_out=%5u : tick_prev_out=%5u tick_ms_out=%5u ", 
		    vectors[i].cnt, vectors[i].tick_prev, vectors[i].tick_milli, vectors[i].tick_prev_out, vectors[i].tick_ms, tick_prev, tick_ms_out);

		if (vectors[i].tick_ms != tick_ms_out || vectors[i].tick_prev_out != tick_prev) {
			printf("FAIL\n");
			r++;
		} else {
			printf("PASS\n");
		}
	}

	return r;
}


int main(int argc, char **argv)
{
	int r = 0;

	printf("ut_tick:\n");

	r |= test_tick();

	r |= test_tick_milli();

	if (r) {
		printf("Tests failed: %d\n", r);
	} else {
		printf("All tests pass\n");
	}

	return r;
}
