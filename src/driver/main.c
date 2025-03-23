#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/printk.h>

//expose character device
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>

#define DEVICE_NAME "suspicious_device"
#define CLASS_NAME "suspicious_class"

static int __init mod_init(void);
static int __init mod_init(void)
{
    return 0;
}
void __exit mod_exit(void);
void __exit mod_exit(void)
{
    return;
}

module_init(mod_init);
module_exit(mod_exit);

MODULE_LICENSE("GPL");
