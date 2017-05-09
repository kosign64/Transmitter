#ifndef RADIO_LIB_H
#define RADIO_LIB_H

#include "nRF24L01.h"
#include "spi_lib.h"

#ifdef __AVR_ATmega8__

#define DDR_NRF DDRD
#define PORT_NRF PORTD
//#define CE 4
#define CE 5
#define IRQ 3

#endif

#ifdef __AVR_ATmega128__

#define DDR_NRF DDRB
#define PORT_NRF PORTB
#define CE 6

#endif

#define W 1
#define R 0

#define CE_LOW() PORT_NRF &= ~(1 << (CE))
#define CE_HIGH() PORT_NRF |= (1 << (CE))

// Public interface
void initAsTrans(uchar *address, uchar addressSize, uchar payloadSize);
void initAsReceiver(uchar *address, uchar addressSize, uchar payloadSize);
void transmitPayload(uchar *buff, uchar payloadSize);
void setNewAdressAndSize(uchar *address, uchar addressSize);
void setNewAddress(uchar *address, uchar addressSize);
void startListengForPayload(void);
void stopListeningForPayload(void);
void resetIRQ(void);
uchar getReg(uchar reg);

// Private functions, do not use them
void portInit(void);
uchar* sendToNrf(uchar readWrite, uchar reg, uchar *array, uchar arraySize);

#endif
