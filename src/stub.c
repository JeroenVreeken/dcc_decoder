/*
	test stubs
	
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
#include "dcc_decoder.h"

void dcc_handle_stub(void) {}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wpragmas" // in case gcc does not recognize -Wattribute-alias
#pragma GCC diagnostic ignored "-Wattribute-alias"

void dcc_handle_reset(void) __attribute__ ((weak, alias ("dcc_handle_stub")));
void dcc_handle_accessory_basic(uint16_t add, uint8_t pair, uint8_t output, bool value) __attribute__ ((weak, alias ("dcc_handle_stub")));
void dcc_handle_accessory_extended(uint16_t output_address, uint8_t aspect) __attribute__ ((weak, alias ("dcc_handle_stub")));

#pragma GCC diagnostic pop
