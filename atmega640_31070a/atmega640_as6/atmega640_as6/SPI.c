/* 
   AVR SPI functions
   Copyright mtech.dk, 2009-10
*/

#include "project.h"
#include <avr/io.h>

void SPIinit() 
// initializes SPI communication
{
  DDR_SPI |= (1<<DD_MOSI)|(1<<DD_SCK)|(1<<DD_SS); // MOSI, SCK, and SS are outputs, others are inputs

/* This is no longer required, only required in Rev.A prototype

	// also need to set SS as output (required due to silly PCB layout on first prototype)
#if defined(__AVR_ATmega8__)
	// is already set as CS, but we set it again to accomodate for other boards 
	DDR_SPI |= (1<<2);
#else
	DDR_SPI |= (1<<0);			             // assume it's bit0 (mega128, portB and others)
#endif
*/

  SPCR = (1<<SPE)|(1<<MSTR)|(1<<SPR0); // Enable SPI, Master, clockrate = 1 MHz
}

unsigned char SPItransmit(unsigned char data)
// Tranceives a single byte on the SPI channel (one byte out, one byte in)
{
  SPDR = data;                 // start transmission
  while (!(SPSR & (1<<SPIF))); // wait until transmission complete
  return SPDR;
}
