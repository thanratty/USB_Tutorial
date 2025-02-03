/* Copyright (c)   2022 Johannes 4Linux  johannes[at]gnu-linux[dot]rocks
                   2013 Guy WEILER       weigu[at]weigu[dot]lu

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.

   Licence: GPL-3        */

#ifndef _USB_SRS_H_
#define _USB_SRS_H_

#include <stdint.h>
#include <avr/io.h>

#include "usb-defs.h" 
#include "usb-types.h"



// ATMega32u4 prescaler for USB PLL with a 16 MHz crystal
//
#define     PLLPRE          0x10


// Prepare some handy bit masks from bit numbers
//
#define     UVREG_m         (1<<UVREGE)
#define     USBE_m          (1<<USBE)
#define     FRZCLK_m        (1<<FRZCLK)
#define     OTGPADE_m       (1<<OTGPADE)
#define     EORSTI_m        (1<<EORSTI)
#define     PLOCK_m         (1<<PLOCK)
#define     RXOUTI_m        (1<<RXOUTI)
#define     TXINI_m         (1<<TXINI)
#define     UVREGE_m        (1<<UVREGE)


// Macros to make program easier to read
//
#define CLEAR_BIT(adr,bitnum)       (adr &= ~(1<<(bitnum)))    // Clear bit # in SF-register (ASM)
#define SET_BIT(adr,bitnum)         (adr |=  (1<<(bitnum)))    // Set bit # in SF-register (ASM)


// Handy macros for command handling
//
#define USB_CMD_VALUE(bRequest,bmRequestType)     ((bRequest << 8) | bmRequestType)

#define USB_CMD(dir, rcpt, type, cmd) \
    ((USB_CMD_##cmd << 8) | (USB_##dir##_TRANSFER << 7) | \
     (USB_##type##_REQUEST << 5) | (USB_##rcpt##_RECIPIENT << 0))






void usb_init_device(void);


#endif
