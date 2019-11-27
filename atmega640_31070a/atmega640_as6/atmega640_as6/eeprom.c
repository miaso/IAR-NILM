/* 
   EEPROM functions for AVR640/164P series (among others)
   (C) mtech.dk, 2009-10

   These functions are used to read data from and write data to the
   AVR640/164P EEPROM.
   
   Notes:
   In order to prevent EEPROM corruption, a suitable BOD level should
   be set.
  
   Typical write time: 3.4 ms
   Reads are performed instantanously as long as no write operation
   is going on.
  
   4K EEPROM available on AVR640/164P.
  
   AVR640/164P EEPROM supports minimum 100.000 write/erase cycles.
   Note that EEPROM is organized into pages of 8 byte for 640 and
   4 bytes for 164P (lower 2-3 bits in EEAR points into the page).
  
   These functions are stalling until the previous write operation
   completes. They should therefore be used with extreme care
   during timing critical operations.
  
   As EEPROM read/write should only be necessary during power up and
   firmware upload this should not be a significant issue.
  
   SPMEN in SPMCSR is not checked in these routines. You therefore
   have to ensure that flash updating has finished before calling
   the EEPROM write routines (otherwise the EEPROM write will not
   initiate).
  
   Warning: Some AVR's zeros the EEAR register on reset. This means
   that there is a high risk of EEPROM byte 0 being written some
   random value if writing to an EEPROM takes place during a power
   failure. This has been fixed on the 164P/640 series (among others).
*/

#include <avr/io.h>
#include <avr/interrupt.h>
#include "eepromMemoryMap.h"

uint8_t EEPROMreadByte(uint16_t Addr)
{
  // Wait for completion of previous write:
  while (EECR & (1 << EEPE));

  // Set up address register:
  EEAR = Addr;

  // Start EEPROM read cycle by setting EERE:
  EECR |= (1 << EERE);

  return EEDR;
}

uint16_t EEPROMreadWord(uint16_t Addr)
{
  uint16_t data;
  data = EEPROMreadByte(Addr+1);
  data = data << 8;
  data += EEPROMreadByte(Addr);
  return data;
}

uint32_t EEPROMread3Bytes(uint16_t Addr)
{
  uint32_t data;
  data = EEPROMreadByte(Addr+2);
  data = data << 8;
  data += EEPROMreadByte(Addr+1);
  data = data << 8;
  data += EEPROMreadByte(Addr);
  return data;
}

uint32_t EEPROMreadDWord(uint16_t Addr)
{
  uint32_t data;
  data = EEPROMreadByte(Addr+3);
  data = data << 8;
  data += EEPROMreadByte(Addr+2);
  data = data << 8;
  data += EEPROMreadByte(Addr+1);
  data = data << 8;
  data += EEPROMreadByte(Addr);
  return data;
}

void EEPROMreadString(uint16_t Addr, char* Data)
{
  for (int i = 0; i < MAXSTR; i++)
  {
  	Data[i] = EEPROMreadByte(Addr + i);

	if (Data[i] == 0)	// Strings are 0 terminated
		  break;
  }
}

void EEPROMwriteByte(uint16_t Addr, uint8_t Data)
{
  // No need to rewrite a byte that already holds the required data:
  if (EEPROMreadByte(Addr) == Data)
    return;

  // Since the above read function does this we don't have to do it again:
  // Wait for completion of previous write:
  // while (EECR & (1 << EEPE));

  uint8_t sreg = SREG;
  cli();  // disable interrupts

  // Set up address and data registers:
  EEAR = Addr;
  EEDR = Data;

  // Write logical one to EEMPE to unlock write access:
//  EECR |= (1 << EEMPE);               // works if compiler optimization option -O2 used

  // Start EEPROM write cycle by setting EEPE:
//  EECR |= (1 << EEPE);

  // Note: If optimization is turned off (-O0) above two statements
  // will not work, so use this instead:
  asm volatile("sbi 0x1F, 0x02"::); // EECR |= (1 << EEMPE);
  asm volatile("sbi 0x1F, 0x01"::); // EECR |= (1 << EEPE);

  SREG = sreg;  // Re-enable interrupts (if they were ever enabled)
}

void EEPROMwriteWord(uint16_t Addr, uint16_t Data)
{
  EEPROMwriteByte(Addr,   Data & 0xFF);
  EEPROMwriteByte(Addr+1, Data >> 8);
}

void EEPROMwrite3Bytes(uint16_t Addr, uint32_t Data)
{
  EEPROMwriteByte(Addr,   Data & 0xFF);
  EEPROMwriteByte(Addr+1, (Data >> 8) & 0xFF);
  EEPROMwriteByte(Addr+2, (Data >> 16) & 0xFF);
}

void EEPROMwriteDWord(uint16_t Addr, uint32_t Data)
{
  EEPROMwriteByte(Addr,   Data & 0xFF);
  EEPROMwriteByte(Addr+1, (Data >> 8) & 0xFF);
  EEPROMwriteByte(Addr+2, (Data >> 16) & 0xFF);
  EEPROMwriteByte(Addr+3, Data >> 24);
}


void EEPROMwriteString(uint16_t Addr, char* Data)
{

	for (int i = 0; i < (MAXSTR - 1); i++)
	{
		EEPROMwriteByte(Addr + i, Data[i]);

		if (Data[i] == 0)	// Strings are 0 terminated
		  break;
	}
	//No matter what, the string field HAS to end in a 0.
	EEPROMwriteByte(Addr + (MAXSTR -1), 0);
}
