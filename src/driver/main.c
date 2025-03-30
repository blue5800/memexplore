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
#include "Ioctl_interface.h"
#define DEVICE_NAME "suspicious_device"
#define CLASS_NAME "suspicious_class"
static int MAJOR;
static int MINOR = 0;
const struct class *c1;
static struct cdev c_dev;



struct mem_args *args = NULL;
#define SIZE sizeof(struct mem_args)



//todo: this after figuring out ioctl arguments
static ssize_t read_mem (struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
	if (!args){
		printk(KERN_ALERT "No arguments\n");
		return -1;
	}
	pid_t pid = args->pid;
	unsigned long long addr = args->addr;
	unsigned long size = args->size;

	if (size <= 0 || count < size){
		printk(KERN_ALERT "Invalid size\n");
		return -1;
	}

	struct task_struct *task = pid_task(find_vpid(pid), PIDTYPE_PID);
	if (!task){
		printk(KERN_ALERT "Failed to find task\n");
		return -1;
	}
	
	ssize_t bytes_read = access_process_vm(task, addr, buf, size, 0);
	return bytes_read;
}

static ssize_t write_mem (struct file *file, const char __user *buf, size_t count, loff_t *ppos)
{
	//ssize_t bytes_written = access_process_vm(task, addr, data, size, 1);
	return 0;
}


//define open and close functions

static int device_open(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "Device opened\n");
    return 0;
}

static int device_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "Device closed\n");
    return 0;
}

static long device_ioctl(struct file *file, unsigned int ioctl_num, unsigned long ioctl_param)
{
	if (args){
		kfree(args);
	}

	args = kmalloc(SIZE, GFP_KERNEL);
	if (!args){
		printk(KERN_ALERT "Failed to allocate memory\n");
		return -1;
	}

	switch (ioctl_num){
		case MY_IOCTL_READ:
			printk(KERN_INFO "ioctl read\n");
			if(copy_from_user(args, (void __user *)ioctl_param, SIZE)){
			printk(KERN_INFO "Failed to copy from user\n");
			return -1;
			}
			printk(KERN_INFO "new data. got pid:%d, vma:0x%llx,size:%lu,data ptr:0x%llx\n", args->pid, args->addr,args->size, (unsigned long long) args->data);

			pid_t pid = args->pid;
			unsigned long long addr = args->addr;
			unsigned long size = args->size;
			char *data = args->data;
			
			struct task_struct *task = pid_task(find_vpid(pid), PIDTYPE_PID);
			if (!task){
				printk(KERN_ALERT "Failed to find task\n");
				return -1;
			}
			void *tmp = kmalloc(size, GFP_KERNEL);
			if (!tmp){
				printk(KERN_ALERT "Failed to allocate memory for temp buffer\n");
				return -1;
			}
			long bytes_read = access_process_vm(task, addr, tmp, size, 0);
			if (bytes_read < 0){
				printk(KERN_ALERT "Failed to read from process\n");
				kfree(tmp);
				return -1;
			}
			if (copy_to_user(data, tmp, size)){
				printk(KERN_ALERT "Failed to copy to user\n");
				kfree(tmp);
				return -1;
			}
			kfree(tmp);
			return bytes_read;

			break;
		case MY_IOCTL_WRITE:
			printk(KERN_INFO "ioctl write\n");

			break;
		default:
			printk(KERN_INFO "Invalid ioctl\n");
			return -1;
	}
	return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = read_mem,
    .write = write_mem,
    .unlocked_ioctl = device_ioctl,
    .open = device_open,
    .release = device_release
};

static int __init mod_init(void);
static int __init mod_init(void)
{
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
