DCC decoder implementation. It needs an edge triggered interrupt and a timer
capable of measuring microsecond-ish resolution (e.g. 4us for atmels)

'function_decoder' is a simple function decoder with 3 outputs.
Works with attiny85 (probably also 45/25 but untested

'accessory_servo' is a accessory decoder for 4 servo based switches.
It works with the atmega328 (e.g. arduino nano)

'accessory_signal' is a accessory decoder for 5 signals using.DCCext aspects
It works with the atmega328 (e.g. arduino nano)


Requires avr-gcc

Build:

  make

Unit tests:

  make test


