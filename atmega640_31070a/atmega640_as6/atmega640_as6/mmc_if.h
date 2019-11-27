/* ***********************************************************************
**
**  Copyright (C) 2006  Jesper Hansen <jesper@redegg.net> 
**
**
**  Interface functions for MMC/SD cards
**
**  File mmc_if.h
**
**  Hacked by Michael Spiceland at http://tinkerish.com to support
**  writing as well.
**
*************************************************************************
**
**  This program is free software; you can redistribute it and/or
**  modify it under the terms of the GNU General Public License
**  as published by the Free Software Foundation; either version 2
**  of the License, or (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program; if not, write to the Free Software Foundation, 
**  Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
**
*************************************************************************/

#ifndef __MMC_IF_H__
#define __MMC_IF_H__

#include <inttypes.h>

uint8_t mmc_init(void);
// Initialize I/O ports for the MMC/SD interface and 
// send init commands to the MMC/SD card
// returns 0 on success, other values on error 

int mmc_readsector(uint32_t lba, uint8_t *buffer);
// Reads a single 512 byte sector from the MMC/SD card
// lba = Logical sectornumber to read
// buffer = Pointer to buffer for received data
// returns 0 on success, -1 on error

int mmc_writesector(uint32_t lba, uint8_t *buffer);
// Writes a single 512 byte sector to the MMC/SD card
// lba = Logical sectornumber to write
// buffer = Pointer to buffer to write to
// returns 0 on success, -1 on error

int mmc_readCSD(uint8_t *buffer);

void mmc_CS_init(void);

#endif
