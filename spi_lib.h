#ifndef SPI_LIB_H
#define SPI_LIB_H

#ifdef __AVR_ATmega128__

#define DDR_SPI DDRB
#define PORT_SPI PORTB
#define SS 0
#define MOSI 2
#define MISO 3
#define SCK 1

#endif

#ifdef __AVR_ATmega8__

#define DDR_SPI DDRB
#define PORT_SPI PORTB
//#define SS 2
#define ASS 2
#define SS 0
#define MOSI 3
#define MISO 4
#define SCK 5

#endif

#define CSN_LOW() PORT_SPI &= ~(1 << (SS))
#define CSN_HIGH() PORT_SPI |= (1 << (SS))

typedef unsigned char uchar;

void spiInit(void);
uchar spiSendByte(uchar byte);

#endif
