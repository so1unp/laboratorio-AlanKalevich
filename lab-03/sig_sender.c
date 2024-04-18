#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Error, formato: sig_sender signum pid\n");
        exit(EXIT_FAILURE);
    }

    int signal = atoi(argv[1]);
    int pid = atoi(argv[2]);

    if(signal == 0 || pid == 0){
        printf("Error, formato: sig_sender signum pid\n");
        exit(EXIT_FAILURE);
    }

    int status = kill(pid, signal);
    if (status < 0) {
        perror("kill");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
