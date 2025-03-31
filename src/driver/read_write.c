#include "read_write.h"
#include "ioctl_interface.h"
ssize_t do_mem_op(void __user *mem_args, unsigned int flags);
ssize_t do_mem_op(void __user *mem_args, unsigned int flags)
{
    //yeah i know it's a lot of bullshit error checking but this is a kernel so yk.
    //process dies? no problem. kernel dies? bad one!
    struct mem_args *args;
    args = kmalloc(ARG_SIZE, GFP_KERNEL);
    if (!args){
        printk(KERN_ALERT "args buffer failed\n");
        return -1;
    }

    if(copy_from_user(args, (void __user *)mem_args, ARG_SIZE)){
        kfree(args);
        printk(KERN_ALERT "args copy failed\n");
        return -1;
    }
    pid_t pid = args->pid;
    unsigned long long addr = args->addr;
    unsigned long size = args->size;
    char *data = args->data;
    struct task_struct *task = pid_task(find_vpid(pid), PIDTYPE_PID);

    if (!task){
        printk(KERN_ALERT "couldn't find target process\n");
        kfree(args);
        return -1;
    }

    void *tmp = kmalloc(size, GFP_KERNEL);
    if (!tmp){
        printk(KERN_ALERT "couldn't alloc temp buffer\n");

        kfree(args);
        return -1;
    }

    if (flags & FOLL_WRITE){
        if (copy_from_user(tmp, data, size)){
            printk(KERN_ALERT "Failed to copy from user\n");
            kfree(args);
            kfree(tmp);
            return -1;
        }
    }

    long bytes_read = access_process_vm(task, addr, tmp, size, flags);
    //0 / positive value is success, negative value is error. keep this in mind unlike in the stupid userspace program where we got this wrong...
    if (bytes_read < 0){
        printk(KERN_ALERT "access_process_vm failed\n");
        kfree(tmp);
        kfree(args);
        return bytes_read;
    }

    if (!(flags & FOLL_WRITE)){
        if (copy_to_user(data, tmp, size)){
            printk(KERN_ALERT "Failed to copy to user\n");
            kfree(args);
            kfree(tmp);
            return -1;
        }
    }
    kfree(tmp);
    kfree(args);
    return bytes_read;
}
/*FOLL_FORCE get_user_pages read/write w/o permission */
// i don't know what this means but it sounds cool
ssize_t read_copy_to_user(void __user *mem_args)
{
    return do_mem_op(mem_args, FOLL_FORCE);
}
ssize_t write_copy_from_user(void __user *mem_args)
{
    return do_mem_op(mem_args, FOLL_WRITE | FOLL_FORCE);
}

MODULE_LICENSE("GPL");