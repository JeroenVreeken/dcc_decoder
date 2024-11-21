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
#define DCC_ADDRESS_BASE_SHORT		0x00
#define DCC_ADDRESS_BASE_LONG           0xc0
#define DCC_ADDRESS_BASE_ACCESSORY	0x80
#define DCC_ADDRESS_BROADCAST		0x00
#define DCC_ADDRESS_IDLE		0xff

#define DCC_ADDRESS_H_BROADCAST		0x00

#define DCC_ADDRESS_SM_MASK		0xf0
#define DCC_ADDRESS_SM_DIRECT_MODE	0x70

#define DCC_SM_RESET_CNT_MIN		3

#define DCC_INSTRUCTION_MASK            0xe0
#define DCC_INSTRUCTION_CTRL		0x00
#define DCC_INSTRUCTION_ADVANCED	0x20
#define DCC_INSTRUCTION_SPEED_F 	0x60	// Speed forward
#define DCC_INSTRUCTION_SPEED_R 	0x40	// Speed reverse
#define DCC_INSTRUCTION_FG1		0x80	// Function group 1: F0(FL) F1, F2, F3, F4
#define DCC_INSTRUCTION_FG2		0xa0	// Function group 2: F9-F12 or Function group 2  F5-F8
#define DCC_INSTRUCTION_FEAT_EX		0xc0	// Feature expansion
#define DCC_FEAT_EX_F13_F20		0x1e	// Feature expansion F13-F20
#define DCC_FEAT_EX_F21_F28		0x1f	// Feature expansion F21-F28
#define DCC_INSTRUCTION_CV		0xe0	// Configuration Variable access
#define DCC_CV_SHORT_FORM		0x10
#define DCC_CV_VARIABLE_HIGH_MASK	0x03
#define DCC_CV_INSTRUCTION_TYPE_MASK	0x0c
#define DCC_CV_VERIFY_BYTE		0x04	// Configuration Variable access
#define DCC_CV_WRITE_BYTE		0x0c	// Configuration Variable access
#define DCC_CV_BIT_MANIPULATE		0x08	// Configuration Variable access
#define DCC_CV_BIT_MANIPULATE_BIT_MASK	0x07
#define DCC_CV_BIT_MANIPULATE_VAL_MASK	0x08
#define DCC_CV_BIT_MANIPULATE_MASK	0xf0
#define DCC_CV_BIT_MANIPULATE_VERIFY	0xe0
#define DCC_CV_BIT_MANIPULATE_WRITE	0xf0

#define DCC_SPEED_MASK			0x1f

#define DCC_ADVANCED_SUB_MASK		0x1f
#define DCC_ADVANCED_SPEEDSTEP		0x1f

#define DCC_SPEEDSTEP_SPEED_MASK	0x7f
#define DCC_SPEEDSTEP_DIRECTION_MASK	0x80

#define DCC_CTRL_MASK			0x0e
#define DCC_CTRL_RESET			0x00

#define DCC_FG1_FL			0x10
#define DCC_FG1_F1			0x01
#define DCC_FG1_F2			0x02
#define DCC_FG1_F3			0x04
#define DCC_FG1_F4			0x08

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

#define DCC_ACCESSORY_EXTENDED_CV_MASK	0xf0
#define DCC_ACCESSORY_EXTENDED_CV_OMP	0xe0

#define DCC_ASPECT_STOP			0
#define DCC_ASPECT_EXPECT_STOP		1
#define DCC_ASPECT_BRAKE_INDICATED	2
#define DCC_ASPECT_PASS_40KMH		4
#define DCC_ASPECT_PASS_INDICATED	6
#define DCC_ASPECT_PASS			16
#define DCC_ASPECT_PROCEED_ON_SIGHT	33
#define DCC_ASPECT_INITIATE_BRAKING	34


#endif // _INCLUDE_DCC_H_
