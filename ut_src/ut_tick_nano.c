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

uint8_t TCCR1A;
uint8_t TCCR1B;
uint8_t TCCR1C;
uint16_t TCNT1;

#define F_CPU 16000000
#define __AVR_ATmega328P__

/************************************/
#include "tick.c"
/************************************/




int test_tick(void)
{
	int r = 0;
	
	struct {
		uint16_t cnt;
		tick_t tick;
	} vectors[] = {
		{ 0x0000, 0x0000},
		{ 0xffff, 0xffff},
		{ 0x8000, 0x8000},
		{ 0x7fff, 0x7fff},
		{ 0x0001, 0x0001},
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
		uint16_t  cnt;
		tick_t    tick_prev;
		tick_ms_t tick_milli;
		tick_t    tick_prev_out;
		tick_ms_t tick_ms;
	} vectors[] = {
		{      0,      0,      0,      0,      0 },
		{      1,      0,      0,      0,      0 },
		{    250,      0,      0,    250,      1 },
		{    499,    250,      1,    250,      1 },
		{    500,    250,      1,    500,      2 },
		{    501,    250,      1,    500,      2 },
		{    501,    500,      2,    500,      2 },
		{   1500,      0,      0,    250,      1 },
		{   1500,    250,      1,    500,      2 },
		{   1500,    500,      2,    750,      3 },
		{   1500,    750,      3,   1000,      4 },
		{   1500,   1000,      4,   1250,      5 },
		{   1500,   1250,      5,   1500,      6 },
		{   1500,   1500,      6,   1500,      6 },
		{  65500,  65500,    260,  65500,    260 },
		{  65535,  65500,    260,  65500,    260 },
		{      0,  65500,    260,  65500,    260 },
		{      1,  65500,    260,  65500,    260 },
		{    213,  65500,    260,  65500,    260 },
		{    214,  65500,    260,    214,    261 },
		{    215,  65500,    260,    214,    261 },
		{    216,  65500,    260,    214,    261 },
		{    250,  65500,    260,    214,    261 },
	};
	
	int i;
	for (i = 0; i < sizeof(vectors)/sizeof(vectors[0]); i++) {
		TCNT1 = vectors[i].cnt;
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
