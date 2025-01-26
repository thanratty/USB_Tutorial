#include <stdint.h>
#include <avr/pgmspace.h> 
#include <avr/interrupt.h> 

#include "usb_srs_vendor_v1_2.h"
#include "descriptors.h"

#include "leds.h"
#include "uart.h"
#include "phex.h"





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
        usb_init_endpoint( 0, EP0_TYPE, EP0_DIRECTION, EP0_SIZE, EP0_BANK );
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


    prequest();

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
            switch (wValue_h)
            {
                case USB_DESC_DEVICE:
                    DB_SERIAL('D');
                    des_bytes = pgm_read_byte(&dev_des);	      
                    usb_send_descriptor(&dev_des, des_bytes);         
                    break;

                // NB - out BOS must be <= 0xFF bytes long
                case USB_DESC_BOS:
                    DB_SERIAL('E');
                    usb_send_descriptor(&usb_bos_descriptor, wLength_l);
                    break;

                case USB_DESC_CONFIG:
                    DB_SERIAL('F');
                    des_bytes = (wLength > sizeof(conf_des)) ? sizeof(conf_des) : wLength;
                    usb_send_descriptor(&conf_des, des_bytes);
                    break;

                case USB_DESC_STRING:
                    DB_SERIAL('G');
                    if (wValue_l == STR_INDEX_LANGUAGE) {
                        des_bytes = pgm_read_byte(&lang_des[0]);
                        usb_send_descriptor(lang_des, des_bytes);
                        }
                    else if (wValue_l == STR_INDEX_MANUFACTURER) {
                        des_bytes = pgm_read_byte(&manu_des[0]);
                        usb_send_descriptor(manu_des, des_bytes);
                        }
                    else if (wValue_l == STR_INDEX_PRODUCT) {
                        des_bytes = pgm_read_byte(&prod_des[0]);
                        usb_send_descriptor(prod_des, des_bytes);
                        }
                    else if (wValue_l == STR_INDEX_SERIAL) {
                        des_bytes = pgm_read_byte(&seri_des[0]);
                        usb_send_descriptor(seri_des, des_bytes);
                        }
                    else if (wValue_l == STR_INDEX_INTERFACE) {
                        des_bytes = pgm_read_byte(&iface_des[0]);
                        usb_send_descriptor(iface_des, des_bytes);
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
            usb_send_descriptor(&usb_msos_descriptor_set, wLength);
            break;

        // 2 Phase, no data phase
        case USB_CMD(OUT, DEVICE, STANDARD, SET_CONFIGURATION):
            DB_SERIAL('I');
            // Reset any endpoints here...
            UENUM = 0;                              // Select EP0
            CBI(UEINTX, TXINI);                     // Send ZLP
            while (!(UEINTX & TXINI_m));            // Wait until ZLP is send and bank is cleared
            break;

        case USB_CMD(OUT, DEVICE, VENDOR, VENDOR_LED_ON):
            DB_SERIAL('J');
            // Update SOS button counter with value from host
            leds_on = true;
            CBI(UEINTX, TXINI);                 // Send OUT package (ZLP) and clear bank
            while (!(UEINTX & TXINI_m));        // Wait for bank to be cleared
            break;

        case USB_CMD(OUT, DEVICE, VENDOR, VENDOR_LED_OFF):
            DB_SERIAL('K');
            leds_on = false;
            CBI(UEINTX, TXINI);                 // Send OUT package (ZLP) and clear bank
            while (!(UEINTX & TXINI_m));        // Wait for bank to be cleared
            break;

        case USB_CMD(OUT, DEVICE, VENDOR, VENDOR_TOGGLE_LED):
            DB_SERIAL('L');
            TX_LED_TOGGLE;
            CBI(UEINTX, TXINI);                 // Send OUT package (ZLP) and clear bank
            while (!(UEINTX & TXINI_m));        // Wait for bank to be cleared
            break;

        case USB_CMD(OUT, DEVICE, VENDOR, VENDOR_SET_COUNTER):
            DB_SERIAL('M');
            // Update SOS button counter with value from host
            button_counter = wValue_l;
            CBI(UEINTX, TXINI);                 // Send OUT package (ZLP) and clear bank
            while (!(UEINTX & TXINI_m));        // Wait for bank to be cleared
            break;

        case USB_CMD(IN, DEVICE, VENDOR, VENDOR_GET_COUNTER):
            DB_SERIAL('N');
            // Return current SOS button counter value to host. Increment locally every time.
            UEDATX = button_counter++;
            for(uint8_t i=1 ; i<wLength_l ; i++)
            {
                UEDATX = i;
                if ((i % EP0_MAX_SIZE) == 0) {
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
            break;

        default:
            DB_SERIAL('k');
            prequest();
            wLength = USB_CMD(OUT, DEVICE, VENDOR, VENDOR_TOGGLE_LED);
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
        if ((i % EP0_MAX_SIZE) == 0)
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
