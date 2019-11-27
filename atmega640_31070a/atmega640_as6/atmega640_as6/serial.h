/* 
   Serial port functions for AVR640
   (C) mtech.dk, 2009-10

   Implements general serial port functions
   These are optional overhead, which makes it easy to
   switch between ports at a later time
   also implements features allowing the use of printf()
*/

#ifndef _serial_h_
#define _serial_h_

#define VESTFROST_UART 3	// Note: If this is changed, the setup function needs to be changed too.
#define DEVI_UART 3

  // Formula taken from chip docs.
#define SERIAL_BAUD_REGISTERS(x)  ((F_CPU + x * 8L) / (x * 16L) - 1)



  #ifdef SERIAL_STDOUT_PORT
    void SerSetupSTDOUT();
  #endif

  void VestfrostSerSetup();

  void SerSetup(uint8_t port, uint16_t speed);

  uint16_t SerPoll(uint8_t port);                       // Returns number of bytes in the input buffer (0 if empty)
  uint16_t SerReady(uint8_t port);                      // Returns number of free bytes left in the output buffer (0 if full)
  uint8_t SerPutChar(uint8_t port, uint8_t output);
  void SerFlushOutput(uint8_t port);
  void SerClearInput(uint8_t port);
  uint8_t SerGetChar(uint8_t port, char*);

  void SerPrintBuf(uint8_t port, char*, char);			// outputs a binary string of length length using SerGetChar()
  void SerPrintStr(uint8_t port, char*);
  uint16_t SerGetString(uint8_t port, char*, char, uint16_t);
  uint16_t SerGetStringSkipRest(uint8_t, char*, char, uint16_t);

#endif 
