#ifndef _USB_DEFS_
#define _USB_DEFS_


// There are eight Standard Device Requests

#define     GET_STATUS                  0x00
#define     CLEAR_FEATURE               0x01
#define     SET_FEATURE                 0x03
#define     SET_ADDRESS                 0x05
#define     GET_DESCRIPTOR              0x06
#define     SET_DESCRIPTOR              0x07
#define     GET_CONFIGURATION           0x08
#define     SET_CONFIGURATION           0x09


// Descriptor types

#define     DEVICE_DESCRIPTOR           0x01
#define     CONFIGURATION_DESCRIPTOR    0x02
#define     STRING_DESCRIPTOR           0x03


// Use VID = 0x8477 and PID = 0x3118
#define     MY_VID                      0x8477
#define     MY_PID                      0x3118


// Prepare some handy bit masks from bit numbers

#define     UVREG_m         (1<<UVREGE)
#define     USBE_m          (1<<USBE)
#define     FRZCLK_m        (1<<FRZCLK)
#define     OTGPADE_m       (1<<OTGPADE)
#define     EORSTI_m        (1<<EORSTI)
#define     PLOCK_m         (1<<PLOCK)
#define     RXOUTI_m        (1<<RXOUTI)
#define     TXINI_m         (1<<TXINI)
#define     UVREGE_m        (1<<UVREGE)

#endif
