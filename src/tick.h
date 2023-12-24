/*
	tick
	
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
#ifndef _INCLUDE_TICK_H_
#define _INCLUDE_TICK_H_


#include <avr/interrupt.h>
#include <avr/io.h>


#define TICK_PRESCALER 64

#define TICK_HZ (F_CPU / TICK_PRESCALER)
#ifndef TICK_US
#define TICK_US (1000000u / TICK_HZ)
#endif

#define T1_PRESCALER_1		(                        (1 << CS10))
#define T1_PRESCALER_8		(            (1 << CS11)            )
#define T1_PRESCALER_64		(            (1 << CS11)|(1 << CS10))
#define T1_PRESCALER_256	((1 << CS12)                        )
#define T1_PRESCALER_1024	((1 << CS12)|            (1 << CS10))



static inline void tick_init(void)
{
	// Configure T1

	TCCR1A = 0;// normal mode, no output pins.
	
#if (TICK_PRESCALER==64)
	TCCR1B = T1_PRESCALER_64;
#else
#error no prescaler
#endif
	
	TCCR1C = 0;
}

static inline uint16_t tick(void)
{
	return TCNT1;
}

#endif // _INCLUDE_TICK_H_
