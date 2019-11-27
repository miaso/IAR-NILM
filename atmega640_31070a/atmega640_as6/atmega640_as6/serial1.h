/* 
   Serial port functions for AVR640
   (C) mtech.dk, 2009-10

   Implements serial port 1
*/

#ifndef _serial1_h_
#define _serial1_h_

  void Ser1Setup(uint16_t speed);

  uint16_t Ser1Poll(void);
  uint16_t Ser1Ready(void);
  uint8_t Ser1PutChar(uint8_t output);
  void Ser1FlushOutput(void);
  void Ser1ClearInput(void);
  uint8_t Ser1GetChar(int8_t* input);

  void Ser1PrintBuf(char* str, char length);
  void Ser1PrintStr(char* str);
  uint16_t Ser1GetString(char* string, char terminator, uint16_t maxSize);
  uint16_t Ser1GetStringSkipRest(char* string, char terminator, uint16_t maxSize);

#endif 
