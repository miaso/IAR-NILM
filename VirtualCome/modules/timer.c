//-----------------------------------------------------------------------------
// Software that is described herein is for illustrative purposes only  
// which provides customers with programming information regarding the  
// products. This software is supplied "AS IS" without any warranties.  
// Embedded Systems Academy, Inc. assumes no responsibility or liability
// for the use of the software, conveys no license or title under any patent, 
// copyright, or mask work right to the product. NXP Semiconductors 
// reserves the right to make changes in the software without 
// notification. NXP Semiconductors also make no representation or 
// warranty that such application will be suitable for the specified 
// use without further testing or modification. 
//-----------------------------------------------------------------------------
//#include <nxp/iolpc2468.h>
//#include "board.h"
#include "type.h"
#include "irq.h"
#include "timer.h"


#include <intrinsics.h>




volatile unsigned long timer_counter = 0;

/******************************************************************************
** Function name:   Timer0Handler
**
** Descriptions:    Timer/Counter 0 interrupt handler
**        executes each 10ms @ 60 MHz CPU Clock
**
** parameters:      None
** Returned value:    None
**
******************************************************************************/
__irq __nested __arm void Timer0Handler (void)
{
  T0IR = 1;     /* clear interrupt flag */

  __enable_interrupt();       /* handles nested interrupt */

  timer_counter++;

  VICADDRESS = 0;   /* Acknowledge Interrupt */
}


__irq __nested __arm void Timer1Handler (void)
{
  T1IR = 1;     /* clear interrupt flag */

  __enable_interrupt();       /* handles nested interrupt */

  timer_counter++;

  VICADDRESS = 0;   /* Acknowledge Interrupt */
}

/*************************************************************************
 * Function Name: Timer1IntrHandler
 * Parameters: none
 *
 * Return: none
 *
 * Description: Timer 1 interrupt handler
 *
 *************************************************************************/
void Timer3IntrHandler (void)
{
  
  timer_counter++;
  // Toggle USB Link LED
  //USB_D_LINK_LED_FIO ^= USB_D_LINK_LED_MASK;
  // clear interrupt
  T3IR_bit.MR0INT = 1;
  VICADDRESS = 0;
}
/******************************************************************************
** Function name:   enable_timer
**
** Descriptions:    Enable timer
**
** parameters:      timer number: 0 or 1
** Returned value:    None
**
******************************************************************************/
void enable_timer( BYTE timer_num )
{
  if ( timer_num == 0 )
  {
    T0TCR = 1;
  }
  else
  {
    T1TCR = 1;
  }
  return;
}

/******************************************************************************
** Function name:   disable_timer
**
** Descriptions:    Disable timer
**
** parameters:      timer number: 0 or 1
** Returned value:    None
**
******************************************************************************/
void disable_timer( BYTE timer_num )
{
  if ( timer_num == 0 )
  {
    T0TCR = 0;
  }
  else
  {
    T1TCR = 0;
  }
  return;
}

/******************************************************************************
** Function name:   reset_timer
**
** Descriptions:    Reset timer
**
** parameters:      timer number: 0 or 1
** Returned value:    None
**
******************************************************************************/
void reset_timer( BYTE timer_num )
{
  DWORD regVal;

  if ( timer_num == 0 )
  {
    regVal = T0TCR;
    regVal |= 0x02;
    T0TCR = regVal;
  }
  else
  {
    regVal = T1TCR;
    regVal |= 0x02;
    T1TCR = regVal;
  }
  return;
}


#define TIMER3_TICK_PER_SEC   10
#define TIMER3_PCLK_OFFSET    46
/******************************************************************************
** Function name:   init_timer
**
** Descriptions:    Initialize timer, set timer interval, reset timer,
**            install timer interrupt handler
**
** parameters:      None
** Returned value:    true or false, if the interrupt handler can't be
**            installed, return false.
**
******************************************************************************/
DWORD init_timer ( DWORD TimerInterval )
{
//  T1TCR = 2;        /* timer disable and reset */
//  timer_counter = 0;
//  T1PR  = 0;
//  T1MR0 = TimerInterval;
//  T1MCR = 3;        /* Interrupt and Reset on MR0 */
//  T1TCR = 1;        /* timer enable and release reset*/
//  if ( install_irq( TIMER1_INT, (void *)Timer1Handler, HIGHEST_PRIORITY ) == FALSE )
//  {
//    return (FALSE);
//  }
//  else
//  {
//    return (TRUE);
//  }
  
    // Enable TIM0 clocks
  PCONP_bit.PCTIM3 = 1; // enable clock

  // Init Time3
  T3TCR_bit.CE = 0;     // counting  disable
  T3TCR_bit.CR = 1;     // set reset
  T3TCR_bit.CR = 0;     // release reset
  T3CTCR_bit.CTM = 0;   // Timer Mode: every rising PCLK edge
  T3MCR_bit.MR0I = 1;   // Enable Interrupt on MR0
  T3MCR_bit.MR0R = 1;   // Enable reset on MR0
  T3MCR_bit.MR0S = 0;   // Disable stop on MR0
  // set timer 1 period
  T3PR = 0;
  T3MR0 = SYS_GetFpclk(TIMER3_PCLK_OFFSET)/(TIMER3_TICK_PER_SEC);
  // init timer 1 interrupt
  T3IR_bit.MR0INT = 1;  // clear pending interrupt
  VIC_SetVectoredIRQ(Timer3IntrHandler,0,VIC_TIMER3);
  VICINTENABLE |= 1UL << VIC_TIMER3;
  T3TCR_bit.CE = 1;     // counting Enable
  __enable_interrupt();
   return (TRUE);
}

void
timer_set(struct timer *t, clock_time_t interval)
{
  t->interval = interval;
  t->start = clock_time();
}
/*---------------------------------------------------------------------------*/
/**
 * Reset the timer with the same interval.
 *
 * This function resets the timer with the same interval that was
 * given to the timer_set() function. The start point of the interval
 * is the exact time that the timer last expired. Therefore, this
 * function will cause the timer to be stable over time, unlike the
 * timer_rester() function.
 *
 * \param t A pointer to the timer.
 *
 * \sa timer_restart()
 */
void
timer_reset(struct timer *t)
{
  t->start += t->interval;
}
/*---------------------------------------------------------------------------*/
/**
 * Restart the timer from the current point in time
 *
 * This function restarts a timer with the same interval that was
 * given to the timer_set() function. The timer will start at the
 * current time.
 *
 * \note A periodic timer will drift if this function is used to reset
 * it. For preioric timers, use the timer_reset() function instead.
 *
 * \param t A pointer to the timer.
 *
 * \sa timer_reset()
 */
void
timer_restart(struct timer *t)
{
  t->start = clock_time();
}
/*---------------------------------------------------------------------------*/
/**
 * Check if a timer has expired.
 *
 * This function tests if a timer has expired and returns true or
 * false depending on its status.
 *
 * \param t A pointer to the timer
 *
 * \return Non-zero if the timer has expired, zero otherwise.
 *
 */
int
timer_expired(struct timer *t)
{
  return (clock_time_t)(clock_time() - t->start) >= (clock_time_t)t->interval;
}


/******************************************************************************
**                            End Of File
******************************************************************************/
