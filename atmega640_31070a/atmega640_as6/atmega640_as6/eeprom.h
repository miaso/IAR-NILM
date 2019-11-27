/* 
   EEPROM functions for AVR640/164P series (among others)
   (C) mtech.dk, 2009-10
*/

#ifndef _EEPROM_H_
#define _EEPROM_H_

void EEPROMwriteByte(uint16_t Addr, uint8_t Data);
void EEPROMwriteWord(uint16_t Addr, uint16_t Data);
void EEPROMwrite3Bytes(uint16_t Addr, uint32_t Data);
void EEPROMwriteDWord(uint16_t Addr, uint32_t Data);
void EEPROMwriteString(uint16_t Addr, char* Data);

uint8_t EEPROMreadByte(uint16_t Addr);
uint16_t EEPROMreadWord(uint16_t Addr);
uint32_t EEPROMread3Bytes(uint16_t Addr);
uint32_t EEPROMreadDWord(uint16_t Addr);
void EEPROMreadString(uint16_t Addr, char* Data);

#endif
