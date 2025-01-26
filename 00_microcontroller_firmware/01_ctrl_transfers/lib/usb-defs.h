#ifndef _USB_DEFS_H_
#define _USB_DEFS_H_



#define     MY_VID                  0x8477
#define     MY_PID                  0x3119
#define     MY_DEVICE_VERSION       0x0003

#define     USB_VERSION             0x0210

#define     USB_CMD_WINUSB_VENDOR_CODE          0x27

#define     USB_WINUSB_WINDOWS_VERSION          0x06030000

#define     USB_WINUSB_PLATFORM_CAPABILITY_ID \
    { 0xdf, 0x60, 0xdd, 0xd8, 0x89, 0x45, 0xc7, 0x4c, \
      0x9c, 0xd2, 0x65, 0x9d, 0x9e, 0x64, 0x8a, 0x9f }


/**
 * Our private commands sent through the control endpoint.
 */
#define     USB_CMD_VENDOR_SET_COUNTER      1
#define     USB_CMD_VENDOR_GET_COUNTER      2
#define     USB_CMD_VENDOR_LED_ON           3
#define     USB_CMD_VENDOR_LED_OFF          4
#define     USB_CMD_VENDOR_TOGGLE_LED       5








#define USB_LANGID_ENGLISH     0x0409 // English (United States)

#define USB_PACK               __attribute__((packed))

#define USB_LIMIT(a, b)        (((int)(a) > (int)(b)) ? (int)(b) : (int)(a))

#define USB_CMD_VALUE(bRequest,bmRequestType)     ((bRequest << 8) | bmRequestType)

#define USB_CMD(dir, rcpt, type, cmd) \
    ((USB_CMD_##cmd << 8) | (USB_##dir##_TRANSFER << 7) | \
     (USB_##type##_REQUEST << 5) | (USB_##rcpt##_RECIPIENT << 0))




/*
 * USB Request Codes
 */
enum
{
  USB_CMD_GET_STATUS        = 0,
  USB_CMD_CLEAR_FEATURE     = 1,
  USB_CMD_SET_FEATURE       = 3,
  USB_CMD_SET_ADDRESS       = 5,
  USB_CMD_GET_DESCRIPTOR    = 6,
  USB_CMD_SET_DESCRIPTOR    = 7,
  USB_CMD_GET_CONFIGURATION = 8,
  USB_CMD_SET_CONFIGURATION = 9,
  USB_CMD_GET_INTERFACE     = 10,
  USB_CMD_SET_INTERFACE     = 11,
  USB_CMD_SYNCH_FRAME       = 12
};


/*
 * USB Descriptor Types
 */
enum
{
  USB_DESC_DEVICE                    = 1,
  USB_DESC_CONFIG                    = 2,
  USB_DESC_STRING                    = 3,
  USB_DESC_INTERFACE                 = 4,
  USB_DESC_ENDPOINT                  = 5,
  USB_DESC_DEVICE_QUALIFIER          = 6,
  USB_DESC_OTHER_SPEED_CONFIG        = 7,
  USB_DESC_INTERFACE_POWER           = 8,
  USB_DESC_OTG                       = 9,
  USB_DESC_DEBUG                     = 10,
  USB_DESC_INTERFACE_ASSOCIATION     = 11,
  USB_DESC_BOS                       = 15,
  USB_DESC_DEVICE_CAPABILITY         = 16,
  USB_DESC_CS_INTERFACE              = 36
};


enum
{
  USB_DEVICE_CAPABILITY_WIRELESS_USB               = 1,
  USB_DEVICE_CAPABILITY_USB_2_0_EXTENSION          = 2,
  USB_DEVICE_CAPABILITY_SUPERSPEED_USB             = 3,
  USB_DEVICE_CAPABILITY_CONTAINER_ID               = 4,
  USB_DEVICE_CAPABILITY_PLATFORM                   = 5,
  USB_DEVICE_CAPABILITY_POWER_DELIVERY             = 6,
  USB_DEVICE_CAPABILITY_BATTERY_INFO               = 7,
  USB_DEVICE_CAPABILITY_PD_CONSUMER_PORT           = 8,
  USB_DEVICE_CAPABILITY_PD_PROVIDER_PORT           = 9,
  USB_DEVICE_CAPABILITY_SUPERSPEED_PLUS            = 10,
  USB_DEVICE_CAPABILITY_PRECISION_TIME_MEASUREMENT = 11,
  USB_DEVICE_CAPABILITY_WIRELESS_USB_EXT           = 12
};


enum
{
  USB_ATTRIBUTE_REMOTE_WAKEUP = 0x20,
  USB_ATTRIBUTE_SELF_POWERED  = 0x40,
  USB_ATTRIBUTE_BUS_POWERED   = 0x80
};


enum
{
  USB_DEVICE_RECIPIENT     = 0,
  USB_INTERFACE_RECIPIENT  = 1,
  USB_ENDPOINT_RECIPIENT   = 2,
  USB_OTHER_RECIPIENT      = 3
};


enum
{
  USB_STANDARD_REQUEST     = 0,
  USB_CLASS_REQUEST        = 1,
  USB_VENDOR_REQUEST       = 2,
  USB_RESERVED_REQUEST     = 3
};


enum
{
  USB_OUT_TRANSFER         = 0,
  USB_IN_TRANSFER          = 1
};


enum
{
  USB_DEVICE_CLASS_MISCELLANEOUS   = 0xef,
  USB_DEVICE_CLASS_VENDOR_SPECIFIC = 0xff,
};


enum
{
  USB_DEVICE_SUBCLASS_COMMON = 0x02,
};


enum // MS OS wDescriptorType
{
  USB_MSOS_DESCRIPTOR_SET              = 0,
  USB_MSOS_SUBSET_HEADER_CONFIGURATION = 1,
  USB_MSOS_SUBSET_HEADER_FUNCTION      = 2,
  USB_MSOS_FEATURE_COMPATIBLE_ID       = 3,
  USB_MSOS_FEATURE_REG_PROPERTY        = 4,
  USB_MSOS_FEATURE_MIN_RESUME_TIME     = 5,
  USB_MSOS_FEATURE_MODEL_ID            = 6,
  USB_MSOS_FEATURE_CCGP_DEVICE         = 7,
  USB_MSOS_FEATURE_VENDOR_REVISION     = 8,
  USB_MSOS_INTERFACE_ASSOCIATION       = 11,
  USB_MSOS_BOS_DESC                    = 15,
  USB_MSOS_DEVICE_CAPABILITY_DESC      = 16,
  USB_MSOS_CS_INTERFACE_DESC           = 36,

};

enum // MS OS wPropertyDataType
{
  USB_MSOS_PROPERTY_DATA_TYPE_SZ                  = 1,
  USB_MSOS_PROPERTY_DATA_TYPE_EXPAND_SZ           = 2,
  USB_MSOS_PROPERTY_DATA_TYPE_BINARY              = 3,
  USB_MSOS_PROPERTY_DATA_TYPE_DWORD_LITTLE_ENDIAN = 4,
  USB_MSOS_PROPERTY_DATA_TYPE_DWORD_BIG_ENDIAN    = 5,
  USB_MSOS_PROPERTY_DATA_TYPE_LINK                = 6,
  USB_MSOS_PROPERTY_DATA_TYPE_MULTI_SZ            = 7,
};



#endif
