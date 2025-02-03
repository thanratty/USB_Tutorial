#ifndef _DESCRIPTORS_H_
#define _DESCRIPTORS_H_


/*  ty TYPE:        0 = Control, 1 = Isochron, 2 = Bulk 3 = Interrupt
    di DIRECTION:   0 = OUT, 1 = IN (not for Control)
    si SIZE:        0 = 8 Bytes, 1 = 16 Bytes, 2 = 32 Bytes, 3 = 64 Bytes,
                    4 = 128 Bytes (only EP1), 5 = 256 Bytes (EP1), all others reserved
    ba BANK:        0 = 1 Bank, 1 = 2 Banks, all others reserved
*/
#define     EP_TYPE_CONTROL         0
#define     EP_TYPE_ISO             1
#define     EP_TYPE_BULK            2
#define     EP_TYPE_INTERRUPT       3

#define     EP_DIR_OUT              0
#define     EP_DIR_IN               1

 
// Endpoint 0 definition: Control OUT, 8 byte FIFO

#define     EP0_TYPE                0           // Type Control
#define     EP0_DIRECTION           0           // Direction OUT
#define     EP0_SIZE                0           // Size: 8 Bytes
#define     EP0_BANK                0           // 1 Bank
#define     EP0_MAX_SIZE            8           // Max packet size for EP0


// String indexes
#define     STR_INDEX_LANGUAGE      0           // Language Descriptor Index
#define     STR_INDEX_MANUFACTURER  1           // Manufacturer String Index
#define     STR_INDEX_PRODUCT       2           // Product String Index
#define     STR_INDEX_SERIAL        3           // Serial Number String Index
#define     STR_INDEX_INTERFACE     4           // Interface String Index
#define     STR_INDEX_CONFIG        5           // Configuration String Index



extern const uint8_t PROGMEM lang_des[];
extern const uint8_t PROGMEM manu_des[];
extern const uint8_t PROGMEM prod_des[];
extern const uint8_t PROGMEM seri_des[];
extern const uint8_t PROGMEM iface_des[];
extern const uint8_t PROGMEM iconf_des[];



extern const alignas(4) usb_bos_descriptor_t      PROGMEM usb_bos_descriptor;
extern const alignas(4) usb_msos_descriptor_set_t PROGMEM usb_msos_descriptor_set;
extern const alignas(2) usb_device_descriptor_t   PROGMEM device_descriptor;
extern const alignas(2) usb_config_descriptor_t   PROGMEM config_descriptor;

#endif
