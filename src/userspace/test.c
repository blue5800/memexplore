#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>

// Define the same structures and macros as in the kernel
#define MY_IOCTL_MAGIC 'k'
#define DEVICE_NAME "/dev/suspicious_device"  // Note: device files are typically in /dev

struct mem_args {
    pid_t pid;
    unsigned long long addr;
    unsigned long size;
    char *data;
};

#define MY_IOCTL_READ _IOWR(MY_IOCTL_MAGIC, 1, struct mem_args)
#define MY_IOCTL_WRITE _IOWR(MY_IOCTL_MAGIC, 2, struct mem_args)

int main() {
    int fd;
    struct mem_args args;
    int ret;
    char buf[1024];
    memset(buf, 'A', 1023);
    buf[1023] = '\0';
    // Open the device file
    fd = open(DEVICE_NAME, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return EXIT_FAILURE;
    }
    
    // Initialize the arguments for reading
    memset(&args, 0, sizeof(args));
    args.pid = getpid();  // Set to the current process ID
    args.addr = (unsigned long long) buf;    // Set to the address to read from
    args.size = 1024;  // Set to the number of bytes to read
    args.data = NULL;  // NULL for reads
    
    // Perform the read operation
    ret = ioctl(fd, MY_IOCTL_READ, &args);
    if (ret < 0) {
        perror("IOCTL read failed");
        close(fd);
        return EXIT_FAILURE;
    }
    
    printf("Successfully performed read operation\n");
    
    // Close the device file
    close(fd);
    
    return EXIT_SUCCESS;
}
