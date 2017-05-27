avrdude -p atmega128 -c stk500v2 -P /dev/ttyUSB0 -U lfuse:w:0xff:m -U hfuse:w:0xc9:m -U efuse:w:0xff:m
