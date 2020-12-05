#include "../include/attiny_spi.h"
#include <avr/io.h>
#include <util/atomic.h>

#define SPI_DDR_PORT DDRB
#define USCK_DD_PIN DDB2
#define DO_DD_PIN DDB1
#define DI_DD_PIN DDB0

void attiny_spi_begin()
{
    USICR &= ~(_BV(USISIE) | _BV(USIOIE) | _BV(USIWM1));
    USICR |= _BV(USIWM0) | _BV(USICS1) | _BV(USICLK);
    SPI_DDR_PORT |= _BV(USCK_DD_PIN);   // set the USCK pin as output
    SPI_DDR_PORT |= _BV(DO_DD_PIN);     // set the DO pin as output
    SPI_DDR_PORT &= ~_BV(DI_DD_PIN);    // set the DI pin as input
}

void attiny_spi_setDataMode(uint8_t spiDataMode)
{
    if (spiDataMode == ATTINY_SPI_MODE1)
        USICR |= _BV(USICS0);
    else
        USICR &= ~_BV(USICS0);
}

uint8_t attiny_spi_transfer(uint8_t spiData)
{
    USIDR = spiData;
    USISR = _BV(USIOIF);                // clear counter and counter overflow interrupt flag
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE)   // ensure a consistent clock period
    {
        while ( !(USISR & _BV(USIOIF)) ) USICR |= _BV(USITC);
    }
    return USIDR;
}

void attiny_spi_end()
{
    USICR &= ~(_BV(USIWM1) | _BV(USIWM0));
}
