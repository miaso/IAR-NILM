/* 
   Serial port functions for AVR640
   (C) mtech.dk, 2009-10

   Implements serial port 3
*/

#include "project.h"
#include <avr/io.h>
#include <avr/interrupt.h>

uint8_t Ser3inbuf[SERIAL_PORT_3_IN_BUFFER_SIZE];
uint8_t Ser3outbuf[SERIAL_PORT_3_OUT_BUFFER_SIZE];

volatile uint16_t Ser3in_ptr, Ser3in_sz, Ser3out_ptr, Ser3out_sz;

void Ser3Setup(uint16_t speed)
// speed are ticks send into UBRRx NOT baudrate
// fixed to 8n1
// speed is calculated by: ((F_CPU + UART_BAUD_RATE * 8L) / (UART_BAUD_RATE * 16L) - 1)
// baudrate to tick conversion not done here because it
// is a complex computation and can often be done by the
// programmer pre-compile time
{
  Ser3in_ptr = 0;
  Ser3in_sz = 0;
  Ser3out_ptr = 0;
  Ser3out_sz = 0;

  UBRR3 = speed;

  UCSR3B = 0xB8; // enable rx and tx + interrupts
}

ISR(SIG_USART3_RECV)
{
  uint8_t data = UDR3;
  if (Ser3in_sz < (SERIAL_PORT_3_IN_BUFFER_SIZE - 1)) {
    uint16_t index = (Ser3in_ptr + Ser3in_sz) % SERIAL_PORT_3_IN_BUFFER_SIZE;
    Ser3inbuf[index] = data;
    Ser3in_sz++;
  }
  else {
    // silently ignore overflow !
//    FLAG_serial3 |= 0x01; // mark overflow 
  }
  Serial3Int(data);
}

ISR(SIG_USART3_DATA)
{
  if (bit_is_set(UCSR3A, UDRE0)) {
    if (Ser3out_sz > 0) {
      UDR3 = Ser3outbuf[Ser3out_ptr];
      Ser3out_sz--;
      Ser3out_ptr = Ser3out_ptr < (SERIAL_PORT_3_OUT_BUFFER_SIZE-1) ? (Ser3out_ptr + 1) : 0;
    }
    else {
      // queue is empty
      UCSR3B &= 0xDF; // bit 5 = UDRIE
    }
  }
}

uint16_t Ser3Poll(void)
{
  uint8_t sreg = SREG;
  cli();

  uint16_t size = Ser3in_sz;

  SREG = sreg;  // Re-enable interrupts (if they were ever enabled)

  return size;
}

uint16_t Ser3Ready(void)
{
  uint8_t sreg = SREG;
  cli();

  uint16_t used = Ser3out_sz;

  SREG = sreg;  // Re-enable interrupts (if they were ever enabled)

  return (1); //????????

  return (SERIAL_PORT_3_OUT_BUFFER_SIZE - used - 1);
}

uint8_t Ser3PutChar(uint8_t output) 
{
  if (!Ser3Ready())
    return 0;

  uint8_t sreg = SREG;
  cli();

  uint16_t index = (Ser3out_ptr + Ser3out_sz) % SERIAL_PORT_3_OUT_BUFFER_SIZE; 
  Ser3outbuf[ index ] = output;
  Ser3out_sz++;

  UCSR3B |= 0x20; // bit 5 = UDRIE

  SREG = sreg;  // Re-enable interrupts (if they were ever enabled)
     
  return 1;
}

void Ser3FlushOutput(void)
{
  uint8_t sreg = SREG;
  cli();

  Ser3out_sz = 0;
  Ser3out_ptr = 0;

  SREG = sreg;  // Re-enable interrupts (if they were ever enabled)
}

void Ser3ClearInput(void)
{
  uint8_t sreg = SREG;
  cli();

  Ser3in_sz = 0;
  Ser3in_ptr = 0;

  SREG = sreg;  // Re-enable interrupts (if they were ever enabled)
}

uint8_t Ser3GetChar(int8_t* input) 
{
  if (!Ser3Poll())
    return 0;

  uint8_t sreg = SREG;
  cli();

  *input = Ser3inbuf[Ser3in_ptr];
  Ser3in_sz--;
  Ser3in_ptr = (Ser3in_ptr < (SERIAL_PORT_3_IN_BUFFER_SIZE-1)) ? (Ser3in_ptr + 1) : 0;
	       
  SREG = sreg;  // Re-enable interrupts (if they were ever enabled)
  return 1;
}

void Ser3PrintBuf(char* str, char length) 
{
  while (length--) {
    if (Ser3Ready()) {
      Ser3PutChar(*str);
      str++;
    }
	// else?????
  }
}

void Ser3PrintStr(char* str) 
{
  while (*str) {
    if (Ser3Ready()) {
      Ser3PutChar(*str);
      str++;
    }
  }
}

uint16_t Ser3GetString(char* string, char terminator, uint16_t maxSize)
{
  uint16_t a = 0;
  for (a = 0; a < maxSize; a++)
  {
    if (!Ser3Poll())
      break;

    uint8_t sreg = SREG;
    cli();

    *(string+a) = Ser3inbuf[Ser3in_ptr];
    Ser3in_sz--;
    Ser3in_ptr = (Ser3in_ptr < (SERIAL_PORT_3_IN_BUFFER_SIZE-1)) ? (Ser3in_ptr + 1) : 0;
    SREG = sreg;  // Re-enable interrupts (if they were ever enabled)

	// PDO:  Hack hack
//    if (*(string+a) == terminator)
//      break;
  }
  return a;
}

uint16_t Ser3GetStringSkipRest(char* string, char terminator, uint16_t maxSize)
{
  uint16_t a;
  for (a = 0; a < maxSize; a++)
  {
    if (!Ser3Poll())
      return a;

    uint8_t sreg = SREG;
    cli();

    *(string+a) = Ser3inbuf[Ser3in_ptr];
    Ser3in_sz--;
    Ser3in_ptr = (Ser3in_ptr < (SERIAL_PORT_3_IN_BUFFER_SIZE-1)) ? (Ser3in_ptr + 1) : 0;
    SREG = sreg;  // Re-enable interrupts (if they were ever enabled)

    if (*(string+a) == terminator)
      return a;
  }

  uint8_t terminatorFound = 0;
  while ((Ser3Poll()) && !terminatorFound)
  {
    uint8_t sreg = SREG;
    cli();

    if (Ser3inbuf[Ser3in_ptr] == terminator)
      terminatorFound = 1; 
      // we still need to skip past the terminator:

    Ser3in_sz--;
    Ser3in_ptr = (Ser3in_ptr < (SERIAL_PORT_3_IN_BUFFER_SIZE-1)) ? (Ser3in_ptr + 1) : 0;
    SREG = sreg;  // Re-enable interrupts (if they were ever enabled)
  }

  return a;
}

