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
static uint8_t address_match = 0;
static uint16_t write_cv;
static uint8_t write_value;


static void cv_handle_write(uint16_t cv, uint8_t value, uint8_t nr_match)
{
	if (address_match && write_cv == cv && write_value == value) {
		address_match++;
		if (address_match == nr_match) {
			cv_write_byte(cv, value);
			address_match = 0;
		}
	} else {
		write_cv = cv;
		write_value = value;
		address_match = 1;
	}
}

static void cv_handle_writebit(uint16_t cv, uint8_t data,  uint8_t nr_match)
{
	uint8_t bit = data & DCC_CV_BIT_MANIPULATE_BIT_MASK;
	bool value = data & DCC_CV_BIT_MANIPULATE_VAL_MASK;
	uint8_t bittype = data & DCC_CV_BIT_MANIPULATE_MASK;
	
	switch (bittype) {
		case DCC_CV_BIT_MANIPULATE_VERIFY:
			break;
		case DCC_CV_BIT_MANIPULATE_WRITE: {
			uint8_t byte = cv_read_byte(cv);
			byte &= ~(1 << bit);
			byte |= (value << bit); 
	
			cv_handle_write(cv, byte, nr_match);
			break;
		}
	}
}


#if defined(CONFIG_DCC_MULTIFUNCTION)

bool cv_address_match(uint8_t address_h, uint8_t address_l)
{
	bool r = false;
	uint8_t cv_conf = cv_read_byte(CV_CONFIGURATION);
	bool is_accessory = cv_conf & CV_CONFIGURATION_ACCESSORY;
	bool is_extended = cv_conf & CV_CONFIGURATION_EXTENDED_ADDRESS;
	
	if (!is_accessory) {
		if (is_extended) {
			if (address_h == cv_read_byte(CV_ADDRESS_LONG_H) && address_l == cv_read_byte(CV_ADDRESS_LONG_L))
				r = true;
		} else {
			if (address_l == cv_read_byte(CV_ADDRESS_SHORT) && address_h == 0)
				r = true;
		}
	}

	if ((address_h == DCC_ADDRESS_H_BROADCAST) && (address_l == DCC_ADDRESS_BROADCAST)) {
		r = true;
	}

	return r;
}



void dcc_handle_multifunction_cv_write(uint8_t address_h, uint8_t address_l, uint8_t type, uint16_t cv, uint8_t value)
{
	if (!cv_address_match(address_h, address_l))
		return;
	
	switch (type) {
		case DCC_CV_VERIFY_BYTE:
			break;
		case DCC_CV_WRITE_BYTE:
			cv_handle_write(cv, value, 2);
			break;
		case DCC_CV_BIT_MANIPULATE: {
			cv_handle_writebit(cv, value, 2);
			break;
		}
	}
}

#endif


#if defined(CONFIG_DCC_ACCESSORY)

bool cv_address_accessory_extended_match(uint16_t output_address)
{
	bool r = false;
	uint8_t cv_conf = cv_read_byte(CV_CONFIGURATION);
	bool is_accessory = cv_conf & CV_CONFIGURATION_ACCESSORY;
	bool is_extended = cv_conf & CV_CONFIGURATION_EXTENDED_ADDRESS;
	
	if (is_accessory && is_extended) {
		uint16_t cv_address = cv_read_byte(CV_ADDRESS_ACC_EXT_L) | (cv_read_byte(CV_ADDRESS_ACC_EXT_H) << 8);
		
		r = (output_address & 2047) == cv_address;
	}
	
	return r;	
}

void cv_address_accessory_extended_set(uint16_t output_address)
{
	uint8_t cv29 = cv_read_byte(CV_CONFIGURATION);
	
	cv29 |= CV_CONFIGURATION_EXTENDED_ADDRESS | CV_CONFIGURATION_ACCESSORY;
	
	cv_write_byte(CV_CONFIGURATION, cv29);
	cv_write_byte(CV_ADDRESS_ACC_EXT_L, output_address & 0xff);
	cv_write_byte(CV_ADDRESS_ACC_EXT_H, (output_address >> 8) & 0x07);
}

uint16_t cv_address_accessory_extended_get(void)
{
	uint16_t cv_address = cv_read_byte(CV_ADDRESS_ACC_EXT_L) | (cv_read_byte(CV_ADDRESS_ACC_EXT_H) << 8);
	if (cv_address == 0)
		cv_address = 2048;
	
	return cv_address;
}

void dcc_handle_accessory_extended_cv_write(uint16_t output_address, uint8_t type, uint16_t cv, uint8_t value)
{
	if (!cv_address_accessory_extended_match(output_address))
		return;

	switch (type) {
		case DCC_CV_VERIFY_BYTE:
			break;
		case DCC_CV_WRITE_BYTE:
			cv_handle_write(cv, value, 2);
			break;
		case DCC_CV_BIT_MANIPULATE: {
			cv_handle_writebit(cv, value, 2);
			break;
		}
	}
}


#endif

#if defined(CONFIG_DCC_SERVICE_MODE)

void dcc_handle_sm_cv_write(uint8_t type, uint16_t cv, uint8_t value)
{
	switch (type) {
		case DCC_CV_VERIFY_BYTE:
			break;
		case DCC_CV_WRITE_BYTE:
			cv_handle_write(cv, value, 5);
			break;
		case DCC_CV_BIT_MANIPULATE: {
			cv_handle_writebit(cv, value, 5);
			break;
		}
	}
}

#endif
