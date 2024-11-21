#include "cv.h"


.section .eeprom

.org CV_ADDRESS_ACC_EXT_L
eeprom_cv1: .byte       10
.org CV_ADDRESS_ACC_EXT_H
eeprom_cv9: .byte       0


.org CV_CONFIGURATION
eeprom_cv29: .byte	CV_CONFIGURATION_EXTENDED_ADDRESS | CV_CONFIGURATION_ACCESSORY


.org CV_MANUFACTURER
eeprom_cv112: .byte	32
eeprom_cv113: .byte	255
eeprom_cv114: .byte	32
eeprom_cv115: .byte	255
eeprom_cv116: .byte	23
eeprom_cv117: .byte	255
eeprom_cv118: .byte	32
eeprom_cv119: .byte	255
eeprom_cv120: .byte	32
eeprom_cv121: .byte	255
eeprom_cv122: .byte	32
eeprom_cv123: .byte	255
