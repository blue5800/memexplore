#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/sched.h>
#include <linux/pid.h>
#include <linux/mm.h>
//expose character device
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/uaccess.h>
#include "ioctl_interface.h"
#include "read_write.h"
#define DEVICE_NAME "suspicious_device"
#define CLASS_NAME "suspicious_class"
static int MAJOR;
static int MINOR = 0;
const struct class *c1;
static struct cdev c_dev;

//todo: i'm not sure yet actually. i don't really know what to do with the read and write functions.
//suggestions? they're here because i was gonna use them but ended up going with ioctl because it does arguments nicer...
static ssize_t read_mem(struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
	return 0;
}

static ssize_t write_mem(struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	return 0;
}

//this needs to exist for ioctl because that requires a file descriptor.
static int device_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "Device opened\n");
    return 0;
}
//if we open we need to close.
static int device_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "Device closed\n");
    return 0;
}

//handle ioctl calls
static long device_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_param)
{

	switch (ioctl_num){
		case MY_IOCTL_READ:
			return read_copy_to_user((void __user *)ioctl_param);
			break;
		case MY_IOCTL_WRITE:
			return write_copy_from_user((void __user *)ioctl_param);
			break;
		default:
			printk(KERN_INFO "Invalid ioctl\n");
			return -1;
	}
	return 0;
}

//yup 
static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = read_mem,
    .write = write_mem,
    .unlocked_ioctl = device_ioctl,
    .open = device_open,
    .release = device_release
};

//i'm not sure why the kernel makes me double define but whatever
static int __init mod_init(void);
static int __init mod_init(void)
{
	printk(KERN_INFO "Initializing bad module\n");
    MAJOR = register_chrdev(0, DEVICE_NAME, &fops);
    if (MAJOR < 0){
    	printk(KERN_ALERT "Failed to register a major number\n");
    	return MAJOR;
    }

    cdev_init(&c_dev, &fops);
    if (cdev_add(&c_dev, MAJOR, 1) < 0){
    	printk(KERN_ALERT "Failed to add a char device\n");
    	return -1;
    }
    c1 = class_create(CLASS_NAME);
    device_create(c1, NULL, MKDEV(MAJOR, MINOR), NULL, DEVICE_NAME);

    printk(KERN_INFO "Registered with major number %d\n", MAJOR);

    return 0;
}
//same thing
void __exit mod_exit(void);
void __exit mod_exit(void)
{
    device_destroy(c1, MKDEV(MAJOR, MINOR));
    unregister_chrdev(MAJOR, DEVICE_NAME);
    printk(KERN_INFO "Unregistered\n");
    return;
}

module_init(mod_init);
module_exit(mod_exit);

MODULE_LICENSE("GPL");
