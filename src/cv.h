/*
	dcc decoder
	
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

#ifndef _INCLUDE_CV_H_
#define _INCLUDE_CV_H_



#define CV_ADDRESS_SHORT	1
#define CV_ADDRESS_LONG_H	17
#define CV_ADDRESS_LONG_L	18
#define CV_CONFIGURATION	29

#define CV_OUTPUT_MAP_F0F	33
#define CV_OUTPUT_MAP_F0R	34
#define CV_OUTPUT_MAP_F1	35

#define CV_OUTPUT_PWM_F0F	49
#define CV_OUTPUT_PWM_F0R	50
#define CV_OUTPUT_PWM_F1	51

#define CV_CONFIGURATION_REVERSE		0x01
#define CV_CONFIGURATION_28_128_SPEEDSTEPS	0x02
#define CV_CONFIGURATION_EXTENDED_ADDRESS	0x20



#ifndef	__ASSEMBLER__

#include <stdint.h>
#include <stdbool.h>


void cv_write_byte(uint16_t cv, uint8_t value);
uint8_t cv_read_byte(uint16_t cv);

bool cv_address_match(uint8_t address_h, uint8_t address_l);


#endif // __ASSEMBLER__
#endif // _INCLUDE_CV_H_
