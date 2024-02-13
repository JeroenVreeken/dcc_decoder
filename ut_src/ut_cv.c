/*
	dcc decoder unittests
	
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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>


#define EEMEM
#define E2END 511

uint8_t ut_cv[E2END+1];

void eeprom_write_byte (uint8_t *p, uint8_t value)
{
	//printf("eeprom_write_byte(%zd, 0x%02x)\n", (size_t)p, value);
	p += (size_t)(ut_cv);
	*p = value;
}

void eeprom_update_byte (uint8_t *p, uint8_t value)
{
	//printf("eeprom_update_byte(%zd, 0x%02x)\n", (size_t)p, value);
	eeprom_write_byte(p, value);
}

uint8_t eeprom_read_byte (const uint8_t *p)
{
	uint8_t value = p[(size_t)ut_cv];
	
	//printf("eeprom_read_byte(%zd): 0x%02x\n", (size_t)p, value);
	return value;
}


/************************************/
#include "cv.c"
/************************************/




int test_address_match(void)
{
	int r = 0;
	
	struct {
		uint8_t address_h;
		uint8_t address_l;
		uint8_t cv29;
		uint8_t cv1;
		uint8_t cv17;
		uint8_t cv18;
		bool match;
	} vectors[] = {
		{ 0,	0,	0, 	3, 	192, 3, 	true	}, // broadcast
		{ 0,	0,	2, 	3, 	192, 3, 	true	}, // broadcast
		{ 0,	0,	32, 	3, 	192, 3, 	true	}, // broadcast
		{ 0,	0,	34, 	3, 	192, 3, 	true	}, // broadcast
		{ 0,	3,	2, 	3, 	192, 3, 	true	}, // short 3
		{ 192,	3,	32, 	3, 	192, 3, 	true	}, // long 3
		{ 192,	3,	34, 	3, 	192, 3, 	true	}, // long 3
		{ 0,	3,	2, 	3, 	192, 1, 	true	}, // short 3
		{ 192,	3,	32, 	1, 	192, 3, 	true	}, // long 3
		{ 192,	3,	34, 	1, 	192, 3, 	true	}, // long 3
		{ 0,	3,	32, 	3, 	192, 3, 	false	}, // short 3, long conf
		{ 0,	3,	34, 	3, 	192, 3, 	false	}, // short 3, long conf
		{ 192,	3,	2, 	3, 	192, 3, 	false	}, // long 3, short conf
		{ 0xdf,	0x64,	32, 	3, 	0xdf, 0x64, 	true	}, // long 8036
		{ 0xdf,	0x64,	34, 	3, 	0xdf, 0x64, 	true	}, // long 8036
	};
	
	int i;
	for (i = 0; i < sizeof(vectors)/sizeof(vectors[0]); i++) {
		bool match = false;
		
		ut_cv[1] = vectors[i].cv1;
		ut_cv[29] = vectors[i].cv29;
		ut_cv[17] = vectors[i].cv17;
		ut_cv[18] = vectors[i].cv18;

		printf("Test address 0x%02x 0x%02x (%d), CV29=0x%02x, CV1=0x%02x, CV17=0x%02x, CV18=0x%02x, match=%d: ",
		    vectors[i].address_h, vectors[i].address_l, (vectors[i].address_h & 0x3f) * 256 | vectors[i].address_l,
		    vectors[i].cv29, vectors[i].cv1, vectors[i].cv17, vectors[i].cv18, vectors[i].match);

		match = cv_address_match(vectors[i].address_h, vectors[i].address_l);
		if (match != vectors[i].match) {
			printf("match=%d ", match);
			r++;
		} else {
			printf("PASS\n");
		}
	}
	
	return r;
}


int test_write(void)
{
	int r = 0;

	ut_cv[1] = 3;
	ut_cv[17] = 192;
	ut_cv[18] = 3;
	ut_cv[29] = 2;
	
	printf("First write CV17 @broadcast: ");
	dcc_handle_multifunction_cv_write(0, 0, 17, 200);
	if (ut_cv[17] != 192) {
		printf("CV wrong: %d (0x%02x)\n", ut_cv[17], ut_cv[17]);
	} else {
		printf("PASS\n");
	}
	printf("Second write CV17 @broadcast: ");
	dcc_handle_multifunction_cv_write(0, 0, 17, 200);
	if (ut_cv[17] != 200) {
		printf("CV wrong: %d (0x%02x)\n", ut_cv[17], ut_cv[17]);
	} else {
		printf("PASS\n");
	}

	printf("First write CV17 @3 short: ");
	dcc_handle_multifunction_cv_write(0, 3, 17, 192);
	if (ut_cv[17] != 200) {
		printf("CV wrong: %d (0x%02x)\n", ut_cv[17], ut_cv[17]);
	} else {
		printf("PASS\n");
	}
	printf("Second write CV17 @3 short: ");
	dcc_handle_multifunction_cv_write(0, 3, 17, 192);
	if (ut_cv[17] != 192) {
		printf("CV wrong: %d (0x%02x)\n", ut_cv[17], ut_cv[17]);
	} else {
		printf("PASS\n");
	}
	
	ut_cv[1] = 3;
	ut_cv[29] = 34;
	ut_cv[17] = 192;
	ut_cv[18] = 3;

	printf("First write CV1 @3 long: ");
	dcc_handle_multifunction_cv_write(192, 3, 1, 99);
	if (ut_cv[1] != 3) {
		printf("CV wrong: %d (0x%02x)\n", ut_cv[1], ut_cv[1]);
	} else {
		printf("PASS\n");
	}
	printf("Second write CV1 @3 long: ");
	dcc_handle_multifunction_cv_write(192, 3, 1, 99);
	if (ut_cv[1] != 99) {
		printf("CV wrong: %d (0x%02x)\n", ut_cv[1], ut_cv[1]);
	} else {
		printf("PASS\n");
	}

	ut_cv[1] = 3;
	ut_cv[29] = 34;
	ut_cv[17] = 192;
	ut_cv[18] = 3;

	printf("First write CV1 @3 long: ");
	dcc_handle_multifunction_cv_write(192, 3, 1, 99);
	if (ut_cv[1] != 3) {
		printf("CV wrong: %d (0x%02x)\n", ut_cv[1], ut_cv[1]);
	} else {
		printf("PASS\n");
	}
	printf("Between write CV1 @3 short: ");
	dcc_handle_multifunction_cv_write(0, 3, 1, 99);
	if (ut_cv[1] != 3) {
		printf("CV wrong: %d (0x%02x)\n", ut_cv[1], ut_cv[1]);
	} else {
		printf("PASS\n");
	}
	printf("Between write CV1 @4 long: ");
	dcc_handle_multifunction_cv_write(192, 4, 1, 99);
	if (ut_cv[1] != 3) {
		printf("CV wrong: %d (0x%02x)\n", ut_cv[1], ut_cv[1]);
	} else {
		printf("PASS\n");
	}
	printf("Second write CV1 @3 long: ");
	dcc_handle_multifunction_cv_write(192, 3, 1, 99);
	if (ut_cv[1] != 99) {
		printf("CV wrong: %d (0x%02x)\n", ut_cv[1], ut_cv[1]);
	} else {
		printf("PASS\n");
	}


	return r;
}


int main(int argc, char **argv)
{
	int r = 0;

	printf("ut_cv:\n");

	r |= test_address_match();

	r |= test_write();

	if (r) {
		printf("Tests failed: %d\n", r);
	} else {
		printf("All tests pass\n");
	}

	return r;
}
