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
#define     MY_VID_HI                   0x84
#define     MY_VID_LO                   0x77
#define     MY_PID_HI                   0x31
#define     MY_PID_LO                   0x18



#endif
