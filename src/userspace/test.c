#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>

// Structure of the input buffer: [pid (int), addr (unsigned long), size (int)]
struct mem_request {
    int pid;
    unsigned long addr;
    int size;
};

int main(void) {
    int fd;
    ssize_t ret;
    char *dev = "/dev/suspicious_device";

    // Open the device
    fd = open(dev, O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return EXIT_FAILURE;
    }

    // For testing, we will try to read from our own process memory.
    // Let’s create a local buffer and store some data in it.
    char *test_data = calloc(1,32768);
    printf("test_data: %lld\n", (long long)test_data);
    test_data[0] = 'H';
    test_data[1] = 'e';
    test_data[2] = 'l';
    test_data[3] = 'l';
    int data_size = 32768;
    
    // Prepare the mem_request structure.
    // We use our own PID and the address of our test_data buffer.
    struct mem_request req;
    req.pid = getpid();
    req.addr = (unsigned long)test_data;
    req.size = 4;

    // The device's read method expects the input buffer to contain the parameters,
    // and then it will return the read bytes in the same buffer.
    // We need to allocate a buffer large enough to hold both the parameters and the read data.
    size_t param_size = sizeof(req);
    size_t total_size = (param_size > data_size ? param_size : data_size);
    char *buffer = malloc(total_size);
    if (!buffer) {
        perror("malloc");
        close(fd);
        return EXIT_FAILURE;
    }

    // Copy the parameters into the beginning of the buffer.
    memcpy(buffer, &req, param_size);

    // Now, call read. The driver's read function is expected to:
    // 1. extract the parameters from the buffer,
    // 2. read data from the target process memory,
    // 3. and copy the data back into the buffer.
    ret = read(fd, buffer, total_size);
    if (ret < 0) {
        perror("Device read failed");
        free(buffer);
        close(fd);
        return EXIT_FAILURE;
    } else {
        printf("Device read returned %zd bytes\n", ret);
    }

    // Print out the data received.
    // This should be the data at the provided address, i.e. the content of test_data.
    // We make sure to add a null terminator before printing.
    char *received_data = malloc(ret + 1);
    if (!received_data) {
        perror("malloc");
        free(buffer);
        close(fd);
        return EXIT_FAILURE;
    }
    memcpy(received_data, buffer, ret);
    received_data[ret] = '\0';
    printf("Read data: %s\n", received_data);

    free(received_data);
    free(buffer);
    close(fd);
    return EXIT_SUCCESS;
}

