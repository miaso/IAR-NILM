/* 
   Serial port functions for AVR640
   (C) mtech.dk, 2009-10

   Implements serial port 1
*/

#include "project.h"
#include <avr/io.h>
#include <avr/interrupt.h>

uint8_t Ser1inbuf[SERIAL_PORT_1_IN_BUFFER_SIZE];
uint8_t Ser1outbuf[SERIAL_PORT_1_OUT_BUFFER_SIZE];

volatile uint16_t Ser1in_ptr, Ser1in_sz, Ser1out_ptr, Ser1out_sz;

void Ser1Setup(uint16_t speed)
// speed are ticks send into UBRRx NOT baudrate
// fixed to 8n1
// speed is calculated by: ((F_CPU + UART_BAUD_RATE * 8L) / (UART_BAUD_RATE * 16L) - 1)
// baudrate to tick conversion not done here because it
// is a complex computation and can often be done by the
// programmer pre-compile time
{
  Ser1in_ptr = 0;
  Ser1in_sz = 0;
  Ser1out_ptr = 0;
  Ser1out_sz = 0;

  UBRR1 = speed;

  UCSR1B = 0xB8; // enable rx and tx + interrupts
}

//ISR(SIG_USART1_RECV)
ISR(USART1_RXC_vect)
{
  uint8_t data = UDR1;
  if (Ser1in_sz < (SERIAL_PORT_1_IN_BUFFER_SIZE - 1)) {
    uint16_t index = (Ser1in_ptr + Ser1in_sz) % SERIAL_PORT_1_IN_BUFFER_SIZE;
    Ser1inbuf[index] = data;
    Ser1in_sz++;
  }
  else {
    // silently ignore overflow !
//    FLAG_serial1 |= 0x01; // mark overflow 
  }
  Serial1Int(data);
}

ISR(SIG_USART1_DATA)
{
  if (bit_is_set(UCSR1A, UDRE0)) {
    if (Ser1out_sz > 0) {
      UDR1 = Ser1outbuf[Ser1out_ptr];
      Ser1out_sz--;
      Ser1out_ptr = Ser1out_ptr < (SERIAL_PORT_1_OUT_BUFFER_SIZE-1) ? (Ser1out_ptr + 1) : 0;
    }
    else {
      // queue is empty
      UCSR1B &= 0xDF; // bit 5 = UDRIE
      Serial1Empty();
    }
  }
}

uint16_t Ser1Poll(void)
{
  uint8_t sreg = SREG;
  cli();

  uint16_t size = Ser1in_sz;

  SREG = sreg;  // Re-enable interrupts (if they were ever enabled)

  return size;
}

uint16_t Ser1Ready(void)
{
  uint8_t sreg = SREG;
  cli();

  uint16_t used = Ser1out_sz;

  SREG = sreg;  // Re-enable interrupts (if they were ever enabled)

  return (SERIAL_PORT_1_OUT_BUFFER_SIZE - used - 1);
}

uint8_t Ser1PutChar(uint8_t output) 
{
  if (!Ser1Ready())
    return 0;

  uint8_t sreg = SREG;
  cli();

  uint16_t index = (Ser1out_ptr + Ser1out_sz) % SERIAL_PORT_1_OUT_BUFFER_SIZE; 
  Ser1outbuf[ index ] = output;
  Ser1out_sz++;

  UCSR1B |= 0x20; // bit 5 = UDRIE

  SREG = sreg;  // Re-enable interrupts (if they were ever enabled)
     
  return 1;
}

void Ser1FlushOutput(void)
{
  uint8_t sreg = SREG;
  cli();

  Ser1out_sz = 0;
  Ser1out_ptr = 0;

  SREG = sreg;  // Re-enable interrupts (if they were ever enabled)
}

void Ser1ClearInput(void)
{
  uint8_t sreg = SREG;
  cli();

  Ser1in_sz = 0;
  Ser1in_ptr = 0;

  SREG = sreg;  // Re-enable interrupts (if they were ever enabled)
}

uint8_t Ser1GetChar(int8_t* input) 
{
  if (!Ser1Poll())
    return 0;

  uint8_t sreg = SREG;
  cli();

  *input = Ser1inbuf[Ser1in_ptr];
  Ser1in_sz--;
  Ser1in_ptr = (Ser1in_ptr < (SERIAL_PORT_1_IN_BUFFER_SIZE-1)) ? (Ser1in_ptr + 1) : 0;
	       
  SREG = sreg;  // Re-enable interrupts (if they were ever enabled)
  return 1;
}

void Ser1PrintBuf(char* str, char length) 
{
  while (length--) {
    if (Ser1Ready()) {
      Ser1PutChar(*str);
      str++;
    }
  }
}

void Ser1PrintStr(char* str) 
{
  while (*str) {
    if (Ser1Ready()) {
      Ser1PutChar(*str);
      str++;
    }
  }
}

uint16_t Ser1GetString(char* string, char terminator, uint16_t maxSize)
{
  uint16_t a = 0;
  for (a = 0; a < maxSize; a++)
  {
    if (!Ser1Poll())
      break;

    uint8_t sreg = SREG;
    cli();

    *(string+a) = Ser1inbuf[Ser1in_ptr];
    Ser1in_sz--;
    Ser1in_ptr = (Ser1in_ptr < (SERIAL_PORT_1_IN_BUFFER_SIZE-1)) ? (Ser1in_ptr + 1) : 0;
    SREG = sreg;  // Re-enable interrupts (if they were ever enabled)

    if (*(string+a) == terminator)
      break;
  }
  return a;
}

uint16_t Ser1GetStringSkipRest(char* string, char terminator, uint16_t maxSize)
{
  uint16_t a;
  for (a = 0; a < maxSize; a++)
  {
    if (!Ser1Poll())
      return a;

    uint8_t sreg = SREG;
    cli();

    *(string+a) = Ser1inbuf[Ser1in_ptr];
    Ser1in_sz--;
    Ser1in_ptr = (Ser1in_ptr < (SERIAL_PORT_1_IN_BUFFER_SIZE-1)) ? (Ser1in_ptr + 1) : 0;
    SREG = sreg;  // Re-enable interrupts (if they were ever enabled)

    if (*(string+a) == terminator)
      return a;
  }

  uint8_t terminatorFound = 0;
  while ((Ser1Poll()) && !terminatorFound)
  {
    uint8_t sreg = SREG;
    cli();

    if (Ser1inbuf[Ser1in_ptr] == terminator)
      terminatorFound = 1; 
      // we still need to skip past the terminator:

    Ser1in_sz--;
    Ser1in_ptr = (Ser1in_ptr < (SERIAL_PORT_1_IN_BUFFER_SIZE-1)) ? (Ser1in_ptr + 1) : 0;
    SREG = sreg;  // Re-enable interrupts (if they were ever enabled)
  }

  return a;
}

