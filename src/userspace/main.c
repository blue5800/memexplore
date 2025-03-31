#include "lib/memexplore.h"
#include <stdio.h>
#include <stdlib.h>

//simple terminal program to test the memory explorer
//asks for pid first. then asks for if it wants to write a pointer or integer (8b vs 4b)
//then asks for the address to write to. then asks for the value to write.

int main(void){
    int pid;
    unsigned long long addr;
    unsigned long long pvalue;
    unsigned int ivalue;
    int res = 0;
    int choice;

    printf("Enter the pid of the process you want to explore: ");
    scanf("%d", &pid);
    printf("PID: %d\n", pid);
start:
    printf("Enter 1 to write to a pointer, 2 to write to an integer: ");
    scanf("%d", &choice);
    if(choice == 1){
        printf("Enter the address to write to: ");
        scanf("%llx", &addr);
        printf("Enter the value to write: ");
        scanf("%llx", &pvalue);
        res = mem_write(pid,(void *)addr,sizeof(unsigned long long),&pvalue);
        if (res == 0){
            printf("Write successful\n");
        } else {
            printf("Write failed\n");
        }
    } else if(choice == 2){
        printf("Enter the address to write to: ");
        scanf("%llx", &addr);
        printf("Enter the value to write: ");
        scanf("%x", &ivalue);
        res = mem_write(pid,(void *)addr,sizeof(unsigned int),&ivalue);
        if (res == 0){
            printf("Write successful\n");
        } else {
            printf("Write failed\n");
        }
    } else {
        printf("exit\n");
        return 0;
    }
    goto start;
    return 0;
}
