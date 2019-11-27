/* 
   Serial port functions for AVR640
   (C) mtech.dk, 2009-10

   Implements serial port 2
*/

#include "project.h"

#include <avr/io.h>
#include <avr/interrupt.h>

uint8_t Ser2inbuf[SERIAL_PORT_2_IN_BUFFER_SIZE];
uint8_t Ser2outbuf[SERIAL_PORT_2_OUT_BUFFER_SIZE];

volatile uint16_t Ser2in_ptr, Ser2in_sz, Ser2out_ptr, Ser2out_sz;

// This variable will keep flags, at first the only thing we're interested in is 
// GSM overflows.
volatile uint8_t Ser2StatusFlag = 0;
#define SER2_OVERFLOW_FLAG 0x01


void Ser2Setup(uint16_t speed)
// speed are ticks send into UBRRx NOT baudrate
// fixed to 8n1
// speed is calculated by: ((F_CPU + UART_BAUD_RATE * 8L) / (UART_BAUD_RATE * 16L) - 1)
// baudrate to tick conversion not done here because it
// is a complex computation and can often be done at compiletime (or earlier)
{
  Ser2in_ptr = 0;
  Ser2in_sz = 0;
  Ser2out_ptr = 0;
  Ser2out_sz = 0;

  UBRR2 = speed;

  UCSR2B = 0xB8; // enable rx and tx + interrupts
}

ISR(SIG_USART2_RECV)
{
  uint8_t data = UDR2;
  if (Ser2in_sz < (SERIAL_PORT_2_IN_BUFFER_SIZE - 1)) {
    uint16_t index = (Ser2in_ptr + Ser2in_sz) % SERIAL_PORT_2_IN_BUFFER_SIZE;
    Ser2inbuf[index] = data;
    Ser2in_sz++;
  }
  else {
    Ser2StatusFlag |= SER2_OVERFLOW_FLAG; // mark overflow 
  }
  Serial2Int(data);
}

ISR(SIG_USART2_DATA)
{
  if (bit_is_set(UCSR2A, UDRE0)) {
    if (Ser2out_sz > 0) {
      UDR2 = Ser2outbuf[Ser2out_ptr];
      Ser2out_sz--;
      Ser2out_ptr = Ser2out_ptr < (SERIAL_PORT_2_OUT_BUFFER_SIZE-1) ? (Ser2out_ptr + 1) : 0;
    }
    else {
      // queue is empty
      UCSR2B &= 0xDF; // bit 5 = UDRIE
//      Serial2Empty();
    }
  }
}

uint16_t Ser2Poll(void)
{
  uint8_t sreg = SREG;
  cli();

  uint16_t size = Ser2in_sz;

  SREG = sreg;  // Re-enable interrupts (if they were ever enabled)

  return size;
}

uint16_t Ser2Ready(void)
{
  uint8_t sreg = SREG;
  cli();

  uint16_t used = Ser2out_sz;

  SREG = sreg;  // Re-enable interrupts (if they were ever enabled)

  return (SERIAL_PORT_2_OUT_BUFFER_SIZE - used - 1);
}

uint8_t Ser2PutChar(uint8_t output) 
{
  if (!Ser2Ready())
    return 0;

  uint8_t sreg = SREG;
  cli();

  uint16_t index = (Ser2out_ptr + Ser2out_sz) % SERIAL_PORT_2_OUT_BUFFER_SIZE; 
  Ser2outbuf[ index ] = output;
  Ser2out_sz++;

  UCSR2B |= 0x20; // bit 5 = UDRIE

  SREG = sreg;  // Re-enable interrupts (if they were ever enabled)
     
  return 1;
}

void Ser2FlushOutput(void)
{
  uint8_t sreg = SREG;
  cli();

  Ser2out_sz = 0;
  Ser2out_ptr = 0;

  SREG = sreg;  // Re-enable interrupts (if they were ever enabled)
}

void Ser2ClearInput(void)
{
  uint8_t sreg = SREG;
  cli();

  Ser2in_sz = 0;
  Ser2in_ptr = 0;

  SREG = sreg;  // Re-enable interrupts (if they were ever enabled)
  Ser2StatusFlag = 0;
}

uint8_t Ser2GetChar(int8_t* input) 
{
  if (!Ser2Poll())
    return 0;

  uint8_t sreg = SREG;
  cli();

  *input = Ser2inbuf[Ser2in_ptr];
  Ser2in_sz--;
  Ser2in_ptr = (Ser2in_ptr < (SERIAL_PORT_2_IN_BUFFER_SIZE-1)) ? (Ser2in_ptr + 1) : 0;
	       
  SREG = sreg;  // Re-enable interrupts (if they were ever enabled)
  return 1;
}

void Ser2PrintBuf(char* str, char length) 
{
  while (length--) {
    if (Ser2Ready()) {
      Ser2PutChar(*str);
      str++;
    }
  }
}

void Ser2PrintStr(char* str) 
{

#ifdef MODEM_TRACE
  printf("Mdm>%s\r\n", str);
#endif

  while (*str) {
    if (Ser2Ready()) {
      Ser2PutChar(*str);
      str++;
    }
  }
}

uint16_t Ser2GetString(char* string, char terminator, uint16_t maxSize)
{
  uint16_t a = 0;

#ifdef MODEM_TRACE
  if (Ser2StatusFlag)
  {
  	printf("!!! Ser2StatusFlag: %d\r\n", Ser2StatusFlag);
	Ser2StatusFlag = 0;
  }

  printf("Mdm<");
#endif

  for (a = 0; a < maxSize; a++)
  {
    if (!Ser2Poll())
      break;

    uint8_t sreg = SREG;
    cli();

#ifdef MODEM_TRACE
  printf("%c", Ser2inbuf[Ser2in_ptr]);
#endif

    *(string+a) = Ser2inbuf[Ser2in_ptr];
    Ser2in_sz--;
    Ser2in_ptr = (Ser2in_ptr < (SERIAL_PORT_2_IN_BUFFER_SIZE-1)) ? (Ser2in_ptr + 1) : 0;
    SREG = sreg;  // Re-enable interrupts (if they were ever enabled)

    if (*(string+a) == terminator)
      break;
  }
  return a;
}

uint16_t Ser2GetStringSkipRest(char* string, char terminator, uint16_t maxSize)
{
  uint16_t a;

#ifdef MODEM_TRACE
  if (Ser2StatusFlag)
  {
  	printf("!!! Ser2StatusFlag: %d\r\n", Ser2StatusFlag);
	Ser2StatusFlag = 0;
  }

  printf("Mdm<");
#endif

  for (a = 0; a < maxSize; a++)
  {
    if (!Ser2Poll())
      return a;

    uint8_t sreg = SREG;
    cli();

#ifdef MODEM_TRACE
    printf("%c", Ser2inbuf[Ser2in_ptr]);
#endif

    *(string+a) = Ser2inbuf[Ser2in_ptr];
    Ser2in_sz--;
    Ser2in_ptr = (Ser2in_ptr < (SERIAL_PORT_2_IN_BUFFER_SIZE-1)) ? (Ser2in_ptr + 1) : 0;
    SREG = sreg;  // Re-enable interrupts (if they were ever enabled)

    if (*(string+a) == terminator)
      return a;
  }

  uint8_t terminatorFound = 0;
  while ((Ser2Poll()) && !terminatorFound)
  {
    uint8_t sreg = SREG;
    cli();

    if (Ser2inbuf[Ser2in_ptr] == terminator)
      terminatorFound = 1; 
      // we still need to skip past the terminator:

    Ser2in_sz--;
    Ser2in_ptr = (Ser2in_ptr < (SERIAL_PORT_2_IN_BUFFER_SIZE-1)) ? (Ser2in_ptr + 1) : 0;
    SREG = sreg;  // Re-enable interrupts (if they were ever enabled)
  }

  return a;
}
