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

void dcc_handle_reset(uint8_t h, uint8_t l) __attribute__ ((weak, alias ("dcc_handle_stub")));
void dcc_handle_accessory_basic(uint16_t add, uint8_t pair, uint8_t output, bool value) __attribute__ ((weak, alias ("dcc_handle_stub")));
void dcc_handle_accessory_extended(uint16_t output_address, uint8_t aspect) __attribute__ ((weak, alias ("dcc_handle_stub")));
void dcc_handle_multifunction_speed(uint8_t address_h, uint8_t address_l, uint8_t speed, bool forward) __attribute__ ((weak, alias ("dcc_handle_stub")));
void dcc_handle_multifunction_speedstep(uint8_t address_h, uint8_t address_l, uint8_t speed, bool forward) __attribute__ ((weak, alias ("dcc_handle_stub")));
void dcc_handle_multifunction_fg1(uint8_t address_h, uint8_t address_l, bool fl, bool f1, bool f2, bool f3, bool f4) __attribute__ ((weak, alias ("dcc_handle_stub")));
void dcc_handle_multifunction_cv_write(uint8_t address_h, uint8_t address_l, uint16_t cv, uint8_t value) __attribute__ ((weak, alias ("dcc_handle_stub")));
void dcc_handle_multifunction_cv_writebit(uint8_t address_h, uint8_t address_l, uint16_t cv, uint8_t bit, bool value) __attribute__ ((weak, alias ("dcc_handle_stub")));
void dcc_handle_accessory_extended_cv_write(uint16_t output_address, uint16_t cv, uint8_t value) __attribute__ ((weak, alias ("dcc_handle_stub")));
void dcc_handle_accessory_extended_cv_writebit(uint16_t output_address, uint16_t cv, uint8_t bit, bool value) __attribute__ ((weak, alias ("dcc_handle_stub")));

#pragma GCC diagnostic pop
