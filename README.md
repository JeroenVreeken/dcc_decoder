DCC decoder implementation. It needs an edge triggered interrupt and a timer
capable of measuring microsecond-ish resolution (e.g. 4us for atmels)

'function_decoder' is a simple function decoder with 3 outputs.
Works with attiny85 (probably also 45/25 but untested)

'accessory_servo' is a accessory decoder for 4 servo based switches.
Works with the atmega328 (e.g. arduino nano)

'accessory_signal_2led' is a accessory decoder.
It controls up to 6 signals with 2 leds each, using.DCCext aspects
Works with the atmega328 (e.g. arduino nano)

'accessory_signal_3led' is a accessory decoder.
It controls up to 4 signals with 3 leds each, using.DCCext aspects
Works with the atmega328 (e.g. arduino nano)

'accessory_mdd' is a accessory decoder.
An alternative firmware for the MDD 4 switch decoder

Requires avr-gcc

Build:

  make

Unit tests:

  make test


