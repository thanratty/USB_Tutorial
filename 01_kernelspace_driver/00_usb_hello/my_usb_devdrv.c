#include <linux/module.h>
#include <linux/init.h>
#include <linux/usb.h>

/* Meta Information */
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Dufflepod");
MODULE_DESCRIPTION("Driver for SOS emergency button Atmega32U4 USB device");
MODULE_INFO(intree, "Y")

#define VENDOR_ID 	0x8477
#define PRODUCT_ID 	0x3118

static struct usb_device_id usb_dev_table [] = {
	{ USB_DEVICE(VENDOR_ID, PRODUCT_ID) },
	{},
};
MODULE_DEVICE_TABLE(usb, usb_dev_table);

static int my_usb_probe(struct usb_interface *intf, const struct usb_device_id *id) {
	printk("sos_driver - Probe Function\n");
	return 0;
}

static void my_usb_disconnect(struct usb_interface *intf) {
	printk("sos_driver - Disconnect Function\n");
}

static struct usb_driver my_usb_driver = {
	.name = "sos_driver",
	.id_table = usb_dev_table,
	.probe = my_usb_probe,
	.disconnect = my_usb_disconnect,
};

/**
 * @brief This function is called, when the module is loaded into the kernel
 */
static int __init my_init(void) {
	int result;
	printk("my_usb_devdrv - Init Function\n");
	result = usb_register(&my_usb_driver);
	if(result) {
		printk("my_usb_devdrv - Error during register!\n");
		return -result;
	}

	return 0;
}

/**
 * @brief This function is called, when the module is removed from the kernel
 */
static void __exit my_exit(void) {
	printk("sos_driver - Exit Function\n");
	usb_deregister(&my_usb_driver);
}

module_init(my_init);
module_exit(my_exit);


