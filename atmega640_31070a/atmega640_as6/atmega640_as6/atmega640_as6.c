/*
   AVR640 code for 31070 project
*/

#include "project.h"

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <string.h>

#include "serial.h"
#include "SPI.h"
#include "timers.h"
#include "CSchip.h"
#include "utils.h"
#include "eeprom.h"
#include "eepromMemoryMap.h"

volatile int newSerialData = 0;

void halt()
// halts execution (currently simply busy loop - could change to some low-power shutdown,
// but only occurs on rare errors (SD-card not found etc)
{
  printf("\r\n!!! EXECUTION STOPPED !!!");
  while (1) {}    
}

void setupAVR164()
{
  // Enable external int 7 (AVR164 asking for a timestamp)
  EICRB |= (1 << ISC70);
  EICRB &= ~(1 << ISC71);
  EIMSK |= (1 << INT7);

}


void ms_sleep(uint32_t ms)
{
  uint32_t stopTime;
  stopTime = getRTCcounter() + ms;

  // stopTime has overflowed, wait until current time overflows too:
  if (stopTime < ms)
  {
    do {}
    while (getRTCcounter() > stopTime);
  }

  do {}
  while (stopTime > getRTCcounter());
}

void sleep(uint8_t seconds)
{
  ms_sleep(seconds*1024);
}

// Uncomment this macro to disable reading calibration params from the EEPROM
// #define EEPROMread3Bytes(x) 0xFFFFFF

void setupCS()
{

  // initialize CS chip:
  CSwrite(0,0,0,4);  // set K=4 because input CLK is 16 MHz, all others default

  CSwrite(31,0,0,0); // goto page 0

  CSwrite(18,0,0,96); //Set IHPF, VHPF and IIR filters. (Mainly to remove DC-content, no need of DC-calibration). Disable AFC
  CSwrite(1,0,0,0); //Set current DC offset zero (should be set zero, when IHPF active)
  CSwrite(3,0,0,0); //Set voltage DC offset zero (should be set zero, when VHPF active)
  CSwrite24(17,0);    //Voltage AC Offset value
  CSwrite(5, 0, 0x0F, 0xA0); // n=4000


  // Values are read from the EEPROM, if not initialized, take a guess...
  // Some clever programmers make functions/macros out of this kind of repetative tasks....
  if (EEPROMread3Bytes(EEPROM_CS_CAL_CURRENT_AC_OFFSET) == 0xFFFFFF)
    CSwrite24(16,1000); //Current AC Offset value
  else
    CSwrite24(16,EEPROMread3Bytes(EEPROM_CS_CAL_CURRENT_AC_OFFSET));
  
  
  if (EEPROMread3Bytes(EEPROM_CS_CAL_VOLTAGE_GAIN) == 0xFFFFFF)
    CSwrite24(4,4019600); //Voltage Gain (Last Calibration)
  else
	CSwrite24(4,EEPROMread3Bytes(EEPROM_CS_CAL_VOLTAGE_GAIN));

  if(EEPROMread3Bytes(EEPROM_CS_CAL_ACTIVE_PWR_OFFSET) == 0xFFFFFF)
    CSwrite24(14,604); //Active Power Offset Calibration
  else
  	CSwrite24(14, EEPROMread3Bytes(EEPROM_CS_CAL_ACTIVE_PWR_OFFSET));

  if (EEPROMread3Bytes(EEPROM_CS_CAL_CURRENT_GAIN) == 0xFFFFFF)
    CSwrite24(2,4600000); //Current Gain
  else
    CSwrite24(2,EEPROMread3Bytes(EEPROM_CS_CAL_CURRENT_GAIN));

  printf("CS calibration data: Current Gain: %ld   Current AC offset: %ld    Voltage Gain: %ld    Power offset: %ld Epsilon:%ld\r\n", CSread(2), CSread(16), CSread(4), CSread(14), CSread(13));

  CScmd(0xE8);  // start continuous conversions 
}





void UART_SendByte(uint8_t data){

/* Wait for empty transmit buffer */
	while ( !( UCSR0A & (1<<UDRE0)) );
/* Put data into buffer, sends the data */
		UDR0 = data;
}

void UARTPutStr(char* s,int length) {
	//char c = 0;
	for (int i=0; i<=length; i++) {
		UART_SendByte(*s);
		s++;
	}
	//UART_SendByte('\n');
//	UART_SendByte('\r');
}


	char buffToSend[20];


void processSample()
{

	uint32_t I_rms = CSread(11);
	uint32_t V_rms = CSread(12);
	uint32_t P_Active = CSread(10);
	uint32_t P_Reactive = CSread(20);

	long signed int Pact;
	
	// If the power measurement has gone negative (I don't know why it does this), convert back to positive value
	if (P_Active > 0x800000)
		Pact = 0x1000000 - P_Active;
	else 
		Pact = P_Active;
	
	// Prints in Decimal, with labels, somewhat human readable
	// printf("AT %08lu Vrms; %08lu Irms; %08lu P\r\n", V_rms, I_rms, P_Active);
		// Prints in HEX without labels
	
		buffToSend[0]=0x01;
		buffToSend[1]=0x02;
		buffToSend[2] = V_rms >> 24;
		buffToSend[3] = V_rms >> 16;
		buffToSend[4] = V_rms >>  8;
		buffToSend[5]=  V_rms;
				buffToSend[6] = I_rms >> 24;
				buffToSend[7] = I_rms >> 16;
				buffToSend[8] = I_rms >>  8;
				buffToSend[9]=  I_rms;
							buffToSend[10] = Pact >> 24;
							buffToSend[11] = Pact >> 16;
							buffToSend[12] = Pact >>  8;
							buffToSend[13]=  Pact;
														buffToSend[14] = P_Reactive >> 24;
														buffToSend[15] = P_Reactive >> 16;
														buffToSend[16] = P_Reactive >>  8;
														buffToSend[17]=  P_Reactive;

														buffToSend[18]=  0x04;
														buffToSend[19]=  0x05;
														UARTPutStr(buffToSend,19);
														//buffToSend[20]=  '\r';
													//	buffToSend[21]=  0xff;
													//	USART_SendByte(buffToSend[21]);
													//	uart_putchar();

	//	printf(buffToSend,20);
	// Prints in HEX without labels
// 	printf("AT 0x%08lx; 0x%08lx; 0x%08lx; 0x%08lx\r\n", V_rms, I_rms, Pact,P_Reactive); 
}


int main(void){
  DDRA = 0x83;
  PORTA = 0x83;
  SetupRTCtimer();
  SerSetupSTDOUT();
  CSchipInit();
  SPIinit();
 
  RELAY_PORT |= (1<<RELAY_PIN);
  STATUS_LED_PORT &= ~(1 << STATUS_LED_PIN);

  sei();      // ready to roll

  printf("\r\n\r\nFrmwareinitializing v1.0.0.0.4!!!!! \r\n");

  setupCS();

  for(;;) {
  	processSample();

	if (newSerialData)
	{
		printf("Ping!\r\n");
		newSerialData = 0;
	}

	sleep(1);
  }
}
