/* 
   Serial port functions for AVR640
   (C) mtech.dk, 2009-10

   Implements serial port 0
*/

#include "project.h"
#include <avr/io.h>
#include <avr/interrupt.h>

uint8_t Ser0inbuf[SERIAL_PORT_0_IN_BUFFER_SIZE];
uint8_t Ser0outbuf[SERIAL_PORT_0_OUT_BUFFER_SIZE];

extern volatile int newSerialData;

volatile uint16_t Ser0in_ptr, Ser0in_sz, Ser0out_ptr, Ser0out_sz;

void Ser0Setup(uint16_t speed)
// speed are ticks send into UBRRx NOT baudrate
// fixed to 8n1
// speed is calculated by: ((F_CPU + UART_BAUD_RATE * 8L) / (UART_BAUD_RATE * 16L) - 1)
// baudrate to tick conversion not done here because it
// is a complex computation and can often be done by the
// programmer pre-compile time
{
  Ser0in_ptr = 0;
  Ser0in_sz = 0;
  Ser0out_ptr = 0;
  Ser0out_sz = 0;

  UBRR0 = speed;

  UCSR0B = 0xB8; // enable rx and tx + interrupts
}

//ISR(SIG_USART0_RECV)
ISR(USART0_RX_vect)
{
  uint8_t data = UDR0;
  if (Ser0in_sz < (SERIAL_PORT_0_IN_BUFFER_SIZE - 1)) {
    uint16_t index = (Ser0in_ptr + Ser0in_sz) % SERIAL_PORT_0_IN_BUFFER_SIZE;
    Ser0inbuf[index] = data;
    Ser0in_sz++;
  }
  else {
    // silently ignore overflow !
//    FLAG_serial0 |= 0x01; // mark overflow 
  }
  // Serial0Int(data);

  STATUS_LED_PORT |= (1 << STATUS_LED_PIN);

  if (data == ';')
    newSerialData = 1;
}

//ISR(SIG_USART0_DATA)
ISR(USART0_UDRE_vect)
{
  if (bit_is_set(UCSR0A, UDRE0)) {
    if (Ser0out_sz > 0) {
      UDR0 = Ser0outbuf[Ser0out_ptr];
      Ser0out_sz--;
      Ser0out_ptr = Ser0out_ptr < (SERIAL_PORT_0_OUT_BUFFER_SIZE-1) ? (Ser0out_ptr + 1) : 0;
    }
    else {
      // queue is empty
      UCSR0B &= 0xDF; // bit 5 = UDRIE
      // Serial0Empty();
    }
  }
}

uint16_t Ser0Poll(void)
{
  uint8_t sreg = SREG;
  cli();

  uint16_t size = Ser0in_sz;

  SREG = sreg;  // Re-enable interrupts (if they were ever enabled)

  return size;
}

uint16_t Ser0Ready(void)
{
  uint8_t sreg = SREG;
  cli();

  uint16_t used = Ser0out_sz;

  SREG = sreg;  // Re-enable interrupts (if they were ever enabled)

  return (SERIAL_PORT_0_OUT_BUFFER_SIZE - used - 1);
}

uint8_t Ser0PutChar(uint8_t output) 
{
  if (!Ser0Ready())
    return 0;

  uint8_t sreg = SREG;
  cli();

  uint16_t index = (Ser0out_ptr + Ser0out_sz) % SERIAL_PORT_0_OUT_BUFFER_SIZE; 
  Ser0outbuf[ index ] = output;
  Ser0out_sz++;

  UCSR0B |= 0x20; // bit 5 = UDRIE

  SREG = sreg;  // Re-enable interrupts (if they were ever enabled)
     
  return 1;
}

void Ser0FlushOutput(void)
{
  uint8_t sreg = SREG;
  cli();

  Ser0out_sz = 0;
  Ser0out_ptr = 0;

  SREG = sreg;  // Re-enable interrupts (if they were ever enabled)
}

void Ser0ClearInput(void)
{
  uint8_t sreg = SREG;
  cli();

  Ser0in_sz = 0;
  Ser0in_ptr = 0;

  SREG = sreg;  // Re-enable interrupts (if they were ever enabled)
}

uint8_t Ser0GetChar(int8_t* input) 
{
  if (!Ser0Poll())
    return 0;

  uint8_t sreg = SREG;
  cli();

  *input = Ser0inbuf[Ser0in_ptr];
  Ser0in_sz--;
  Ser0in_ptr = (Ser0in_ptr < (SERIAL_PORT_0_IN_BUFFER_SIZE-1)) ? (Ser0in_ptr + 1) : 0;
	       
  SREG = sreg;  // Re-enable interrupts (if they were ever enabled)
  return 1;
}

void Ser0PrintBuf(char* str, char length) 
{
  while (length--) {
    if (Ser0Ready()) {
      Ser0PutChar(*str);
      str++;
    }
  }
}

void Ser0PrintStr(char* str) 
{
  while (*str) {
    if (Ser0Ready()) {
      Ser0PutChar(*str);
      str++;
    }
  }
}

uint16_t Ser0GetString(char* string, char terminator, uint16_t maxSize)
{
  uint16_t a = 0;
  for (a = 0; a < maxSize; a++)
  {
    if (!Ser0Poll())
      break;

    uint8_t sreg = SREG;
    cli();

    *(string+a) = Ser0inbuf[Ser0in_ptr];
    Ser0in_sz--;
    Ser0in_ptr = (Ser0in_ptr < (SERIAL_PORT_0_IN_BUFFER_SIZE-1)) ? (Ser0in_ptr + 1) : 0;
    SREG = sreg;  // Re-enable interrupts (if they were ever enabled)

    if (*(string+a) == terminator)
      break;
  }
  return a;
}

uint16_t Ser0GetStringSkipRest(char* string, char terminator, uint16_t maxSize)
{
  uint16_t a;
  for (a = 0; a < maxSize; a++)
  {
    if (!Ser0Poll())
      return a;

    uint8_t sreg = SREG;
    cli();

    *(string+a) = Ser0inbuf[Ser0in_ptr];
    Ser0in_sz--;
    Ser0in_ptr = (Ser0in_ptr < (SERIAL_PORT_0_IN_BUFFER_SIZE-1)) ? (Ser0in_ptr + 1) : 0;
    SREG = sreg;  // Re-enable interrupts (if they were ever enabled)

    if (*(string+a) == terminator)
      return a;
  }

  uint8_t terminatorFound = 0;
  while ((Ser0Poll()) && !terminatorFound)
  {
    uint8_t sreg = SREG;
    cli();

    if (Ser0inbuf[Ser0in_ptr] == terminator)
      terminatorFound = 1; 
      // we still need to skip past the terminator:

    Ser0in_sz--;
    Ser0in_ptr = (Ser0in_ptr < (SERIAL_PORT_0_IN_BUFFER_SIZE-1)) ? (Ser0in_ptr + 1) : 0;
    SREG = sreg;  // Re-enable interrupts (if they were ever enabled)
  }

  return a;
}

