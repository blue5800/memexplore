#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/printk.h>
#include <linux/sched.h>
#include <linux/pid.h>
#include <linux/mm.h>
#include <linux/fs.h>
#include <linux/cdev.h>

#include "Ioctl_interface.h"

ssize_t read_copy_to_user(void __user *mem_args);
ssize_t write_copy_from_user(void __user *mem_args);
