#include "types.h"
#include "user.h"
#include "date.h"

int main(int argc, char* argv[]) {

    printf(1, "Cantidad de procesos: %d\n", pscnt());

    int pid = fork();

    if (pid == 0) {
        printf(1, "Cantidad de procesos despues de crear 1 hijo: %d\n", pscnt());
    }
    wait();
    exit();
}