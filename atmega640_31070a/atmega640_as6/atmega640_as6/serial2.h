/* 
   Serial port functions for AVR640
   (C) mtech.dk, 2009-10

   Implements serial port 2
*/

#ifndef _serial2_h_
#define _serial2_h_

  void Ser2Setup(uint16_t speed);

  uint16_t Ser2Poll(void);
  uint16_t Ser2Ready(void);
  uint8_t Ser2PutChar(uint8_t);
  void Ser2FlushOutput(void);
  void Ser2ClearInput(void);
  uint8_t Ser2GetChar(int8_t* input);

  void Ser2PrintBuf(char* str, char length);
  void Ser2PrintStr(char* str);
  uint16_t Ser2GetString(char* string, char terminator, uint16_t maxSize);
  uint16_t Ser2GetStringSkipRest(char* string, char terminator, uint16_t maxSize);

#endif 
