#ifndef	_ATTINY_SPI_H_
#define	_ATTINY_SPI_H_

#include "attiny_io.h"

// SPI data modes
#define ATTINY_SPI_MODE0 0x00
#define ATTINY_SPI_MODE1 0x04

void attiny_spi_begin();
void attiny_spi_setDataMode(uint8_t spiDataMode);
uint8_t attiny_spi_transfer(uint8_t spiData);
void attiny_spi_end();

#endif	/* !_ATTINY_SPI_H_ */
