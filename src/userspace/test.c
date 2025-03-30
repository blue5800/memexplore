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

struct mem_args{
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
    char buf2[1024];
    memset(buf2, '\0', 1023);

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
    args.data = buf2;   //target buffer

    
    // Perform the read operation
    printf("buffer pre read: %s\n", buf2);

    ret = ioctl(fd, MY_IOCTL_READ, &args);
    if (ret < 0) {
        perror("IOCTL read failed");
        close(fd);
        return EXIT_FAILURE;
    }
    
    printf("Successfully did read\n");
    printf("Data read: %s\n", buf2);
    
    printf("-----------------\n");
    printf("now writing to buffer\n");
    char buf3[1024];
    memset(buf3, 'e', 1023);
    strcpy(buf3, "octopus");
    args.addr = (unsigned long long) buf;
    args.data = buf3;
    args.size = 1024;
    printf("buffer pre write: %s\n", buf);
    ret = ioctl(fd, MY_IOCTL_WRITE, &args);
    if (ret < 0) {
        perror("IOCTL write failed");
        close(fd);
        return EXIT_FAILURE;
    }
    printf("Successfully did write\n");
    printf("Data written: %s\n", buf);

    // Close the device file
    close(fd);
    
    return EXIT_SUCCESS;
}
