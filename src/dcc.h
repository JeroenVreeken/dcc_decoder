/*
	dcc definitions
	
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
#ifndef _INCLUDE_DCC_H_
#define _INCLUDE_DCC_H_



#define DCC_ADDRESS_BASE_MASK           0xc0
#define DCC_ADDRESS_BASE_LONG           0xc0
#define DCC_ADDRESS_BASE_ACCESSORY	0x80
#define DCC_ADDRESS_BROADCAST		0x00
#define DCC_ADDRESS_IDLE		0xff

#define DCC_RESET_DATA		0x00

#define DCC_ACCESSORY_SIGN		0x80
#define DCC_ACCESSORY_ADDH_MASK		0x70
#define DCC_ACCESSORY_ADDH_SHIFT		4
#define DCC_ACCESSORY_PAIR_MASK		0x06
#define DCC_ACCESSORY_PAIR_SHIFT		1
#define DCC_ACCESSORY_OUTPUT_MASK	0x01
#define DCC_ACCESSORY_ACTIVATE		0x08
#define DCC_ACCESSORY_EXTENDED_MASK	0x89
#define DCC_ACCESSORY_EXTENDED_ASPECT	0x01

#define DCC_ACCESSORY_BROADCAST_ADDRESS	0x1ff



#endif // _INCLUDE_DCC_H_
