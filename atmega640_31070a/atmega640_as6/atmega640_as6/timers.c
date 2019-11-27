/* 
  Timer functions
  Copyright mtech.dk, 2009-10
*/

// using timer1 (could be made configurable)
// also could configure timeout values in array (100 ms, 1 s, 10 s, ...)
// using timer3 for 1024 Hz RTC counter

#include "project.h"
#include <avr/io.h>
#include <avr/interrupt.h>


// 1024 Hz counter used to timestamp our data
// 1024 Hz used in order to easily (by shifting) converting into NTP time format
volatile uint32_t RTCcounter;

//ISR(SIG_OUTPUT_COMPARE3A) {
ISR(TIMER3_COMPA_vect) { 
  RTCcounter++; //
}

void SetupRTCtimer(void)
{
  uint8_t sreg = SREG;
  cli();

  TCNT3 = 0;                     // reset counter
  TCCR3B = (1<<WGM32)|(1<<CS30); // CTC mode (mode 4) and prescaler = 1
  OCR3A = 15624;                 // = 16e6/(1(prescaler)*1024(Hz))-1
  TIMSK3 = (1<<OCIE3A);          // enable timer interrupt

  SREG = sreg;  // Re-enable interrupts (if they were ever enabled)
}

uint32_t getRTCcounter(void)
// returns the current RTC counter.
{
  // We need to turn off interrupts, otherwise the value may be updated while
  // we copy it:
  uint8_t sreg = SREG;
  cli();

  uint32_t result = RTCcounter;

  SREG = sreg;  // Re-enable interrupts (if they were ever enabled)

  return result;
}


