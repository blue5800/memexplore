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

#define DEVICE_NAME "suspicious_device"
#define CLASS_NAME "suspicious_class"
static int MAJOR;
static int MINOR = 0;
const struct class *c1;
static struct cdev c_dev;
static ssize_t read_mem (struct file *file, char __user *buf, size_t count, loff_t *ppos)
{
    //we expect the buffer to contain 3 integral types.
    //1: pid
    //2: virtual address
    //3: number of bytes to read
    int pid;
    unsigned long addr;
    int size;
    if (count < sizeof(int) + sizeof(unsigned long) + sizeof(int)){printk(KERN_ALERT "Buffer too small\n");return -EINVAL;}
    if (copy_from_user(&pid, buf, sizeof(int))){printk(KERN_ALERT "Failed to read pid\n");return -EFAULT;}
    if (copy_from_user(&addr, buf + sizeof(int), sizeof(unsigned long))){printk(KERN_ALERT "Failed to read address\n");return -EFAULT;}
    if (copy_from_user(&size, buf + sizeof(int) + sizeof(unsigned long), sizeof(int))){printk(KERN_ALERT "Failed to read size\n");return -EFAULT;}
    
    if (pid < 0 || size < 0 || count < size){printk(KERN_ALERT "something wrong. pid: %d, size: %d, count: %lu \n",pid,size,count);return -EINVAL;}
    printk("args: pid: %d, addr: %lu, size: %d\n",pid,addr,size);
    struct task_struct *task;
    task = pid_task(find_vpid(pid), PIDTYPE_PID);

    if (task == NULL){printk(KERN_ALERT "Failed to find task\n");return -EINVAL;}
    
    void *data = kmalloc(size, GFP_KERNEL);

    if (data == NULL){printk(KERN_ALERT "Failed to allocate memory\n");kfree(data);return -ENOMEM;}

    ssize_t bytes_read = access_process_vm(task, addr, data, size, 0);

    if (bytes_read < 0){printk(KERN_ALERT "Failed to read memory\n");kfree(data);return -EFAULT;}
    
    if (copy_to_user(buf, data, bytes_read)){printk(KERN_ALERT "Failed to copy data to user\n");kfree(data);return -EFAULT;}
    kfree(data);
    return 0;
}

static struct file_operations fops = {
    .owner = THIS_MODULE,
    .read = read_mem,
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
