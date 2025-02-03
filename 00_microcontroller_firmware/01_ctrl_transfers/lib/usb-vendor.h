#ifndef _USB_VENDOR_H_
#define _USB_VENDOR_H_




/************************************************************************
 * 
 * Anything outside the USB spec & specific to our defice goes in here
 * 
 ************************************************************************/


// Define our USB device
#define     MY_VID                              0x8477
#define     MY_PID                              0x3119
#define     MY_DEVICE_VERSION                   0x0003
#define     USB_CMD_WINUSB_VENDOR_CODE          0x27
#define     NUM_ENDPOINTS                       0               // Excluding EP0


/**
 * Our private commands sent through the control endpoint EP0
 */
#define     USB_CMD_VENDOR_SET_COUNTER      1
#define     USB_CMD_VENDOR_GET_COUNTER      2
#define     USB_CMD_VENDOR_LED_ON           3
#define     USB_CMD_VENDOR_LED_OFF          4
#define     USB_CMD_VENDOR_TOGGLE_LED       5


#endif

