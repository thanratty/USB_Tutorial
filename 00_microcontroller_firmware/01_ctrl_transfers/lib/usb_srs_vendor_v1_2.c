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
#include "usb_descriptors.h"
#include "usb_winusb.h"

#include "leds.h"
#include "uart.h"
#include "phex.h"

#define U16TOBYTES(x) (x) & 0xFF, (x) >> 8



#define     DB_SERIAL(x)        do { uart1_putc(x); } while(0)



// Count # of SOS button presses here
uint8_t button_counter = 0;

bool leds_on = true;



uint8_t bmRequestType;
uint8_t bRequest;
uint8_t wValue_l,  wValue_h;
uint8_t wIndex_l,  wIndex_h;
uint8_t wLength_l, wLength_h;
uint16_t wLength, wValue, wIndex;




    /*
     * Declare the USB descriptors in flash memory
     */

    // Device Descriptor
    static const uint8_t PROGMEM dev_des[18] = {
        18,                                     /* bLength = 18 */  
        USB_DEVICE_DESCRIPTOR,                  /* bDescriptorType = 0x01, Descriptor ID = 1 -> Device descriptor */
        U16TOBYTES(0x0200),                     /* bcdUSB = 0x0200, USB_Spec2_0 */
        0x00,                                   /* bDeviceClass = 0x00, class code defined on interface level */
        0x00,                                   /* bDeviceSubClass = 0x00 */
        0x00,                                   /* bDeviceProtocoll = 0x00 */
        Ep0_fs,                                 /* bMaxPacketSize0 = EP0FS, max. package size EP0 (here 8 B) */
        U16TOBYTES(MY_VID),                     /* idVendor */
        U16TOBYTES(MY_PID),                     /* idProduct */
        0x01,0x00,                              /* bcdDevice = 0x0100, Release number device */
        Manu_i,                                 /* iManufacturer = Index for string-descriptor manufacturer */
        Prod_i,                                 /* iProduct = Index for string-descriptor product */
        Seri_i,                                 /* iSerialNumber = Index for string-descriptor serial number */
        0x01                                    /* bNumConfigurations = 1, Number of available configurations */
    };

    // Configurations Descriptor
    static const uint8_t PROGMEM conf_des[18] =  {
        9,                                      /* bLength = 0x09, descriptor length in bytes */
        USB_CONFIGURATION_DESCRIPTOR,           /* bDescriptorType = 0x02, Descriptor ID = 2 -> Configuration descriptor */
        U16TOBYTES(18),                         /* wTotalLength = Config + Interface desc len, 9+9 */
        1,                                      /* bNumInterfaces = 1 */
        1,                                      /* bConfigurationValue = 1, must not be 0 */
        0,                                      /* iConfiguration = 0, index for str.-descriptor configuration */
        USB_ATTRIBUTE_BUS_POWERED,              /* bmAttributes = 0x80,bus-powered, no remote wakeup Bit 7=1 */ 
        250,                                    /* MaxPower = 250(dezimal), means 250*2mA = 500mA */ 

    // Interface Descriptor
        9,                                      /* bLength = 0x09, length of descriptor in bytes */
        USB_INTERFACE_DESCRIPTOR,               /* bDescriptorType = 0x04, descriptor ID = 4 -> Interface descriptor*/
        0,                                      /* bInterfaceNumber = 0; */
        0,                                      /* bAlternateSetting = 0; */
        Nr_eps,                                 /* bNumEndpoints = USB_Endpoints; */
        0xFF,                                   /* bInterfaceClass = 0xFF, classcode: custome (0xFF) */
        0xFF,                                   /* bInterfaceSubClass = 0xFF, subclasscode: custome (0xFF) */
        0xFF,                                   /* bInterfaceProtocol = 0xFF, protocoll code: custome (0xFF) */
        Intf_i,                                 /* iInterface = 0, Index for string descriptor interface */

        // Endpoint descriptors after here
    };                    


    /*** Language Descriptor ***/      
    static const uint8_t PROGMEM lang_des[4] = {
        4,                                      /* bLength = 0x04, length of descriptor in bytes */
        USB_STRING_DESCRIPTOR,                  /* bDescriptorType = 0x03, Descriptor ID = 3 -> String descriptor */
        0x09,0x04                               /* wLANGID[0] = 0x0409 = English USA (Supported Lang. Code 0) */
    };

    /*** Manufacturer Descriptor ***/          
    static const uint8_t PROGMEM manu_des[30] = {
        30,                                     /* bLength = 0x04, length of descriptor in bytes */
        USB_STRING_DESCRIPTOR,                  /* bDescriptorType = 0x03, Descriptor ID = 3 -> String descriptor */
                                                /* bString = Unicode Encoded String (16 Bit!) */
        'P',0, 'i',0, 'n',0, 'k',0, ' ',0, 'O',0, 'b',0, 'o',0, 'e',0, ' ',0, 'L',0, 't',0, 'd',0, '.',0
    };    

    /*** Product Descriptor ***/       
    static const uint8_t PROGMEM prod_des[26] = {
        26,                     /* bLength = 0x22, length of descriptor in bytes */
        USB_STRING_DESCRIPTOR,         /* bDescriptorType = 0x03, Descriptor ID = 3 -> String descriptor */
                                /* bString = Unicode Encoded String (16 Bit!)*/
        'S',0,'p',0,'a',0,'f',0,'f',0,' ',0,'C',0,'a',0,'n',0,'n',0,'o',0,'n',0
    };        

    /*** Serial Number Descriptor ***/      
    static const uint8_t PROGMEM seri_des[10] = {
        10,                     /* bLength = 0x12, length of descriptor in bytes */
        USB_STRING_DESCRIPTOR,         /* bDescriptorType = 0x03, Descriptor ID = 3 -> String descriptor */
                                /* bString = Unicode Encoded String (16 Bit!) */
        '1',0,'2',0,'3',0,'4',0,
    };

    // Reference: https://learn.microsoft.com/en-us/windows-hardware/drivers/network/mb-interface-model-supplement
/* 
    static const uint8_t PROGMEM MS_OS_String_des[18] = 
    {
        18,                         // bLength (checked OK)
        USB_STRING_DESCRIPTOR,             // bType = string

        // MSFT100
        'M', 0x00, 'S', 0x00, 'F', 0x00, 'T', 0x00, '1', 0x00, '0', 0x00, '0', 0x00,

        0x33,                       // bVendorCode
        0x00                        // Pad field
    };


 */



const alignas(4) usb_bos_hierarchy_t usb_bos_hierarchy =
{
  .bos =
  {
    .bLength             = sizeof(usb_bos_descriptor_t),
    .bDescriptorType     = USB_BOS_DESCRIPTOR,
    .wTotalLength        = sizeof(usb_bos_hierarchy_t),
    .bNumDeviceCaps      = 1,
  },

  .winusb =
  {
    .bLength                = sizeof(usb_winusb_capability_descriptor_t),
    .bDescriptorType        = USB_DEVICE_CAPABILITY_DESCRIPTOR,
    .bDevCapabilityType     = USB_DEVICE_CAPABILITY_PLATFORM,
    .bReserved              = 0,
    .PlatformCapabilityUUID = USB_WINUSB_PLATFORM_CAPABILITY_ID,
    .dwWindowsVersion       = USB_WINUSB_WINDOWS_VERSION,
    .wMSOSDescriptorSetTotalLength = sizeof(usb_msos_descriptor_set_t),
    .bMS_VendorCode         = USB_WINUSB_VENDOR_CODE,
    .bAltEnumCode           = 0,
  },
};



const alignas(4) usb_msos_descriptor_set_t usb_msos_descriptor_set =
{
  .header =
  {
    .wLength             = sizeof(usb_winusb_set_header_descriptor_t),
    .wDescriptorType     = USB_WINUSB_SET_HEADER_DESCRIPTOR,
    .dwWindowsVersion    = USB_WINUSB_WINDOWS_VERSION,
    .wDescriptorSetTotalLength = sizeof(usb_msos_descriptor_set_t),
  },

  .subset =
  {
    .header = {
      .wLength           = sizeof(usb_winusb_subset_header_function_t),
      .wDescriptorType   = USB_WINUSB_SUBSET_HEADER_FUNCTION,
      .bFirstInterface   = USB_INTF_BULK,
      .bReserved         = 0,
      .wSubsetLength     = sizeof(usb_msos_descriptor_subset_t),
    },

    .comp_id =
    {
      .wLength           = sizeof(usb_winusb_feature_compatble_id_t),
      .wDescriptorType   = USB_WINUSB_FEATURE_COMPATBLE_ID,
      .CompatibleID      = { 'W', 'I', 'N', 'U', 'S', 'B', '\0', '\0' },
      .SubCompatibleID   = { 0 },
    },

    .property =
    {
      .wLength             = sizeof(usb_winusb_feature_reg_property_guids_t),
      .wDescriptorType     = USB_WINUSB_FEATURE_REG_PROPERTY,
      .wPropertyDataType   = USB_WINUSB_PROPERTY_DATA_TYPE_MULTI_SZ,
      .wPropertyNameLength = sizeof(usb_msos_descriptor_set.subset.property.PropertyName),
      .PropertyName        = {
          'D',0,'e',0,'v',0,'i',0,'c',0,'e',0,'I',0,'n',0,'t',0,'e',0,'r',0,'f',0,'a',0,'c',0,'e',0,
          'G',0,'U',0,'I',0,'D',0,'s',0, 0, 0 },
      .wPropertyDataLength = sizeof(usb_msos_descriptor_set.subset.property.PropertyData),
      .PropertyData        = {
          '{',0,'C',0,'D',0,'B',0,'3',0,'B',0,'5',0,'A',0,'D',0,'-',0,'2',0,'9',0,'3',0,'B',0,'-',0,
          '4',0,'6',0,'6',0,'3',0,'-',0,'A',0,'A',0,'3',0,'6',0,'-',0,'1',0,'A',0,'A',0,'E',0,'4',0,
          '6',0,'4',0,'6',0,'3',0,'7',0,'7',0,'6',0,'}',0, 0, 0,
          0, 0 },
    },
  },
};





void prequest( void )
{
    phexbyte_sp(bmRequestType);
    phexbyte_sp(bRequest);
    phexword_sp(wValue);
    phexword_sp(wIndex);
    phexword_sp(wLength);
    pcrlf();
}




/**
 * @brief USB General Interrupt (S253)
 */ 
ISR(USB_GEN_vect)
{
    // Check for 'end of reset' interrupt
    if (UDINT & EORSTI_m)
    {
        // Clear 'End of Reset' interrupt
        CBI(UDINT,EORSTI);
        // Init Endpoint 0
        usb_init_endpoint(0, Ep0_ty, Ep0_di, Ep0_si, Ep0_ba);    
        // Enable RX IRQ
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
        // Check if Setup package arrived, if so call usb_ep0_setup
        case 1: 
	        // Select EP 0
            UENUM = 0;
            SBI(UEINTX,RXSTPI);
            usb_ep0_setup();
            break;

        /* Expand here for other endpoints */

        default:
            DB_SERIAL('x');
            break;
    }
}



/**
 * @brief USB-Aktivierung (Full-Speed 12Mbit/s) und Interrupts erlauben
 */
void usb_init_device(void)
{
    UHWCON = UVREGE_m;                  // Enable USB Pad regulator */
    // Enable USB power
    USBCON = (USBE_m | FRZCLK_m | OTGPADE_m);
    CBI(USBCON, FRZCLK);                // Toggle FRZCLK to get WAKEUP IRQ */
    SBI(USBCON, FRZCLK);

    // Configure the PLL
    PLLCSR = PLLPRE;                    // PLL Control & Status. Set PINDIV=1 for 16MHz
    SBI(PLLCSR, PLLE);                  // Enable PLL
    while (!(PLLCSR & PLOCK_m));        // Wait for PLL to lock

    CBI(USBCON, FRZCLK);                // Leave Power Saving mode
    UDCON = 0;                          // Attach device

    SBI(UDIEN, EORSTE);                 // Enable Reset IRQ
    sei();                              // Global IRQ enable
}



/**
 * @brief Activate Endpoint
 *
 * @param number     EP Number: 0...6
 * @param type:      0 = Control, 1 = Isochron, 2 = Bulk 3 = Interrupt
 * @param direction: 0 = OUT, 1 = IN 
 * @param size:      0 = 8 Bytes, 1 = 16 Bytes, 2 = 32 Bytes, 3 = 64 Bytes, 
 *                   4 = 128 Bytes (only EP1), 5 = 256 Bytes (EP1), others reserved
 * @param bank:      0 = 1 Bank, 1 = 2 banks, others reserved
 */
void usb_init_endpoint( uint8_t number, 
                        uint8_t type, 
                        uint8_t direction, 
                        uint8_t size, 
                        uint8_t bank )
{
    UENUM = number;     // Select EP
    SBI(UECONX, EPEN);  // Enable EP

    UECFG0X = ((type << 6) | (direction));  // Set type and direction
    UECFG1X = ((size << 4) | (bank << 2));  // Set size and num of banks

    SBI(UECFG1X,ALLOC);                     // Allocate EP memory
}



/**
 * @brief Function handles setup/control transmissions to EP0
 */
void usb_ep0_setup(void)
{
    uint8_t des_bytes;

    bmRequestType = UEDATX; 
    bRequest  = UEDATX; 
    wValue_l  = UEDATX; 
    wValue_h  = UEDATX; 
    wIndex_l  = UEDATX; 
    wIndex_h  = UEDATX;
    wLength_l = UEDATX; 
    wLength_h = UEDATX; 

    wLength = (wLength_h << 8) | wLength_l;
    wValue  = (wValue_h << 8)  | wValue_l;
    wIndex  = (wIndex_h << 8)  | wIndex_l;

    CBI(UEINTX, RXSTPI);    // ACK received Setup package

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

    switch (USB_CMD_VALUE(bRequest,bmRequestType))
    {
        // GET_STATUS, 3 Phases
        case USB_CMD(IN, DEVICE, STANDARD, GET_STATUS):
            DB_SERIAL('A');
            UEDATX = 0;                         // Send back 16 Bit status -> Not self powered, no wakeup, not halted
            UEDATX = 0;
            CBI(UEINTX,TXINI);                  // Send data (ACK) and clear FIFO
            while (!(UEINTX & RXOUTI_m));       // Wait for ZLP from host
            CBI(UEINTX,RXOUTI);                 // Clear flag
            break;

        case USB_CMD(IN, INTERFACE, STANDARD, GET_STATUS):
            DB_SERIAL('B');
            UEDATX = 0;                         // Send back 16 Bit status -> Not self powered, no wakeup, not halted
            UEDATX = 0;
            CBI(UEINTX,TXINI);                  // Send data (ACK) and clear FIFO
            while (!(UEINTX & RXOUTI_m));       // Wait for ZLP from host
            CBI(UEINTX,RXOUTI);                 // Clear flag
            break;

        // SET_ADDRESS, 2 Phases (no data phase )
        case USB_CMD(OUT, DEVICE, STANDARD, SET_ADDRESS):
            DB_SERIAL('C');
            UDADDR = (wValue_l & 0x7F);         // Save address at UADD (ADDEN = 0)
            CBI(UEINTX,TXINI);                  // Send OUT package (ZLP) and clear bank
            while (!(UEINTX & TXINI_m));        // Wait for bank to be cleared
            SBI(UDADDR, ADDEN);                 // Enable address
            break;

        // GET_DESCRIPTOR, 3 Phase Transfer
        case USB_CMD(IN, DEVICE, STANDARD, GET_DESCRIPTOR):
            // Descriptor type
            switch (wValue_h)
            {
                case USB_DEVICE_DESCRIPTOR:
                    DB_SERIAL('D');
                    des_bytes = pgm_read_byte(&dev_des[0]);	      
                    usb_send_descriptor(&dev_des, des_bytes);         
                    break;

                case USB_BOS_DESCRIPTOR:
                    DB_SERIAL('E');
                    des_bytes = pgm_read_byte(&usb_bos_hierarchy);
                    usb_send_descriptor(&usb_bos_hierarchy, des_bytes);         
                    break;

                case USB_CONFIGURATION_DESCRIPTOR:
                    DB_SERIAL('F');
                    des_bytes = wLength;
                    if ((wLength_h) || (wLength_l > sizeof(conf_des)))
                        des_bytes = sizeof(conf_des);
                    usb_send_descriptor(&conf_des, des_bytes);
                    break;

                case USB_STRING_DESCRIPTOR:
                    DB_SERIAL('G');
                    if (wValue_l == Lang_i) {
                        des_bytes = pgm_read_byte(&lang_des[0]);
                        usb_send_descriptor(lang_des, des_bytes);
                        }
                    else if (wValue_l == Manu_i) {
                        des_bytes = pgm_read_byte(&manu_des[0]);
                        usb_send_descriptor(manu_des, des_bytes);
                        }
                    else if (wValue_l == Prod_i) {
                        des_bytes = pgm_read_byte(&prod_des[0]);
                        usb_send_descriptor(prod_des, des_bytes);
                        }
                    else if (wValue_l == Seri_i) {
                        des_bytes = pgm_read_byte(&seri_des[0]);
                        usb_send_descriptor(seri_des, des_bytes);
                        }
                    else
                        DB_SERIAL('s');
                    break;

                default:
                    DB_SERIAL('d');
                    break; 
            }
            break;


        case USB_CMD(IN, DEVICE, VENDOR, WINUSB_VENDOR_CODE):
            DB_SERIAL('H');
            des_bytes = pgm_read_byte(&usb_msos_descriptor_set);
            usb_send_descriptor(&usb_msos_descriptor_set, des_bytes);         
            break;

        // 2 Phase, no data phase
        case USB_CMD(OUT, DEVICE, STANDARD, SET_CONFIGURATION):
            DB_SERIAL('I');
            // Reset any endpoints here...
            UENUM = 0;                              // Select EP0
            CBI(UEINTX, TXINI);                     // Send ZLP
            while (!(UEINTX & TXINI_m));            // Wait until ZLP is send and bank is cleared
            break;

        case USB_CMD(OUT, DEVICE, VENDOR, VENDOR_CMD_LED_ON):
            DB_SERIAL('J');
            // Update SOS button counter with value from host
            leds_on = true;
            CBI(UEINTX, TXINI);                 // Send OUT package (ZLP) and clear bank
            while (!(UEINTX & TXINI_m));        // Wait for bank to be cleared
            break;

        case USB_CMD(OUT, DEVICE, VENDOR, VENDOR_CMD_LED_OFF):
            DB_SERIAL('K');
            leds_on = false;
            CBI(UEINTX, TXINI);                 // Send OUT package (ZLP) and clear bank
            while (!(UEINTX & TXINI_m));        // Wait for bank to be cleared
            break;

        case USB_CMD(OUT, DEVICE, VENDOR, VENDOR_CMD_TOGGLE_LED):
            DB_SERIAL('L');
            TX_LED_TOGGLE;
            CBI(UEINTX, TXINI);                 // Send OUT package (ZLP) and clear bank
            while (!(UEINTX & TXINI_m));        // Wait for bank to be cleared
            break;

        case USB_CMD(OUT, DEVICE, VENDOR, VENDOR_CMD_SET_COUNTER):
            DB_SERIAL('M');
            // Update SOS button counter with value from host
            button_counter = wValue_l;
            CBI(UEINTX, TXINI);                 // Send OUT package (ZLP) and clear bank
            while (!(UEINTX & TXINI_m));        // Wait for bank to be cleared
            break;

        case USB_CMD(IN, DEVICE, VENDOR, VENDOR_CMD_GET_COUNTER):
            DB_SERIAL('N');
            // Return current SOS button counter value to host. Increment locally every time.
            UEDATX = button_counter++;
            for(uint8_t i=1 ; i<wLength_l ; i++)
            {
                UEDATX = i;
                if ((i % Ep0_fs) == 0) {
                    // FIFO is full, send data
                    CBI(UEINTX,TXINI);                             // Send IN package
                    while (!(UEINTX & (RXOUTI_m | TXINI_m)));      // Wait for ACK from host
                }
            }
            if (!(UEINTX & RXOUTI_m)) {
                CBI(UEINTX,TXINI);                                 // Send IN package
                while (!(UEINTX & RXOUTI_m));                      // Wait for (ZLP) ACK from host
            }   

            CBI(UEINTX,RXOUTI);     // Handshake to acknowledge IRQ

        default:
            DB_SERIAL('k');
            prequest();
            wLength = USB_CMD(OUT, DEVICE, VENDOR, VENDOR_CMD_TOGGLE_LED);
            phexword(wLength);
            pcrlf();

            SBI(UECONX,STALLRQ);
            break;
    }
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
void usb_send_descriptor(const void* ptr, uint8_t desc_bytes)
{
const uint8_t* descriptor = (const uint8_t*) ptr;

    for (uint16_t i=1 ; i<=desc_bytes ; i++)
    {
        // Abort if the host wants to
        if (UEINTX & RXOUTI_m)
            return;

        // Write byte to FIFO
        UEDATX = pgm_read_byte(descriptor++);

        // After 8 Bytes send the package and delete memory bank
        if ((i % Ep0_fs) == 0)
        {
            // FIFO is full, send data
            CBI(UEINTX, TXINI);                           // Send IN package
            while (!(UEINTX & (RXOUTI_m | TXINI_m)));     // Wait for ACK from host
        }
    }

    if (!(UEINTX & RXOUTI_m))
    {
        CBI(UEINTX, TXINI);                 // Send IN package
        while (!(UEINTX & RXOUTI_m));       // Wait for (ZLP) ACK from host
    }   

    CBI(UEINTX, RXOUTI);                    // Handshake to acknowledge IRQ
}
