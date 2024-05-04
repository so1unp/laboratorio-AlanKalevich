#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// Variable global que incrementan los hilos.
static long glob = 0;

void increment_glob(int loops)
{
    int loc, j;

    // incrementa glob
    for (j = 0; j < loops; j++) {
        loc = glob;
        loc++;
        glob = loc;
    }
}

void *hilo(void *p)
{
    long num = (long)p;
    increment_glob(num);
    pthread_exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[])
{
    long loops;

    // Controla numero de argumentos.
    if (argc != 2)
    {
        fprintf(stderr, "Uso: %s ciclos\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    loops = atoi(argv[1]);

    // Verifica argumentos.
    if (loops < 1)
    {
        fprintf(stderr, "Error: ciclos debe ser mayor a cero.\n");
        exit(EXIT_FAILURE);
    }

    pthread_t h1;
    pthread_create(&h1, NULL, hilo, (void *)loops);

    pthread_t h2;
    pthread_create(&h2, NULL, hilo, (void *)loops);

    pthread_join(h1, NULL);
    pthread_join(h2, NULL);

    printf("%ld\n", glob);

    exit(EXIT_SUCCESS);
}