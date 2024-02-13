/*
	dcc decoder CV
	
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

#include "cv.h"
#include "dcc.h"
#include <avr/eeprom.h>




static inline uint8_t *cv_ptr(uint16_t cv)
{
	uint8_t *ptr = NULL;
	
	return &ptr[cv];
}

void cv_write_byte(uint16_t cv, uint8_t value)
{
	if (cv > E2END)
		return;

	uint8_t *ptr = cv_ptr(cv);
	
	eeprom_update_byte(ptr, value);
}

uint8_t cv_read_byte(uint16_t cv)
{
	if (cv > E2END)
		return 0;

	uint8_t *ptr = cv_ptr(cv);
	
	return eeprom_read_byte(ptr);
}

// Variables to guard CV writes 
// A write needs to be send twice without other packets targeted at us
static bool address_match = false;
static uint16_t write_cv;
static uint8_t write_value;
static uint8_t write_bit;

bool cv_address_match(uint8_t address_h, uint8_t address_l)
{
	bool r = false;
	
	if (cv_read_byte(CV_CONFIGURATION) & CV_CONFIGURATION_EXTENDED_ADDRESS) {
		if (address_h == cv_read_byte(CV_ADDRESS_LONG_H) && address_l == cv_read_byte(CV_ADDRESS_LONG_L))
			r = true;
	} else {
		if (address_l == cv_read_byte(CV_ADDRESS_SHORT) && address_h == 0)
			r = true;
	}
	
	if ((address_h == DCC_ADDRESS_H_BROADCAST) && (address_l == DCC_ADDRESS_BROADCAST)) {
		r = true;
	}

	address_match |= r;

	return r;
}


void dcc_handle_multifunction_cv_write(uint8_t address_h, uint8_t address_l, uint16_t cv, uint8_t value)
{
	bool match = address_match;

	if (!cv_address_match(address_h, address_l))
		return;
	
	if (!match && write_cv == cv && write_value == value) {
		cv_write_byte(cv, value);
	} else {
		write_cv = cv;
		write_value = value;
		address_match = false;
	}		
}

void dcc_handle_multifunction_cv_writebit(uint8_t address_h, uint8_t address_l, uint16_t cv, uint8_t bit, bool value)
{
	bool match = address_match;

	if (!cv_address_match(address_h, address_l))
		return;
	
	if (!match && write_cv == cv && write_bit == bit && write_value == value) {
		uint8_t byte = cv_read_byte(cv);
	
		byte &= ~(1 << bit);
		byte |= (value << bit); 
	
		cv_write_byte(cv, value);
	} else {
		write_cv = cv;
		write_bit = bit;
		write_value = value;
		address_match = false;
	}
}

