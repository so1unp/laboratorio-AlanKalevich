#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <unistd.h>
#include <signal.h>
#include <sys/resource.h>

static int cantHijos;
static int pidHijos[100];

struct sigaction signalAction;

int busywork(void) {
    struct tms buf;
    for (;;) {
        times(&buf);
    }
}

void showUsage() {
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    printf("Child %d (nice %2d):\t%3li\n", getpid(), getpriority(PRIO_PROCESS, 0), usage.ru_utime.tv_sec + usage.ru_stime.tv_sec);
    exit(EXIT_SUCCESS);
}

void fatherTerminated() {
    int i;
    for (i = 0; i < cantHijos; i++) {
        int status = kill(pidHijos[i], 15);
        if (status < 0) {
            perror("kill: ");
            exit(EXIT_FAILURE);
        }
    }
    exit(EXIT_SUCCESS);
}

int main(int argc, char* argv[])
{
    if (argc != 4) {
        fprintf(stderr, "Cantidad de parametros invalidos.\n"); 
        fprintf(stderr, "Formato: forkprio [cant de hijos] [cant de segundos] [prioridad si ó no (0 ó 1)].\n");
        exit(EXIT_FAILURE);
    }

    if (atoi(argv[1]) < 0 || atoi(argv[2]) < 0 || (atoi(argv[3]) != 0 && atoi(argv[3]) != 1)) {
        fprintf(stderr, "Cantidad de parametros invalidos.\n");
        fprintf(stderr, "Formato: forkprio [cant de hijos] [cant de segundos] [prioridad si ó no (0 ó 1)].\n");
        exit(EXIT_FAILURE);
    }

    cantHijos = atoi(argv[1]);
    int i;
    pid_t pid = getpid();

    for (i = 0; i < cantHijos; i++)
        pidHijos[i] = 0;

    i = 0;

    while (i < cantHijos && pid != 0) {
        pid = fork();
        if (pid != 0) {
            pidHijos[i] = pid;
            if (atoi(argv[3]) == 1)
                setpriority(PRIO_PROCESS, (id_t)pid, i);
        }
        i++;
    }

    if (pid == 0) {
        signalAction.sa_handler = showUsage;
        sigaction(SIGTERM, &signalAction, NULL);

        signalAction.sa_handler = SIG_IGN;
        sigaction(SIGINT, &signalAction, NULL);

        busywork();
    }
    else {
        if (atoi(argv[2]) == 0) {
            signalAction.sa_handler = fatherTerminated;
            sigaction(SIGINT, &signalAction, NULL);

            pause();
        }
        else {
            unsigned int time = (unsigned int)atoi(argv[2]);
            sleep(time);
            fatherTerminated();
        }
    }
    exit(EXIT_SUCCESS);
}
