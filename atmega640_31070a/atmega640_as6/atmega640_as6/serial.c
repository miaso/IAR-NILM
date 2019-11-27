/* 
   Serial port functions for AVR640
   (C) mtech.dk, 2009-10

   Implements general serial port functions
   These are optional overhead, which makes it easy to
   switch between ports at a later time
   also implements features allowing the use of printf()
   printf() support currently implemented using polled i/o
   this could be changed to interrupt based i/o
*/

#include <avr/io.h>
#include <avr/interrupt.h>

#include "project.h"
#include "serial.h" 
#include "serial0.h"
#include "serial1.h"
#include "serial2.h"
#include "serial3.h"

#ifdef SERIAL_STDOUT_PORT

#include <stdio.h>

#define SERIAL_STDOUT_BAUD_REGISTERS  SERIAL_BAUD_REGISTERS(SERIAL_STDOUT_BAUD_RATE)

#if SERIAL_STDOUT_PORT == 0
  #define SERIAL_STDOUT_UBRRx  UBRR0
  #define SERIAL_STDOUT_UDRx   UDR0
  #define SERIAL_STDOUT_UCSRxA UCSR0A
  #define SERIAL_STDOUT_UCSRxB UCSR0B
  #define SERIAL_STDOUT_UCSRxC UCSR0C
#elif SERIAL_STDOUT_PORT == 1
  #define SERIAL_STDOUT_UBRRx  UBRR1
  #define SERIAL_STDOUT_UDRx   UDR1
  #define SERIAL_STDOUT_UCSRxA UCSR1A
  #define SERIAL_STDOUT_UCSRxB UCSR1B
  #define SERIAL_STDOUT_UCSRxC UCSR1C
#elif SERIAL_STDOUT_PORT == 2
  #define SERIAL_STDOUT_UBRRx  UBRR2
  #define SERIAL_STDOUT_UDRx   UDR2
  #define SERIAL_STDOUT_UCSRxA UCSR2A
  #define SERIAL_STDOUT_UCSRxB UCSR2B
  #define SERIAL_STDOUT_UCSRxC UCSR2C
#elif SERIAL_STDOUT_PORT == 3
  #define SERIAL_STDOUT_UBRRx  UBRR3
  #define SERIAL_STDOUT_UDRx   UDR3
  #define SERIAL_STDOUT_UCSRxA UCSR3A
  #define SERIAL_STDOUT_UCSRxB UCSR3B
  #define SERIAL_STDOUT_UCSRxC UCSR3C
#endif

int printCHAR(char character, FILE *stream)
// note: polled i/o
{ 
  while ((SERIAL_STDOUT_UCSRxA & (1 << UDRE0)) == 0) {}; 
  SERIAL_STDOUT_UDRx = character; 
  return 0; 
}

FILE uart_str = FDEV_SETUP_STREAM(printCHAR, NULL, _FDEV_SETUP_RW);

void SerSetupSTDOUT()
// initializes STDOUT serial port
{
//  SERIAL_STDOUT_UCSRxB |= (1 << RXEN0) | (1 << TXEN0); 
  SERIAL_STDOUT_UCSRxB |= (1 << RXEN0) | (1 << TXEN0) | (1 << RXCIE0); 
  SERIAL_STDOUT_UCSRxC |= (1 << UCSZ00) | (1 << UCSZ01); 

  SERIAL_STDOUT_UBRRx = SERIAL_STDOUT_BAUD_REGISTERS;

  stdout = &uart_str; 
}

#endif // define SERIAL_STDOUT_PORT

void SerSetup(uint8_t port, uint16_t speed)
// initializes serial port
// speed is ticks written into UBRRx NOT baudrate
// fixed to 8n1
// speed is calculated by: ((F_CPU + UART_BAUD_RATE * 8L) / (UART_BAUD_RATE * 16L) - 1)
// baudrate to tick conversion not done here because it
// is a complex computation and can often be done at compiletime (or earlier)
{
#ifdef SERIAL0
  if (port == 0) Ser0Setup(speed);
#endif

#ifdef SERIAL1
  if (port == 1) Ser1Setup(speed);
#endif

#ifdef SERIAL2
  if (port == 2) Ser2Setup(speed);
#endif

#ifdef SERIAL3
  if (port == 3) Ser3Setup(speed);
#endif
}

// Initializes the serial port 3 used by Vestfrost
void VestfrostSerSetup()
{
	UCSR3C = 0x0E;	// Two stop bits!
	SerSetup(3, SERIAL_BAUD_REGISTERS(9600) );
}

uint16_t SerPoll(uint8_t port)
// returns number of characters in input buffer, 0 if empty
{
#ifdef SERIAL0
  if (port == 0) return Ser0Poll();
#endif

#ifdef SERIAL1
  if (port == 1) return Ser1Poll();
#endif

#ifdef SERIAL2
  if (port == 2) return Ser2Poll();
#endif

#ifdef SERIAL3
  if (port == 3) return Ser3Poll();
#endif

  return 0;
}

uint16_t SerReady(uint8_t port)
// returns number of free bytes on the output buffer, 0 if full
{
#ifdef SERIAL0
  if (port == 0) return Ser0Ready();
#endif

#ifdef SERIAL1
  if (port == 1) return Ser1Ready();
#endif

#ifdef SERIAL2
  if (port == 2) return Ser2Ready();
#endif

#ifdef SERIAL3
  if (port == 3) return Ser3Ready();
#endif

  return 0;
}

uint8_t SerPutChar(uint8_t port, uint8_t output)
// outputs a single character
{
#ifdef SERIAL0
  if (port == 0) return Ser0PutChar(output);
#endif

#ifdef SERIAL1
  if (port == 1) return Ser1PutChar(output);
#endif

#ifdef SERIAL2
  if (port == 2) return Ser2PutChar(output);
#endif

#ifdef SERIAL3
  if (port == 3) return Ser3PutChar(output);
#endif

  return 0;
}

void SerFlushOutput(uint8_t port)
// flushes output buffer
{
#ifdef SERIAL0
  if (port == 0) Ser0FlushOutput();
#endif

#ifdef SERIAL1
  if (port == 1) Ser1FlushOutput();
#endif

#ifdef SERIAL2
  if (port == 2) Ser2FlushOutput();
#endif

#ifdef SERIAL3
  if (port == 3) Ser3FlushOutput();
#endif
}

void SerClearInput(uint8_t port)
// empties input buffer
{
#ifdef SERIAL0
  if (port == 0) Ser0ClearInput();
#endif

#ifdef SERIAL1
  if (port == 1) Ser1ClearInput();
#endif

#ifdef SERIAL2
  if (port == 2) Ser2ClearInput();
#endif

#ifdef SERIAL3
  if (port == 3) Ser3ClearInput();
#endif
}

uint8_t SerGetChar(uint8_t port, char* input)
// moves a single character into input, returns 0 if no data available
{
#ifdef SERIAL0
  if (port == 0) return Ser0GetChar(input);
#endif

#ifdef SERIAL1
  if (port == 1) return Ser1GetChar(input);
#endif

#ifdef SERIAL2
  if (port == 2) return Ser2GetChar(input);
#endif

#ifdef SERIAL3
  if (port == 3) return Ser3GetChar(input);
#endif

  return 0;
}

void SerPrintBuf(uint8_t port, char* str, char length)
// outputs a binary string of length length using SerGetChar()
{
#ifdef SERIAL0
  if (port == 0) Ser0PrintBuf(str, length);
#endif

#ifdef SERIAL1
  if (port == 1) Ser1PrintBuf(str, length);
#endif

#ifdef SERIAL2
  if (port == 2) Ser2PrintBuf(str, length);
#endif

#ifdef SERIAL3
  if (port == 3) Ser3PrintBuf(str, length);
#endif
}

void SerPrintStr(uint8_t port, char* str)
// outputs a zero-terminated string using SerGetChar()
{
#ifdef SERIAL0
  if (port == 0) Ser0PrintStr(str);
#endif

#ifdef SERIAL1
  if (port == 1) Ser1PrintStr(str);
#endif

#ifdef SERIAL2
  if (port == 2) Ser2PrintStr(str);
#endif

#ifdef SERIAL3
  if (port == 3) Ser3PrintStr(str);
#endif
}

uint16_t SerGetString(uint8_t port, char* string, char terminator, uint16_t maxSize)
// moves received data into string
// stops at terminator or after at most maxSize characters
// returns number of characters copied
{
#ifdef SERIAL0
  if (port == 0) return Ser0GetString(string, terminator, maxSize);
#endif

#ifdef SERIAL1
  if (port == 1) return Ser1GetString(string, terminator, maxSize);
#endif

#ifdef SERIAL2
  if (port == 2) return Ser2GetString(string, terminator, maxSize);
#endif

#ifdef SERIAL3
  if (port == 3) return Ser3GetString(string, terminator, maxSize);
#endif

  return 0;
}

uint16_t SerGetStringSkipRest(uint8_t port, char* string, char terminator, uint16_t maxSize)
// moves received data into string
// stops at terminator or after at most maxSize characters
// if terminator not found before maxSize, further data is removed from the queue until
// terminator is found, or the queue becomes empty
// returns number of characters copied
{
#ifdef SERIAL0
  if (port == 0) return Ser0GetStringSkipRest(string, terminator, maxSize);
#endif

#ifdef SERIAL1
  if (port == 1) return Ser1GetStringSkipRest(string, terminator, maxSize);
#endif

#ifdef SERIAL2
  if (port == 2) return Ser2GetStringSkipRest(string, terminator, maxSize);
#endif

#ifdef SERIAL3
  if (port == 3) return Ser3GetStringSkipRest(string, terminator, maxSize);
#endif

  return 0;
}
