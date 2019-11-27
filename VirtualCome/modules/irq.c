/*****************************************************************************
 *   irq.c: Interrupt handler C file for NXP LPC230x Family Microprocessors
 *
 *   Copyright(C) 2006, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2006.07.13  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#include <nxp/iolpc2468.h>
#include "type.h"
#include "irq.h"

/* Initialize the interrupt controller */
/******************************************************************************
** Function name:   init_VIC
**
** Descriptions:    Initialize VIC interrupt controller.
** parameters:      None
** Returned value:    None
**
******************************************************************************/
void init_VIC(void)
{
DWORD i = 0;
DWORD *vect_addr, *vect_cntl;

  /* initialize VIC*/
  VICINTENCLEAR = 0xffffffff;
  VICADDRESS = 0;
  VICINTSELECT = 0;

  /* set all the vector and vector control register to 0 */
  for ( i = 0; i < VIC_SIZE; i++ )
  {
    vect_addr = (DWORD *)(VIC_BASE_ADDR + VECT_ADDR_INDEX + i*4);
    vect_cntl = (DWORD *)(VIC_BASE_ADDR + VECT_CNTL_INDEX + i*4);
    *vect_addr = 0x0;
    *vect_cntl = 0xF;
  }
  return;
}

/******************************************************************************
** Function name:   install_irq
**
** Descriptions:    Install interrupt handler
** parameters:      Interrupt number, interrupt handler address,
**            interrupt priority
** Returned value:    true or false, return false if IntNum is out of range
**
******************************************************************************/
DWORD install_irq( DWORD IntNumber, void *HandlerAddr, DWORD Priority )
{
DWORD *vect_addr;
DWORD *vect_cntl;

  VICINTENCLEAR = 1 << IntNumber; /* Disable Interrupt */
  if ( IntNumber >= VIC_SIZE )
  {
    return ( FALSE );
  }
  else
  {
    /* find first un-assigned VIC address for the handler */
    vect_addr = (DWORD *)(VIC_BASE_ADDR + VECT_ADDR_INDEX + IntNumber*4);
    vect_cntl = (DWORD *)(VIC_BASE_ADDR + VECT_CNTL_INDEX + IntNumber*4);
    *vect_addr = (DWORD)HandlerAddr;  /* set interrupt vector */
    *vect_cntl = Priority;
    VICINTENABLE = 1 << IntNumber;  /* Enable Interrupt */
    return( TRUE );
  }
}

/******************************************************************************
**                            End Of File
******************************************************************************/
