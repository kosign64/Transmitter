#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include "spi_lib.h"
#include "radio_lib.h"

#define ADDR_SIZE 5
#define POINT_NUMBER 7

#define LED 7

typedef struct
{
    unsigned angle : 7;
    signed   x     : 5;
    unsigned y     : 4;
}Point;

typedef enum
{
    START,
    ANGLE,
    POS
} State;

static const uchar payloadSize = POINT_NUMBER * sizeof(Point);
static uchar address[ADDR_SIZE] = {0x11, 0x12, 0x12, 0x12, 0x12};

uchar points[POINT_NUMBER * 2];

ISR(USART0_RX_vect)
{
    static State state = START;
    static uchar i = 0;
    uchar data = UDR0;
    //PORTF ^= _BV(LED);
    switch (state) {
    case START:
        if(data == 0xFF)
        {
            state = ANGLE;
        }
        break;
    case ANGLE:
        points[i] = data;
        state = POS;
        break;
    case POS:
        points[i + 1] = data;
        state = ANGLE;
        i += 2;
        if(i >= POINT_NUMBER)
        {
            i = 0;
            PORTF ^= _BV(LED);
            state = START;
            transmitPayload((uchar*)points, payloadSize);
        }
        break;
    }
}

int main(void)
{
    DDRF = _BV(LED);
    // UART 8-N-1-115200
    UCSR0B = _BV(RXCIE0) | _BV(RXEN0) | _BV(TXEN0);
    UCSR0C = _BV(UCSZ01) | _BV(UCSZ00);
    UBRR0H = 0;
    UBRR0L = 5;
    _delay_ms(1000);
    initAsTrans(address, ADDR_SIZE, payloadSize);
    PORTF |= _BV(LED);

    sei();

    _delay_ms(3000);

    while(1)
    {

    }
}
