#pragma once
#define NO_DRIVER -8
int mem_read(int target_pid, void *base_address, unsigned long num_bytes, void *destination_buffer);
int mem_write(int target_pid, void *base_address, unsigned long num_bytes, void *source_buffer);

