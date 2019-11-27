/* 
   Serial port functions for AVR640
   (C) mtech.dk, 2009-10

   Implements serial port 3
*/

#ifndef _serial3_h_
#define _serial3_h_

  void Ser3Setup(uint16_t speed);

  uint16_t Ser3Poll(void);
  uint16_t Ser3Ready(void);
  uint8_t Ser3PutChar(uint8_t output);
  void Ser3FlushOutput(void);
  void Ser3ClearInput(void);
  uint8_t Ser3GetChar(int8_t* input);

  void Ser3PrintBuf(char* str, char length);
  void Ser3PrintStr(char* str);
  uint16_t Ser3GetString(char* string, char terminator, uint16_t maxSize);
  uint16_t Ser3GetStringSkipRest(char* string, char terminator, uint16_t maxSize);

#endif 
