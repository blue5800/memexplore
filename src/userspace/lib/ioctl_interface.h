#pragma once
char device_name[] = "suspicious_device";

struct mem_args{
	int pid;
	unsigned long long addr;
	unsigned long size;
	char *data;
};
#define MY_IOCTL_MAGIC 'k'
#define MY_IOCTL_READ _IOWR(MY_IOCTL_MAGIC, 1, struct mem_args)
#define MY_IOCTL_WRITE _IOWR(MY_IOCTL_MAGIC, 2, struct mem_args)
