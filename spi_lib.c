#include "spi_lib.h"

#include <avr/io.h>

void spiInit(void)
{
    // init proper data direction of pins
    DDR_SPI |= (1 << SS) |(1 << MOSI) | (1 << SCK);
    //PORT_SPI |= (1 << MISO);     
    // Enable SPI, Master, set clock rate fck/16
    SPCR = (1 << SPE) | (1 << MSTR) | (1 << SPR0);
}

uchar spiSendByte(uchar byte)
{
    // Start transmission
    SPDR = byte;
    // Wait for transmission complete
    while(!(SPSR & (1 << SPIF)));
    
    return SPDR;
}
