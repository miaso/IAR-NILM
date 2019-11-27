/* 
   Platform and project independent utility functions
   (C) mtech.dk, 2009-10
*/

#ifndef _utils_h_
#define _utils_h_

void NTPToStr(uint32_t NTPsecond, char buf[]);

unsigned short calcCRCFast(void* packet, unsigned short length);

// Functions for rounding 'x' down (floor) or up (ceil) in increments of 'resolution'
int int_floor(int x, int resolution);
int int_ceil(int x, int resolution);
int int_round(int x, int resolution);

#endif
