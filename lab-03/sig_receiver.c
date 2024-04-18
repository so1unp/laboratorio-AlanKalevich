#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

void myHandler(int signum) {

    printf("Señal recibida: %i, descripción: %s\n", signum, strsignal(signum));

}

int main(void) {

    printf("PID de mi prog: %i\n", getpid());

    int sigNumber;
    for (sigNumber = 1; sigNumber < 255; sigNumber++)
        signal(sigNumber, myHandler);

    while (1) 
        pause();
    
    exit(EXIT_SUCCESS);
}
