#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <string.h>
#include "lib/memexplore.h"
// Define the same structures and macros as in the kernel

int main() {
    int ret;
    char buf[1024];
    memset(buf, 'A', 1023);
    buf[1023] = '\0';
    char buf2[1024];
    memset(buf2, '\0', 1023);
    strcpy(buf2, "hi, we're pre read rn.");
    printf("buffer pre read: %s\n", buf2);
    int pid = getpid();
    ret = mem_read(pid, buf, 1024, buf2);
    if (ret < 0) {
        printf("Failed to read\n");
        if (ret == NO_DRIVER) {
            printf("Driver not loaded\n");
        }
        return EXIT_FAILURE;
    }
    
    printf("Successfully did read\n");
    printf("Data read: %s\n", buf2);
    
    printf("-----------------\n");
    printf("now writing to buffer\n");

    char buf3[1024];
    memset(buf3, 'e', 1023);
    strcpy(buf3, "octopus");
    printf("buffer pre write: %s\n", buf);
    ret = mem_write(pid, buf, 1024, buf3);
    if (ret < 0) {
        printf("Failed to write\n");
        if (ret == NO_DRIVER) {
            printf("Driver not loaded\n");
        }
        return EXIT_FAILURE;
    }
    printf("Successfully did write\n");
    printf("Data written: %s\n", buf);

    // Close the device file
    int *p = malloc(4);
    p[0] = 20;
    p[1] = 30;
    p[2] = 40;
    p[3] = 50;
    printf("Data pre write: %d\n", p[2]);
    int replace = 100;
    ret = mem_write(pid, (&p[2]), 4, &replace);
    if (ret < 0) {
        printf("Failed to write\n");
        if (ret == NO_DRIVER) {
            printf("Driver not loaded\n");
        }
        return EXIT_FAILURE;
    }
    printf("Successfully did write\n");

    printf("Data written: %d\n", p[2]);
    return EXIT_SUCCESS;
}
