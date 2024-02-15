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
static uint16_t tick_acc;
static uint16_t tick_milli;

void tick_handle(void)
{
	tick_t now = tick();
	tick_acc += now - tick_prev;
	tick_prev = now;
	
	while (tick_acc >= TICK_MILLIHZ) {
		tick_milli++;
		tick_acc -= TICK_MILLIHZ;
	}
}

uint16_t tick_ms(void)
{
	return tick_milli;
}
