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

#include <stdint.h>
#include <avr/pgmspace.h> 
#include <avr/interrupt.h> 

#include "usb_srs_vendor_v1_2.h"
#include "usb_defs.h"

uint8_t display = 0;



/**
 * @brief USB General Interrupt (S253)
 */ 
ISR(USB_GEN_vect)
{
    /* Check for 'end of reset' interrupt */
    if (UDINT & (1<<EORSTI))
    {
        /* Clear 'end of reset' interrupt */
        CBI(UDINT,EORSTI);
        /* Init Endpoint 0 */
        usb_init_endpoint(0, Ep0_ty, Ep0_di, Ep0_si, Ep0_ba);    
        /* Enable RX IRQ */  
        SBI(UEIENX,RXSTPE);
    }
}



/**
 * @brief Endpoint/Pipe Communication Interrupt Service Routine (S254)
 */    
ISR(USB_COM_vect)
{
    /* EP is Bit Set in UEINT, e.g. 2 = (1<<1) -> EP1 */
    switch (UEINT)
    {
        /* Check if Setup package arrived, if so call usb_ep0_setup */
        case 1: 
	        /* Select EP 0 */
            UENUM = 0;
            if (UEINTX & (1<<RXSTPI))
                usb_ep0_setup();
            break;

        /* Expand here for other endpoints ... */

        default: 
                break;
    }
}



/**
 * @brief USB-Aktivierung (Full-Speed 12Mbit/s) und Interrupts erlauben
 */
void usb_init_device(void)
{
    UHWCON = (1<<UVREGE);                               /* Enable USB Pad regulator */
    USBCON = ((1<<USBE) | (1<<FRZCLK) | (1<<OTGPADE));  /* Enable USB power */

    USBCON &= ~(1<<FRZCLK);                             /* Toggle FRZCLK to get WAKEUP IRQ */
    USBCON |= (1<<FRZCLK);

    /* Configure the PLL */
    PLLCSR = PLLPRE;                    /* PLL Prescaler -> Set PINDIV=1 for 16MHz */
    PLLCSR |= (1<<PLLE);                /* Enable PLL */
    while (!(PLLCSR &(1<<PLOCK)));      /* Wait for PLL to lock */

    USBCON &= ~(1<<FRZCLK); /* Leave Power Saving mode */
    UDCON = 0;              /* Attach device */

    UDIEN = (1<<EORSTE);    /* Enable Reset IRQ */
    sei();                  /* Global IRQ enable */
}



/**
 * @brief Activate Endpoint
 *
 * @param ep_number  EPNumber: 0...6
 * @param type:      0 = Control, 1 = Isochron, 2 = Bulk 3 = Interrupt
 * @param direction: 0 = OUT, 1 = IN 
 * @param size:      0 = 8 Bytes, 1 = 16 Bytes, 2 = 32 Bytes, 3 = 64 Bytes, 
 *                   4 = 128 Bytes (only EP1), 5 = 256 Bytes (EP1), others reserved
 * @param bank:      0 = 1 Bank, 1 = 2 banks, others reserved
 */
void usb_init_endpoint( uint8_t number_ep, 
                        uint8_t type, 
                        uint8_t direction, 
                        uint8_t size, 
                        uint8_t bank )
{
    UENUM = number_ep;  /* Select EP */
    SBI(UECONX,EPEN);   /* Enable EP */

    UECFG0X = ((type << 6) | (direction));  /* Set type and direction */
    UECFG1X = ((size << 4) | (bank << 2));  /* Set size and num of banks */

    SBI(UECFG1X,ALLOC);                     /* Allocate EP memory */
}



/**
 * @brief Function handles setup/control transmissions to EP0
 */
void usb_ep0_setup(void)
{
    uint8_t bmRequestType;
    uint8_t bRequest;
    uint8_t wValue_l,  wValue_h;
    uint8_t wIndex_l,  wIndex_h;
    uint8_t wLength_l, wLength_h;
    uint8_t des_bytes;
    uint16_t length;
    int i;

    /* Let's declare the USB descriptors in flash memory */

    /*** Device Descriptor ***/
    static const uint8_t PROGMEM dev_des[] = {
        18,                     /* bLength = 18 (0x12), descriptor length in bytest */  
        0x01,                   /* bDescriptorType = 0x01, Descriptor ID = 1 -> Device descriptor */
        0x10,0x01,              /* bcdUSB = 0x0200, USB_Spec2_0 */
        0x00,                   /* bDeviceClass = 0x00, class code defined on interface level */
        0x00,                   /* bDeviceSubClass = 0x00 */
        0x00,                   /* bDeviceProtocoll = 0x00 */
        Ep0_fs,                 /* bMaxPacketSize0 = EP0FS, max. package size EP0 (here 8 B) */
        MY_VID_LO,MY_VID_HI,    /* idVendor */
        MY_PID_LO,MY_PID_HI,    /* idProduct */
        0x01,0x00,              /* bcdDevice = 0x0100, Release number device */
        Manu_i,                 /* iManufacturer = Index for string-descriptor manufacturer */
        Prod_i,                 /* iProduct = Index for string-descriptor product */
        Seri_i,                 /* iSerialNumber = Index for string-descriptor serial number */
        0x01                    /* bNumConfigurations = 1, Number of available configurations */
    };

    /*** Configurations Descriptor ***/
    static const uint8_t PROGMEM conf_des[] =  {
        9,                      /* bLength = 0x09, descriptor length in bytes */
        0x02,                   /* bDescriptorType = 0x02, Descriptor ID = 2 -> Configuration descriptor */
        low(wTotalLength),high(wTotalLength),  /* wTotalLength, length of  Configuration */
        0x01,                   /* bNumInterfaces = 1 */
        0x01,                   /* bConfigurationValue = 1, must not be 0 */
        0,                      /* iConfiguration = 0, index for str.-descriptor configuration */
        0x80,                   /* bmAttributes = 0x80,bus-powered, no remote wakeup Bit 7=1 */ 
        250,                    /* MaxPower = 250(dezimal), means 250*2mA = 500mA */ 
        /*** Interface Descriptor ***/
        9,                      /* bLength = 0x09, length of descriptor in bytes */
        0x04,                   /* bDescriptorType = 0x04, descriptor ID = 4 -> Interface descriptor*/
        0,                      /* bInterfaceNumber = 0; */
        0,                      /* bAlternateSetting = 0; */
        Nr_eps,                 /* bNumEndpoints = USB_Endpoints; */
        0xFF,                   /* bInterfaceClass = 0xFF, classcode: custome (0xFF) */
        0xFF,                   /* bInterfaceSubClass = 0xFF, subclasscode: custome (0xFF) */
        0xFF,                   /* bInterfaceProtocol = 0xFF, protocoll code: custome (0xFF) */
        Intf_i,                 /* iInterface = 0, Index for string descriptor interface */
        /* Endpoint descriptors */
    };                    

    /*** Language Descriptor ***/      
    static const uint8_t PROGMEM lang_des[] = {
        4,        /* bLength = 0x04, length of descriptor in bytes */
        0x03,     /* bDescriptorType = 0x03, Descriptor ID = 3 -> String descriptor */
        0x09,0x04 /* wLANGID[0] = 0x0409 = English USA (Supported Lang. Code 0) */
    };

    /*** Manufacturer Descriptor ***/          
    static const uint8_t PROGMEM manu_des[] = {
        24,       /* bLength = 0x04, length of descriptor in bytes */
        0x03,     /* bDescriptorType = 0x03, Descriptor ID = 3 -> String descriptor */
                          /* bString = Unicode Encoded String (16 Bit!) */
        'B',0,'r',0,'i',0,'g',0,'h',0,'t',0,'l',0,'i',0, 'g', 0, 'h', 0, 't', 0,
    };        

    /*** Product Descriptor ***/       
    static const uint8_t PROGMEM prod_des[] = {
        22,         /* bLength = 0x22, length of descriptor in bytes */
        0x03,       /* bDescriptorType = 0x03, Descriptor ID = 3 -> String descriptor */
                    /* bString = Unicode Encoded String (16 Bit!)*/
        'T',0,'e',0,'s',0,'t',0,'d',0,'e',0,'v',0,'i',0,'c',0,'e',0
    };        
    /*** Serial Number Descriptor ***/      
    /*** Serial Descriptor ***/      
    static const uint8_t PROGMEM seri_des[] = {
        10,         /* bLength = 0x12, length of descriptor in bytes */
        0x03,       /* bDescriptorType = 0x03, Descriptor ID = 3 -> String descriptor */
                    /* bString = Unicode Encoded String (16 Bit!) */
        '1',0,'2',0,'3',0,'4',0,
    };


    /* Add any EP descriptors here... */

    bmRequestType = UEDATX; 
    bRequest  = UEDATX; 
    wValue_l  = UEDATX; 
    wValue_h  = UEDATX; 
    wIndex_l  = UEDATX; 
    wIndex_h  = UEDATX;
    wLength_l = UEDATX; 
    wLength_h = UEDATX; 
    length = wLength_l + (wLength_h << 8);

    CBI(UEINTX, RXSTPI); /* ACK received Setup package */
                    
/* bmRequestType bit definitions

Bit     Field
7       Data Phase Transfer Direction
            0 = Host to Device
            1 = Device to Host
6:5     Type
            00 = Standard
            01 = Class
            10 = Vendor
            11 = Reserved
4:0     Recipient
            0 = Device
            1 = Interface
            2 = Endpoint
            3 = Other
            4..31 = Reserved
*/


    if ((bmRequestType & 0x60) == 0)
    {
        /* Type = Standard Device Request */ 
        switch (bRequest)
        {
            case GET_STATUS:      /* GET_STATUS 3 Phases */
                UEDATX = 0; /* Send back 16 Bit for status -> Not self powered, no wakeup, not halted */
                UEDATX = 0;
                CBI(UEINTX,TXINI);                  /* Send data (ACK) and clear FIFO */
                while (!(UEINTX & (1 << RXOUTI)));  /* Wait for ZLP from host */
                CBI(UEINTX, RXOUTI);                /* Clear flag */
                break;

            case SET_ADDRESS: /* SET_ADDRESS 2 Phasen (no data phase ) */
                UDADDR = (wValue_l & 0x7F); /* Save address at UADD  (ADDEN = 0) */
                CBI(UEINTX,TXINI);                  /* Send OUT package (ZLP) and clear bank */
                while (!(UEINTX & (1<<TXINI)));     /* Wait for bank to be cleared */
                SBI(UDADDR, ADDEN);                 /* Enable address */
                break;

            case GET_DESCRIPTOR:
            {
                /* GET_DESCRIPTOR 3 Phase Transfer */
                switch (wValue_h)
                {
                    case DEVICE_DESCRIPTOR:
                        des_bytes = pgm_read_byte(&dev_des[0]);	      
                        usb_send_descriptor(dev_des, des_bytes);         
                        break;

                    case CONFIGURATION_DESCRIPTOR:
                        des_bytes = wLength_l;
                        if (wLength_h || (wLength_l > wTotalLength) || (wLength_l == 0)) 
                            des_bytes = wTotalLength;
                        usb_send_descriptor(conf_des, des_bytes);
                        break;

                    case STRING_DESCRIPTOR:
                        switch (wValue_l)
                        {
                            case Lang_i:
                                des_bytes = pgm_read_byte(&lang_des[0]);
                                usb_send_descriptor(lang_des, des_bytes);
                                break;
                            case Manu_i:
                                des_bytes = pgm_read_byte(&manu_des[0]);
                                usb_send_descriptor(manu_des, des_bytes);
                                break;
                            case Prod_i:
                                des_bytes = pgm_read_byte(&prod_des[0]);
                                usb_send_descriptor(prod_des, des_bytes);
                                break;
                            case Seri_i:
                                des_bytes = pgm_read_byte(&seri_des[0]);
                                usb_send_descriptor(seri_des, des_bytes);
                                break;              
                            default:
                                break;
                        }
                        break;

                    default:
                        break; 
                }      
            }
            break;

        case SET_CONFIGURATION: /* 2 Phase, no data phases */
            /* Reset any endpoints here... */

            UENUM = 0;                      /* Select EP0 */
            CBI(UEINTX, TXINI);             /* Send ZLP */
            while (!(UEINTX & (1<<TXINI))); /* Wait until ZLP is send and bank is cleared */
            break;

        default: 
            SBI(UECONX,STALLRQ);
               break;
        }
    }
    else if((bmRequestType & 0x60) == 0x40)     // Vendor request to device
    {
        switch(bRequest)
        {
            case 0x1:
                // Get new value from host for 7 segment display
                display = wValue_l;
                CBI(UEINTX,TXINI);              /* Send OUT package (ZLP) and clear bank */
                while (!(UEINTX & (1<<TXINI))); /* wait for bank to be cleared */
                break;

            case 0x2:
                // Return current displayed value to host
                UEDATX = display;
                for(i=1 ; i<length ; i++)
                {
                    UEDATX = i;
                    if ((i % Ep0_fs) == 0)
                    {
                        /* FIFO is full, send data */
                        CBI(UEINTX,TXINI);                                  /* Send IN package */
                        while (!(UEINTX & ((1<<RXOUTI) | (1<<TXINI))));     /* Wait for ACK from host */
                    }
                }
                if (!(UEINTX & (1 << RXOUTI)))
                {
                    CBI(UEINTX,TXINI);                                      /* Send IN package */
                    while (!(UEINTX & (1 << RXOUTI)));                      /* Wait for (ZLP) ACK from host */
                }   

                CBI(UEINTX,RXOUTI);     /* Handshake to acknowledge IRQ */

            default:
                SBI(UECONX,STALLRQ);
                break;
        }
    }
    else
        SBI(UECONX,STALLRQ);    /* Not a standard request, STALL response */
}



/*! \brief Sende Deskriptor zum PC (22.14 IN-EP management S 275)
    Es werden nur so viele Bytes gesendet wie angefragt. Falls PC in dieser 
    Phase (bei Control Transaktion) abbrechen moechte, so sendet er ein ZLP 
    Paket (2.14.1.1 S 276). 
    Fuelle FIFO (gegebenfalls mehrmals) und sende Daten.
     */
/**
 * @brief Send a descriptor to the host
 * Only as many bytes as requested will be send. 
 * This will fill the FIFO (sometimes mutlible times) and sends the data
 *
 * @param descriptor	descriptor to send
 * @param desc_bytes    size of descriptors
 */
void usb_send_descriptor(const uint8_t* descriptor, uint8_t desc_bytes)
{
    for (uint16_t i=1 ; i<=desc_bytes ; i++)
    {
        /* Abort, if host wants to abort */
        if (UEINTX & (1 << RXOUTI))
            return;

        /* Write a Byte to FIFO */
        UEDATX = pgm_read_byte(descriptor++);

        /* After 8 Bytes send the package and delete memory bank */ 
        if ((i % Ep0_fs) == 0)
        {
            /* FIFO is full, send data */
            CBI(UEINTX,TXINI); /* Send IN package */
            while (!(UEINTX & ((1<<RXOUTI) | (1<<TXINI)))); /* Wait for ACK from host */
        }
    }

    if (!(UEINTX & (1 << RXOUTI)))
    {
        CBI(UEINTX,TXINI);                  /* Send IN package */
        while (!(UEINTX & (1 << RXOUTI)));  /* Wait for (ZLP) ACK from host */
    }   
    
    CBI(UEINTX, RXOUTI); /* Handshake to acknowledge IRQ */
}
