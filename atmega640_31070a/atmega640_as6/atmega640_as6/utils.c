/* 
   Platform and project independent utility functions
   (C) mtech.dk, 2009-10
*/

#include <stdio.h>

#define	SECS_DAY	86400UL  
#define	LEAPYEAR(year)	(!((year) % 4) && (((year) % 100) || !((year) % 400)))
#define	YEARSIZE(year)	(LEAPYEAR(year) ? 366 : 365)

// isleapyear = 0-1
// month=0-11
// return: how many days a month has
//
// We could do this if ram was no issue:
//uint8_t monthlen(uint8_t isleapyear,uint8_t month){
//const uint8_t mlen[2][12] = {
//		{ 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 },
//		{ 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 }
//	};
//	return(mlen[isleapyear][month]);
//}
//
uint8_t monthlen(uint8_t isleapyear, uint8_t month)
// returns number of days in month (0-11)
{
	if(month==1){
		return(28+isleapyear);
	}
	if (month>6){
		month--;
	}
	if (month%2==1){
		return(30);
	}
	return(31);
}

void NTPToStr(uint32_t NTPsecond, char buf[])
// converts an NTP time (upper 32 bits) into a human readable text string
{
  uint32_t D;
  uint16_t Y;
  uint8_t M,h,m,s;

	D = NTPsecond / SECS_DAY;
  NTPsecond = NTPsecond % SECS_DAY;

  // DMY:
  // (NTP uses seconds since 1900):
  Y = 1900;
	while (D >= YEARSIZE(Y)) {
		D -= YEARSIZE(Y);
		Y++;
	}

  M = 0;
	while (D >= monthlen(LEAPYEAR(Y),M)) {
		D -= monthlen(LEAPYEAR(Y),M);
		M++;
	}
  M++;
  D++;

  // hms:
	s = NTPsecond % 60UL;
	m = (NTPsecond % 3600UL) / 60;
	h = NTPsecond / 3600UL;

  sprintf(buf, "%02ld-%02d-%04d, %02d:%02d:%02d", D, M, Y, h, m, s);
}

unsigned short calcCRCFast(void* packet, unsigned short length)
// CRC-CCITT-16 0x1021 implementation, credits goes to:
// http://www.ccsinfo.com/forum/viewtopic.php?t=24977
// This version is basically as fast as the ones based on look-up tables
{
	int i;
	unsigned long crc, x;
	
	crc = 0x0ffff;

	// Loop through the data:
	for (i=0; i<length; i++)
	{
		x = crc >> 8;
		x ^= ((unsigned char*)packet)[i];
    x ^= x >> 4;
    crc = ((crc << 8) ^ (x << 12) ^ (x << 5) ^ x) & 0xFFFF;
	}

	return crc;
}


int int_floor(int x, int resolution)
{
  int Remainder;
  
  // The result of % is machine-dependant if x<0, so:
  if (x < 0) {
    Remainder = (-x) % resolution;
    if (Remainder)
      x = x + Remainder - resolution;
  }
  else {
    Remainder = x % resolution;
    if (Remainder)
      x = x - Remainder;
  }
  return x;
}

int int_ceil(int x, int resolution)
{
  int x2;
  
  x2 = int_floor(x, resolution);
  if (x > x2)
    x = x2 + resolution;
  else
    x = x2;
  return x;
}

// at 0.5 we get 1
int int_round(int x, int resolution)
{
	if ( ( x - int_floor(x, resolution) ) < (int_ceil(x, resolution) - x))
		return int_floor(x, resolution);
	else
		return int_ceil(x, resolution);
}
