#include <stdint.h>
#include <assert.h>
#include <avr/pgmspace.h> 
#include <avr/interrupt.h> 

#include "usb-defs.h"
#include "usb-types.h"
#include "descriptors.h"


    /*
     * Declare the USB descriptors in flash memory
     */


    /*** Language Descriptor ***/      
    const uint8_t PROGMEM lang_des[] = {
        4,                                      /* bLength = 0x04, length of descriptor in bytes */
        USB_DESC_STRING,                        /* bDescriptorType = 0x03, Descriptor ID = 3 -> String descriptor */
        0x09,0x04                               /* wLANGID[0] = 0x0409 = English USA (Supported Lang. Code 0) */
    };

    /*** Manufacturer Descriptor ***/          
    const uint8_t PROGMEM manu_des[] = {
        30,                                     /* bLength = 0x04, length of descriptor in bytes */
        USB_DESC_STRING,                        /* bDescriptorType = 0x03, Descriptor ID = 3 -> String descriptor */
        'P',0, 'i',0, 'n',0, 'k',0, ' ',0, 'O',0, 'b',0, 'o',0, 'e',0, ' ',0, 'L',0, 't',0, 'd',0, '.',0
    };    

    /*** Product Descriptor ***/       
    const uint8_t PROGMEM prod_des[] = {
        26,                      /* bLength = 0x22, length of descriptor in bytes */
        USB_DESC_STRING,         /* bDescriptorType = 0x03, Descriptor ID = 3 -> String descriptor */
        'S',0,'p',0,'a',0,'f',0,'f',0,' ',0,'C',0,'a',0,'n',0,'n',0,'o',0,'n',0
    };        

    /*** Serial Number Descriptor ***/      
    const uint8_t PROGMEM seri_des[] = {
        10,                      /* bLength = 0x12, length of descriptor in bytes */
        USB_DESC_STRING,         /* bDescriptorType = 0x03, Descriptor ID = 3 -> String descriptor */
        '1',0,'2',0,'3',0,'4',0,
    };

    /*** Serial Number Descriptor ***/      
    const uint8_t PROGMEM iface_des[] = {
        12,                      /* bLength = 0x12, length of descriptor in bytes */
        USB_DESC_STRING,         /* bDescriptorType = 0x03, Descriptor ID = 3 -> String descriptor */
        'i',0,'F',0,'a',0,'c',0,'e',0
    };

    /*** Serial Number Descriptor ***/      
    const uint8_t PROGMEM iconf_des[] = {
        26,                      /* bLength */
        USB_DESC_STRING,         /* bDescriptorType = 0x03, Descriptor ID = 3 -> String descriptor */
        'B',0,'u',0,'t',0,'t',0,'o',0,'n',0,
        'C',0,'o',0,'n',0,'f',0,'i',0,'g',0
    };





const alignas(4) usb_bos_descriptor_t PROGMEM usb_bos_descriptor =
{
  .header =
  {
    .bLength                = sizeof(usb_bos_header_t),
    .bDescriptorType        = USB_DESC_BOS,
    .wTotalLength           = sizeof(usb_bos_descriptor),
    .bNumDeviceCaps         = 1
  },

  .platform =
  {
    .bLength                = sizeof(usb_bos_platform_descriptor_t),
    .bDescriptorType        = USB_DESC_DEVICE_CAPABILITY,
    .bCapabilityType        = USB_DEVICE_CAPABILITY_PLATFORM,
    .bReserved              = 0,
    .msGUID                 = USB_WINUSB_PLATFORM_CAPABILITY_ID,
    .dwWindowsVersion       = USB_WINUSB_WINDOWS_VERSION,
    .wMSOSTotalLength       = sizeof(usb_msos_descriptor_set_t),
    .bVendorRequestCode     = USB_CMD_WINUSB_VENDOR_CODE,
    .bAltEnum               = 0
  }
};





const alignas(4) usb_msos_descriptor_set_t PROGMEM usb_msos_descriptor_set =
{
    .header =
    {
        .wLength         = sizeof(usb_msos_descriptor_set_header_t),
        .wDescriptorType = USB_MSOS_DESCRIPTOR_SET,
        .dwWinVersion    = USB_WINUSB_WINDOWS_VERSION,
        .wTotalLength    = sizeof(usb_msos_descriptor_set_t)
    },

    .feature =
    {
        .wLength         = 20,
        .wDescriptorType = USB_MSOS_FEATURE_COMPATIBLE_ID,
        .CompatibleID    = { 'W', 'I', 'N', 'U', 'S', 'B', 0, 0 },
        .SubCompatibleID = { 0, 0, 0, 0, 0, 0, 0, 0 },

        .regprop = 
        {
            .wLength = 132,
            .wDescriptorType = USB_MSOS_FEATURE_REG_PROPERTY,
            .wStringType     = USB_MSOS_PROPERTY_DATA_TYPE_MULTI_SZ,

            .wPropertyNameLength = 42,
            .PropertyName = { 'D',0,'e',0,'v',0,'i',0,'c',0,'e',0,'I',0,'n',0,'t',0,'e',0,'r',0,'f',0,'a',0,'c',0,'e',0,
                              'G',0,'U',0,'I',0,'D',0,'s',0,
                               0, 0
            },

            .wPropertyDataLength = 80,
            .PropertyData = {
                    '{',0,'C',0,'D',0,'B',0,'3',0,'B',0,'5',0,'A',0,'D',0,'-',0,'2',0,'9',0,'3',0,'B',0,'-',0,      // 30
                    '4',0,'6',0,'6',0,'3',0,'-',0,'A',0,'A',0,'3',0,'6',0,'-',0,'1',0,'A',0,'A',0,'E',0,'4',0,      // 30
                    '6',0,'4',0,'6',0,'3',0,'7',0,'7',0,'6',0,'}',0,    // 16
                    0, 0,
                    0, 0
            }
        }
    }
};





// Device Descriptor
//
const alignas(2) usb_device_descriptor_t PROGMEM device_descriptor = 
{
    .bLength                = sizeof(usb_device_descriptor_t),
    .bDescriptorType        = USB_DESC_DEVICE,
    .bcdUSB                 = USB_VERSION,
    .bDeviceClass           = USB_CLASS_VENDOR_SPECIFIC,
    .bDeviceSubClass        = 0,
    .bDeviceProtocol        = 0,
    .bMaxPacketSizeEp0      = EP0_MAX_SIZE,
    .idVendor               = MY_VID,
    .idProduct              = MY_PID,
    .bcdDevice              = MY_DEVICE_VERSION,
    .iManufacturer          = STR_INDEX_MANUFACTURER,
    .iProduct               = STR_INDEX_PRODUCT,
    .iSerialNumber          = STR_INDEX_SERIAL,
    .bNumConfigurations     = 1
};



// Configurations Descriptor
//
const alignas(2) usb_config_descriptor_t PROGMEM config_descriptor = 
{
    .bLength                = 9,
    .bDescriptorType        = USB_DESC_CONFIG,
    .wTotalLength           = 9+9,                              // Config + Interface descriptors
    .bNumInterfaces         = 1,
    .bConfigurationValue    = 1,                                // Must not be 0
    .iConfiguration         = STR_INDEX_CONFIG,                 // String index for config
    .bmAttributes           = USB_ATTRIBUTE_BUS_POWERED,
    .bMaxPower              = 150,

    // Interface Descriptor follows

    .interface =
    {
        .bLength            = 9,
        .bDescriptorType    = USB_DESC_INTERFACE,
        .bInterfaceNumber   = 0,
        .bAlternateSetting  = 0,
        .bNumEndpoints      = NUM_ENDPOINTS,
        .bInterfaceClass    = USB_CLASS_VENDOR_SPECIFIC,
        .bInterfaceSubClass = USB_CLASS_VENDOR_SPECIFIC,
        .bInterfaceProtocol = 0,
        .iInterface         = STR_INDEX_INTERFACE
    },

    // Any endpoint descriptors go here.

};


