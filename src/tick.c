/*
	tick support code for timekeeping
	
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

#include "tick.h"


static tick_t tick_prev;
tick_ms_t tick_milli;

void tick_handle(void)
{
#if defined (__AVR_ATtiny25__) || defined (__AVR_ATtiny45__) || defined (__AVR_ATtiny85__)
	if (TIFR & _BV(TOV1)) {
		tick_milli++;
		TIFR = _BV(TOV1);

		tick_prev += 256 - TICK_MILLIHZ;
		if (tick_prev >= TICK_MILLIHZ) {
			tick_milli++;
			tick_prev -= TICK_MILLIHZ;
		}
	}
#else
	tick_t now = tick();

	tick_t diff = now - tick_prev;
	
	if (diff >= TICK_MILLIHZ) {
		tick_milli++;
		tick_prev += TICK_MILLIHZ;
	}
#endif
}

