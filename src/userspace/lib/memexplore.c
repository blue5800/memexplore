#include "ioctl_interface.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <stdlib.h>

int do_mem_op(int target_pid, unsigned long long base_address, unsigned long num_bytes, void *buffer, int write){
    int fd = open(device_name, O_RDWR);
    if (fd < 0) {
        return EXIT_FAILURE;
    }
    struct mem_args args;
    args.pid = target_pid;
    args.addr = base_address;
    args.size = num_bytes;
    args.data = buffer;
    int ret = ioctl(fd, write ? MY_IOCTL_WRITE : MY_IOCTL_READ, &args);
    close(fd);
    return ret;
}

int mem_read(int target_pid, unsigned long long base_address, unsigned long num_bytes, void *destination_buffer){
    return do_mem_op(target_pid, base_address, num_bytes, destination_buffer, 0);
}
int mem_write(int target_pid, unsigned long long base_address, unsigned long num_bytes, void *source_buffer){
    return do_mem_op(target_pid, base_address, num_bytes, source_buffer, 1);
}
