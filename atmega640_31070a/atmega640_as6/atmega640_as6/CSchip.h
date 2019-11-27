/* 
   CS-chip related functions
   (C) mtech.dk, 2009-10
*/

#ifndef _CSchip_h_
#define _CSchip_h_

#include "project.h"

#define CSresetON()   CS_RESET_PORT &= ~(1<<CS_RESET_PIN)
#define CSresetOFF()  CS_RESET_PORT |= (1<<CS_RESET_PIN)

void CSchipInit();
void CScmd(uint8_t cmd);
void CSwrite(uint8_t reg, uint8_t high, uint8_t mid, uint8_t low);
void CSwrite24(uint8_t reg, uint32_t value);
uint32_t CSread(uint8_t reg);

#endif
