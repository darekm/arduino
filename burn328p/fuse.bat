avrdude  -c usbasp -p m328p -v -B10  -U efuse:w:0x05:m -U hfuse:w:0xDA:m -U lfuse:w:0xA2:m

