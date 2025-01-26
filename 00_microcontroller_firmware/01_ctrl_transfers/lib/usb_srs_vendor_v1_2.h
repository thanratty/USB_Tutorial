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
#define CBI(adr,bitnum)    (adr &= ~(1<<(bitnum)))    /* Clear bit # in SF-register (ASM) */
#define SBI(adr,bitnum)    (adr |=  (1<<(bitnum)))    /* Set bit # in SF-register (ASM) */



// Prescaler for USB PLL with a 16 MHz crystal
//
#define PLLPRE 0x10




void usb_init_device(void);
void usb_init_endpoint(uint8_t num, uint8_t type, uint8_t direction, uint8_t size, uint8_t bank);

/* Functions for enumeration */
void usb_ep0_setup(void);
void usb_send_descriptor(const void* ptr, uint8_t desc_bytes);


#endif
