/* 
  Project.h file for AVR640 code (supervisor functions)

  This file contains project wide defines and other settings
*/

// CPU lock frequency, previously defined at compile time, this code is not portable...
#define F_CPU 16000000UL

/*----------------------
   General SPI settings
  ----------------------*/
// port and pin definitions + speed
#define DD_MOSI    PINB2
#define DD_SCK     PINB1
#define DD_SS      PINB0
#define DDR_SPI    DDRB

#define SPI_PORT	PORTB
#define SPI_DDR		DDRB
#define SPI_PIN		PINB

#define SPI_SCK   1
#define SPI_MOSI  2
#define SPI_MISO  3

/*------------------
   CS-chip settings
  ------------------*/
// port and pin definitions
#define CS_RESET_PORT PORTB
#define CS_RESET_DDR  DDRB
#define CS_RESET  4

#define CS_CS_PORT PORTB
#define CS_CS  0

/*-----------------
   SDcard settings
  -----------------*/
// port and pin definitions
#define MMC_CS_PORT	PORTB
#define MMC_CS_DIR	DDRB

#define MMC_SCK		SPI_SCK
#define MMC_MOSI	SPI_MOSI
#define MMC_MISO	SPI_MISO
#define MMC_CS		6

/*----------------------
   Serial port settings
  ----------------------*/
// port to use for stdout:
// this allows you to use printf(), but currently using polled i/o,
// so use sparsely and with care!
#define SERIAL0 1
#define SERIAL_STDOUT_PORT        0
#define SERIAL_STDOUT_BAUD_RATE   115200


#define SERIAL_PORT_0_IN_BUFFER_SIZE   32
#define SERIAL_PORT_0_OUT_BUFFER_SIZE  32
// #define SERIAL_PORT_2_OUT_BUFFER_SIZE 640

  // GSM serial connection:
#define GSMser 2

void Serial2Int(char data);
void Serial2Empty();

#define Serial3Int Serial2Int


/*----------------------
   Other I/O settings
  ----------------------*/
#define RELAY_PORT         PORTC
#define RELAY_DDR    	   DDRC
#define RELAY_PIN          3

#define STATUS_LED_PORT	   PORTA
#define STATUS_LED_PIN	   7

