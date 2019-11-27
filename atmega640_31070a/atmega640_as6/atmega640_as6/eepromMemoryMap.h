/* 
   DFR project EEPROM memory map
   (C) mtech.dk, 2009-10
*/

#ifndef _EEPROMMEMORYMAP_H_
#define _EEPROMMEMORYMAP_H_

// On older AVR's the EEAR register is zeroed on reset. This means
// that there is a high risk of EEPROM byte 0 being written some
// random value if writing to an EEPROM takes place during a power
// failure. This has been fixed on the 164P/640 series (among others).

// Define the EEPROM memory allocation here:

// cell 0 not used (ok to use on 640/164P, but not on some older devices during power failure
// * = Must be set on initial flash load
//#define EEPROM_FIRMWARE_REVISION           5 // *

//#include "eeprom.h"

#define EEPROM_SD_ID_READ                 10 // initialized when new SD card inserted
#define EEPROM_SD_ID_WRITE                11 // initialized when new SD card inserted
// (the above two differs only when reading has not yet reached the end of the card, but writing has)
#define EEPROM_SD_SECTOR_TRANSMIT_NEXT    12 // " (32 bits) search for first unused sector also starts here
#define EEPROM_CONFIG_ID				  0x10

#define EEPROM_BLACKOUT_WRITE_NEXT        20 // " next sector to write blackout data to (900 offset)
#define EEPROM_BLACKOUT_SEND_NEXT         21 // " next sector to transmit (900 offset); only transmit if different from EEPROM_BLACKOUT_WRITE_NEXT

#define EEPROM_VF_KF					  0x30	// Depricated in v.8
#define EEPROM_VF_DELTA_MAX				  0x32  // Depricated in v.8
#define EEPROM_VF_OFFSET_MAX			  0x34
#define EEPROM_VF_OFFSET_MIN			  0x36
#define EEPROM_VF_FREQ_OFFSET_MAX		  0x38
#define EEPROM_VF_FREQ_OFFSET_MIN		  0x3A

#define EEPROM_DEVI_KF					  0x40  // Depricated in v.8
#define EEPROM_DEVI_DELTA_MAX			  0x42  // Depricated in v.8
#define EEPROM_DEVI_OFFSET_MAX			  0x44
#define EEPROM_DEVI_OFFSET_MIN			  0x46
#define EEPROM_DEVI_FREQ_OFFSET_MAX		  0x48
#define EEPROM_DEVI_FREQ_OFFSET_MIN		  0x4A
#define EEPROM_DEVI_IDLE_TIME_SEC		  0x4C

#define EEPROM_MSR_FOFF					  0x50
#define EEPROM_MSR_FREC					  0x52
#define EEPROM_MSR_T_RDEL				  0x54
#define EEPROM_MSR_T_MIND				  0x56
#define EEPROM_MSR_T_MAXD				  0x58
#define EEPROM_MSR_T_MINC				  0x5A
#define EEPROM_MSR_MEAS_SMOOTH			  0x5C

#define EEPROM_F_HIGHRES_LOW			  0x60		// Low F Trigger to start high resolution sampling (in mHz of power system, ie. 49900)
#define EEPROM_LOWRES_S					  0x62		// Frequency of low resolution samples (in sec)
#define EEPROM_SERVERPOLL_S				  0x64		// Frequency of uploading data to the server and downloading SNTP and config (in sec)
#define EEPROM_HIGHRES_D_S				  0x66		// Duration of high resolution sampling (in sec)
#define EEPROM_HIGHRES_SKIP				  0x68		// Highres sampling frequency N*250ms.
													// When =1 samples taken every 250ms, =2 500ms, =3 750ms, =4 1000ms, =8 2sec...
#define EEPROM_F_HIGHRES_HIGH			  0x6A		// High F Trigger to start high resolution sampling (in mHz of power system, ie. 50100)

// Section for communication parameters.  Strings are kept is a seperate section
#define EEPROM_HOST_PORT				  0x70


//String section.  All strings have the save fixed (max) size of MAXSTR = 32 bytes
#define MAXSTR 32

#define EEPROM_S_SERVER					  0x80
#define EEPROM_S_CONFIG_PATH			  0xA0
#define EEPROM_S_PUT_PATH	   		  	  0xC0
#define EEPROM_S_FW_PATH				  0xE0

// EEPROM values that are presearved between upgrades.  All values 16bit unless otherwise noted.
#define EEPROM_SERIAL                     0x400 
#define EEPROM_MODEL                      0x402
#define EEPROM_FIRMWARE_FINGERPRINT	      0x404 // This is the checksum of the checksum sector.
#define EEPROM_SYSTEM_STATE               0x406 // FF = virgin; 1 = operational; 0 = new firmware available; 2 = new firmware installed
#define EEPROM_SIM_PIN					  0x408	// 4 bytes...
 
#define EEPROM_CS_CALIBRATION_DATA        0x410 // byte 0x410 indicates if calibration data is available (FFh = no calibration data available)
#define EEPROM_CS_CAL_CURRENT_AC_OFFSET   0x414 // 24bit value, stored in 32 bits
#define EEPROM_CS_CAL_VOLTAGE_GAIN        0x418
#define EEPROM_CS_CAL_ACTIVE_PWR_OFFSET   0x41C
#define EEPROM_CS_CAL_CURRENT_GAIN		  0x420



// * = Must be set on initial flash load
// all other unused

#endif
