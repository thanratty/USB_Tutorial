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
#define     BOS_DESCRIPTOR              0x0f



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



// Microsoft OS 2.0 descriptor types

#define     MS_OS_20_SET_HEADER_DESCRIPTOR	        0x00
#define     MS_OS_20_SUBSET_HEADER_CONFIGURATION	0x01
#define     MS_OS_20_SUBSET_HEADER_FUNCTION	        0x02
#define     MS_OS_20_FEATURE_COMPATBLE_ID	        0x03
#define     MS_OS_20_FEATURE_REG_PROPERTY	        0x04
#define     MS_OS_20_FEATURE_MIN_RESUME_TIME	    0x05
#define     MS_OS_20_FEATURE_MODEL_ID	            0x06
#define     MS_OS_20_FEATURE_CCGP_DEVICE	        0x07
#define     MS_OS_20_FEATURE_VENDOR_REVISION	    0x08











typedef struct tag_MS_DEV_INT_GUID
{
    uint8_t GUID[78];                       // Including trailing u'\0'
} sGUID;


typedef struct tag_MS_REG_PROP_DESC_GUID
{
    uint8_t wLength[2];                     // sizeof(struct MS_REG_PROP_DESC_GUID)
    uint8_t wDescriptorType[2];             // 0x04
    uint8_t wStringType[2];                 // 0x07 - UTF-16 encoded null terminated strings
    uint8_t wPropertyNameLength[2];         // sizeof(struct MS_DEV_GUID_NAME)
    uint8_t MS_DEV_GUID_NAME[40];           // u"DeviceInterfaceGUID", 0,0
    uint8_t wPropertyDataLength[2];         // sizeof (struct MS_DEV_INT_GUID)
    sGUID   GUID;                           // "{AB12AB12-AB12-AB12-AB12-AB12AB12AB12}"
} sMS_REG_PROP_DESC_GUID;


typedef struct tag_MS_COMP_ID_FEAT_DESC
{
    uint8_t wLength[2];                 // 0x14
    uint8_t wDescriptorType[2];         // 0x03
    uint8_t CompatibleID[8];            // 'W', 'I', 'N', 'U', 'S', 'B', '\0', 0x00
    uint8_t SubCompatibleID[8];         // 0x00
    sMS_REG_PROP_DESC_GUID GUID;
} sMS_COMP_ID_FEAT_DESC;


typedef struct tag_MS_FUNC_SUBSET_HEADER 
{
    uint8_t wLength[2];             // 0x08
    uint8_t wDescriptorType[2];     // 0x02
    uint8_t bFirstInterface;        // interface number
    uint8_t bReserved;              // 0x00
    uint8_t wSubsetLength[2];       // sizeof(sMS_FUNC_SUBSET_HEADER) including sMS_COMP_ID_FEAT_DESC
    sMS_COMP_ID_FEAT_DESC FEAT;
} sMS_FUNC_SUBSET_HEADER;


typedef struct tag_MS_OS_DESC_SET_HEADER
{
    uint8_t wLength[2];             // 0x0A
    uint8_t wDescriptorType[2];     // 0x0000
    uint8_t dwWindowsVersion[4];    // 0x06030000
    uint8_t wTotalLength[2];        // sizeof(sMS_OS_DESC_SET_HEADER) including sMS_FUNC_SUBSET_HEADER
    sMS_FUNC_SUBSET_HEADER FUNC;    // The rest of the header
} sMS_OS_DESC_SET_HEADER;



/* MS BOS Descriptor */
typedef struct tag_MS_BOS_DESCRIPTOR
{
    uint8_t bLength;            // to bNumPlatDesc: 0x05
    uint8_t bRequestType;       // 0x0F
    uint8_t wTotalLen[2];       // sizeof(struct MS_BOS_DESCRIPTOR)
    uint8_t bNumPlatDesc;       // The number of platform descriptors - just 1 needed for WINUSB

    uint8_t bDescLen;           // len to end of struct
    uint8_t bDescType;          // 0x10
    uint8_t bCapabilityType;    // 0x05
    uint8_t bReserved;          // 0x00

    uint8_t MSGUID[16]; 

    uint8_t dwWinVersion[4];    // 0x06030000 for Win 8.1 and above

    uint8_t wOSDescLen[2];      // sizeof(struct MS_OS_DESC_SET_HEADER)
    uint8_t bRequest;           // bRequest used to get MS_OS_DESC_SET_HEADER
    uint8_t bSendRequest;       // 0x00

} sMS_BOS_DESCRIPTOR;
 



#endif
