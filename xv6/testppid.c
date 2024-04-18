#include "types.h"
#include "user.h"
#include "date.h"

int main(int argc, char* argv[])
{
    int pidPadre = getpid();
    int pidHijo = fork();

    if (pidHijo == 0) {
        printf(1, "Pid del padre, por el hijo: %d\n", getppid());
        printf(1, "Pid del hijo, por el hijo: %d\n", getpid());
    } else {
        wait();
        printf(1, "\nPid del padre, por el padre: %d\n", pidPadre);
        printf(1, "Pid del hijo, por el padre: %d\n", pidHijo);
    }
    exit();
}
