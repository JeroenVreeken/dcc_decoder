#include "cv.h"


.section .eeprom

.org CV_ADDRESS_SHORT
eeprom_cv1: .byte 	3

.org CV_ADDRESS_LONG_H
eeprom_cv17: .byte	192
eeprom_cv18: .byte	3

.org CV_CONFIGURATION
eeprom_cv29: .byte	CV_CONFIGURATION_28_128_SPEEDSTEPS

.org CV_OUTPUT_MAP_F0F
eeprom_cv33: .byte	1
eeprom_cv34: .byte	2
eeprom_cv35: .byte	4

.org CV_OUTPUT_PWM_F0F
eeprom_cv49: .byte	128
eeprom_cv50: .byte	128
eeprom_cv51: .byte	128
