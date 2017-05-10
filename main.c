#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "spi_lib.h"
#include "radio_lib.h"

#define ADDR_SIZE 5
#define POINT_NUMBER 40

typedef struct
{
    unsigned angle : 7;
    signed   x     : 5;
    unsigned y     : 4;
}Point;

typedef enum
{
    ANGLE,
    POS
} State;

static const uchar payloadSize = POINT_NUMBER * sizeof(Point);
static uchar address[ADDR_SIZE] = {0x11, 0x12, 0x12, 0x12, 0x12};

Point points[POINT_NUMBER];

ISR(USART0_RX_vect)
{
    static State state = ANGLE;
    static uchar i = 0;
    uchar data = UDR0;
    switch (state) {
    case ANGLE:
        *((uchar*)(&points[i])) = data;
        state = POS;
        break;
    case POS:
        *((uchar*)(&points[i]) + 1) = data;
        state = ANGLE;
        if(++i >= POINT_NUMBER)
        {
            i = 0;
            points[i].angle |= _BV(7);
            transmitPayload((uchar*)points, payloadSize);
        }
        break;
    }
}

int main(void)
{
    spiInit();
    // UART 8-N-1-115200
    UCSR0B = _BV(RXCIE0) | _BV(RXEN0) | _BV(TXEN0);
    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
    UBRR0H = 0;
    UBRR0L = 5;
    initAsTrans(address, ADDR_SIZE, payloadSize);

    sei();

    while(1)
    {

    }
}
