/* 
   Serial port functions for AVR640
   (C) mtech.dk, 2009-10

   Implements serial port 0
*/

#ifndef _serial0_h_
#define _serial0_h_

  void Ser0Setup(uint16_t speed);

  uint16_t Ser0Poll(void);
  uint16_t Ser0Ready(void);
  uint8_t Ser0PutChar(uint8_t);
  void Ser0FlushOutput(void);
  void Ser0ClearInput(void);
  uint8_t Ser0GetChar(int8_t* input);

  void Ser0PrintBuf(char* str, char length);
  void Ser0PrintStr(char* str);
  uint16_t Ser0GetString(char* string, char terminator, uint16_t maxSize);
  uint16_t Ser0GetStringSkipRest(char* string, char terminator, uint16_t maxSize);

#endif 
