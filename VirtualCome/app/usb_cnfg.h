/*************************************************************************
 *
 *    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2007
 *
 *    File name   : usb_cfg.h
 *    Description : Define main module
 *
 *    History :
 *    1. Date        : January 7, 2007
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *    $Revision: 28 $
**************************************************************************/

#include "includes.h"

#ifndef __USB_CNFG_H
#define __USB_CNFG_H

/* USB High Speed support*/
#define USB_HIGH_SPEED                  0

/* USB interrupt priority */
#define USB_INTR_PRIORITY               0
#define USB_DEV_PRIORITY                0   // 1 - Frame is high priority,
                                            // 2 - EPs are high priority
/* Endpoint priority setting*/
#define USB_EP_PRIORITY                 0x00000000

/* USB Events */
#define USB_SOF_EVENT                   0
#define USB_ERROR_EVENT                 1   // for debug
#define USB_SOF_FRAME_NUMB              0   // disable frame number

//DMA Settings
#define USB_DMA_DD_MAX_NUMB             0   // number of DMA descriptors
#define USB_DMA_ID_MAX_NUMB             0   // number of Isochronous DMA descriptors
#define DMA_INT_ENABLE_MASK             0   // DMA interrupt enable (End of Transfer,
                                            // New DD request, System error interrupt)
/* USB PORT settings */
#define USB_PORT_SEL                    1

/* USB Clock settings */
#define USB_CLK_DIV                     6

/* Device power atrb  */
#define USB_SELF_POWERED                0
#define USB_REMOTE_WAKEUP               0

/* Max Interfaces number */
#define USB_MAX_INTERFACE               1

/* Endpoint definitions */
#define Ep0MaxSize                      8

#define ReportEp                        ENP1_IN
#define ReportEpMaxSize                 16

#define CommOutEp                       ENP2_OUT
#define CommOutEpMaxSize                64

#define CommInEp                        ENP2_IN
#define CommInEpMaxSize                 64

/* Class defenitions*/
#define CDC_DEVICE_SUPPORT_LINE_CODING  1
#define CDC_DEVICE_SUPPORT_LINE_STATE   1

#define CDC_DEVICE_SUPPORT_BREAK        1
#define CDC_BRK_TIMER_INTR_PRI          3

#define CDC_DATA_RATE                   CBR_9600
#define CDC_DATA_BITS                   8
#define CDC_PARITY                      NOPARITY
#define CDC_STOP_BITS                   ONESTOPBIT

#define CDC_LINE_DTR                    1
#define CDC_LINE_RTS                    1

/* Other defenitions */

#endif //__USB_CNFG_H
