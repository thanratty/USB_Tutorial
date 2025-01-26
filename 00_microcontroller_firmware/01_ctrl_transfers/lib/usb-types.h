#ifndef _USB_TYPES_H_
#define _USB_TYPES_H_



/*- Includes ----------------------------------------------------------------*/
#include <stddef.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdalign.h>





typedef struct USB_PACK
{
  uint8_t   bmRequestType;
  uint8_t   bRequest;
  uint16_t  wValue;
  uint16_t  wIndex;
  uint16_t  wLength;
} usb_request_t;


/*
typedef struct USB_PACK
{
  uint8_t   bLength;
  uint8_t   bDescriptorType;
} usb_descriptor_header_t;
*/


typedef struct USB_PACK
{
  uint8_t   bLength;
  uint8_t   bDescriptorType;
  uint16_t  bcdUSB;
  uint8_t   bDeviceClass;
  uint8_t   bDeviceSubClass;
  uint8_t   bDeviceProtocol;
  uint8_t   bMaxPacketSizeEp0;
  uint16_t  idVendor;
  uint16_t  idProduct;
  uint16_t  bcdDevice;
  uint8_t   iManufacturer;
  uint8_t   iProduct;
  uint8_t   iSerialNumber;
  uint8_t   bNumConfigurations;
} usb_device_descriptor_t;

typedef struct USB_PACK
{
  uint8_t   bLength;
  uint8_t   bDescriptorType;
  uint8_t   bInterfaceNumber;
  uint8_t   bAlternateSetting;
  uint8_t   bNumEndpoints;
  uint8_t   bInterfaceClass;
  uint8_t   bInterfaceSubClass;
  uint8_t   bInterfaceProtocol;
  uint8_t   iInterface;
} usb_interface_descriptor_t;

typedef struct USB_PACK
{
  uint8_t   bLength;
  uint8_t   bDescriptorType;
  uint16_t  wTotalLength;
  uint8_t   bNumInterfaces;
  uint8_t   bConfigurationValue;
  uint8_t   iConfiguration;
  uint8_t   bmAttributes;
  uint8_t   bMaxPower;
  usb_interface_descriptor_t    interface;
} usb_config_descriptor_t;

typedef struct USB_PACK
{
  uint8_t   bLength;
  uint8_t   bDescriptorType;
  uint16_t  wLANGID;
} usb_string_descriptor_zero_t;

typedef struct USB_PACK
{
  uint8_t   bLength;
  uint8_t   bDescriptorType;
  uint16_t  bString;
} usb_string_descriptor_t;




typedef struct USB_PACK
{
    uint8_t     bLength;
    uint8_t     bDescriptorType;
    uint8_t     bCapabilityType;
    uint8_t     bReserved;
    uint8_t     msGUID[ 16 ];
    uint32_t    dwWindowsVersion;
    uint16_t    wMSOSTotalLength;
    uint8_t     bVendorRequestCode;
    uint8_t     bAltEnum;
} usb_bos_platform_descriptor_t;


typedef struct USB_PACK
{
  uint8_t       bLength;
  uint8_t       bDescriptorType;
  uint16_t      wTotalLength;
  uint8_t       bNumDeviceCaps;
} usb_bos_header_t;


typedef struct USB_PACK
{
  usb_bos_header_t                  header;
  usb_bos_platform_descriptor_t     platform;
} usb_bos_descriptor_t;






typedef struct USB_PACK
{
  uint16_t  wLength;
  uint16_t  wDescriptorType;
  uint16_t  wStringType;
  uint16_t  wPropertyNameLength;
  uint8_t   PropertyName[ 42 ];
  uint16_t  wPropertyDataLength;
  uint8_t   PropertyData[ 80 ];
} usb_reg_property_GUID_descriptor_t;



typedef struct USB_PACK
{
  uint16_t  wLength;
  uint16_t  wDescriptorType;
  uint8_t   CompatibleID[8];        // Corrent
  uint8_t   SubCompatibleID[8];     // Correct
  usb_reg_property_GUID_descriptor_t        regprop;
} usb_msos_compatble_feature_descriptor_t;


typedef struct USB_PACK
{
    uint16_t    wLength;
    uint16_t    wDescriptorType;
    uint32_t    dwWinVersion;
    uint16_t    wTotalLength;
} usb_msos_descriptor_set_header_t;



typedef struct USB_PACK
{
    usb_msos_descriptor_set_header_t            header;
    usb_msos_compatble_feature_descriptor_t     feature;
} usb_msos_descriptor_set_t;








#endif // _USB_STD_H_
