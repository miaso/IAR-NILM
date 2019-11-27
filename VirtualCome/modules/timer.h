/*****************************************************************************
 *   timer.h:  Header file for NXP LPC23xx/24xx Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.07.13  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#ifndef __TIMER_H
#define __TIMER_H
//#include "type.h"
#include "clock.h"

struct timer {
  clock_time_t start;
  clock_time_t interval;
};

void timer_set(struct timer *t, clock_time_t interval);
void timer_reset(struct timer *t);
void timer_restart(struct timer *t);
int timer_expired(struct timer *t);

#define Fcclk 72000000
#define Fpclk (Fcclk / 4)
#define TIME_INTERVAL Fpclk/100 - 1

extern DWORD init_timer( DWORD timerInterval );
extern void enable_timer( BYTE timer_num );
extern void disable_timer( BYTE timer_num );
extern void reset_timer( BYTE timer_num );
extern volatile unsigned long timer_counter;

#endif /* end __TIMER_H */
/*****************************************************************************
**                            End Of File
******************************************************************************/
