/*
	function decoder
	
	accessory implementation

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

#include <avr/wdt.h>

#include "tick.h"
#include "pwm.h"
#include "dcc_decoder.h"
#include "cv.h"



bool state_forward;
bool state_fl;
bool state_f1;
bool state_f2;
bool state_f3;
bool state_f4;



void dcc_handle_reset(uint8_t address_h, uint8_t address_l)
{
	if (!cv_address_match(address_h, address_l))
		return;

	state_fl = false;
	state_f1 = false;
	state_f2 = false;
	state_f3 = false;
	state_f4 = false;
}


void dcc_handle_multifunction_speed(uint8_t address_h, uint8_t address_l, uint8_t speed, bool forward)
{
	if (!cv_address_match(address_h, address_l))
		return;

	state_forward = forward;
}

void dcc_handle_multifunction_speedstep(uint8_t address_h, uint8_t address_l, uint8_t speed, bool forward)
{
	if (!cv_address_match(address_h, address_l))
		return;

	state_forward = forward;
}

void dcc_handle_multifunction_fg1(uint8_t address_h, uint8_t address_l, bool fl, bool f1, bool f2, bool f3, bool f4)
{
	if (!cv_address_match(address_h, address_l))
		return;

	state_fl = fl;
	state_f1 = f1;
	state_f2 = f2;
	state_f3 = f3;
	state_f4 = f4;
}






static void outputs(void)
{
	bool f0r = state_fl && !state_forward;
	bool f0f = state_fl && state_forward;
	bool f1 = state_f1;
	bool f2 = state_f2;
	uint8_t state = (f0f << 0) | (f0r << 1) | (f1 << 2) | (f2 << 3);

	uint8_t cv_f0f = cv_read_byte(CV_OUTPUT_MAP_F0F);
	uint8_t cv_f0r = cv_read_byte(CV_OUTPUT_MAP_F0R);
	uint8_t cv_f1 = cv_read_byte(CV_OUTPUT_MAP_F1);
	
	bool output_f0f = (state & cv_f0f) != 0;
	bool output_f0r = (state & cv_f0r) != 0;
	bool output_f1 = (state & cv_f1) != 0;

	
	if (output_f0f) {
		uint8_t pwm = cv_read_byte(CV_OUTPUT_PWM_F0F);
		pwm_set(0, pwm);
	} else {
		pwm_set(0, 0);
	}
	
	if (output_f0r) {
		uint8_t pwm = cv_read_byte(CV_OUTPUT_PWM_F0R);
		pwm_set(1, pwm);
	} else {
		pwm_set(1, 0);
	}

	if (output_f1) {
		uint8_t pwm = cv_read_byte(CV_OUTPUT_PWM_F1);
		pwm_set(3, pwm);
	} else {
		pwm_set(3, 0);
	}
}


int main(void)
{
	wdt_reset();
	
	tick_init();
	dcc_decoder_init();
	pwm_init();
	
	sei();
	
	while (1) {
		wdt_reset();
		dcc_decoder_handle();

		outputs();
	}
	
	return 0;
}
