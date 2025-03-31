#include <stdio.h>
#include <unistd.h>
void nice_function() {
    printf("Hello from nice_function!\n");
}

void evil_function() {
    printf("YOUR COMPUTER IS NOW MINE!\n");
}

int main() {
    printf("running with pid %d\n", getpid());
    printf("nice function lives at %p\n", nice_function);
    printf("evil function lives at %p\n", evil_function);
    void (*nice_ptr)(void) = nice_function;
    int should_run = 1;
    printf("nice_ptr points to: %p\n", nice_ptr);
    
    printf("nice_ptr address: %p\n", &nice_ptr);
    printf("should_run address: %p\n", &should_run);

    while (should_run){
        nice_ptr();
        sleep(1);
    }
    printf("how did you even get here??? impossible!!\n");
    return 0;
}
