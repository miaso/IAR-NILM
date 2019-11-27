/* 
   CS-chip related functions
   (C) mtech.dk, 2009-10
*/

#include <avr/io.h>
#include "project.h"
#include "SPI.h"

void CSchipInit()
// Initializes CS chip
{
  CS_RESET_PORT |= (1<<CS_RESET); // /Reset high
  CS_RESET_DDR |= (1<<CS_RESET);
  CS_CS_PORT |= (1<<CS_CS);       // /CS high
}

void CScmd(uint8_t cmd)
// Transmits a CS command
{
  CS_CS_PORT &= ~(1<<CS_CS);      // /CS low
  SPItransmit(cmd);
  CS_CS_PORT |= (1<<CS_CS);       // /CS high
}

void CSwrite(uint8_t reg, uint8_t high, uint8_t mid, uint8_t low)
// Writes to a CS register
{
  CS_CS_PORT &= ~(1<<CS_CS);      // /CS low
  SPItransmit((reg << 1) | 0x40); // write cmd
  SPItransmit(high);
  SPItransmit(mid);
  SPItransmit(low);
  CS_CS_PORT |= (1<<CS_CS);       // /CS high
}

void CSwrite24(uint8_t reg, uint32_t value)
// Writes 24 bit value to CS register
{
  CSwrite(reg, value >> 16, value >> 8, value);
}

uint32_t CSread(uint8_t reg)
// Reads a CS register
{
  CS_CS_PORT &= ~(1<<CS_CS);      // /CS low
  asm volatile ("nop");
  asm volatile ("nop");
  asm volatile ("nop");
  SPItransmit(reg << 1);          // read register
  uint8_t a = SPItransmit(0xFF);  // start contionus conversions
  uint8_t b = SPItransmit(0xFF);  // start contionus conversions
  uint8_t c = SPItransmit(0xFF);  // start contionus conversions
  asm volatile ("nop");
  asm volatile ("nop");
  asm volatile ("nop");
  CS_CS_PORT |= (1<<CS_CS);       // /CS high
  return ((unsigned long)a << 16) | ((unsigned long)b << 8) | c;
}
