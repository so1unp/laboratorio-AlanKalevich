#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>
#include <string.h>
#include <semaphore.h>
#include <pthread.h>

#define ITEMS       15
#define MAX_WORD    50
#define PERMISSIONS 0666

struct wordstack {
    int free;
    int items;
    int max_word;
    pthread_mutex_t mutex;
    sem_t full;
    sem_t empty;
    char heap[ITEMS][MAX_WORD];
};

typedef struct wordstack wordstack_t;

void usage(char* argv[])
{
    fprintf(stderr, "Uso: %s comando parametro\n", argv[0]);
    fprintf(stderr, "Comandos:\n");
    fprintf(stderr, "\t-w pila palabra: agrega palabra en la pila\n");
    fprintf(stderr, "\t-r pila pos: elimina la palabra de la pila.\n");
    fprintf(stderr, "\t-p pila: imprime la pila de palabras.\n");
    fprintf(stderr, "\t-c pila: crea una zona de memoria compartida con el nombre indicado donde almacena la pila.\n");
    fprintf(stderr, "\t-d pila: elimina la pila indicada.\n");
    fprintf(stderr, "\t-h imprime este mensaje.\n");
}

void pushWord(char* stackName, char* input) {

    if (strlen(input) > MAX_WORD) {
        printf("input length greater than MAX_WORD. Input length: %d, MAX_WORD: %d\n", (int)strlen(input), MAX_WORD);
        exit(EXIT_FAILURE);
    }

    int shm_fd = shm_open(stackName, O_RDWR, PERMISSIONS);
    if (shm_fd < 0) {
        perror("shm_fd");
        exit(EXIT_FAILURE);
    }
    wordstack_t* myMap = (wordstack_t*)mmap(NULL, sizeof(wordstack_t), PROT_WRITE, MAP_SHARED, shm_fd, 0);

    int i;
    sem_wait(&(myMap->empty));
    sem_post(&(myMap->full));
    pthread_mutex_lock(&(myMap->mutex));
    for (i = 0; i < strlen(input); i++) {
        myMap->heap[myMap->free][i] = input[i];
    }
    pthread_mutex_unlock(&(myMap->mutex));
    myMap->free++;
    myMap->items++;


    close(shm_fd);
}

void popWord(char* stackName) {
    int shm_fd = shm_open(stackName, O_RDWR, PERMISSIONS);
    if (shm_fd < 0) {
        perror("shm_fd");
        exit(EXIT_FAILURE);
    }
    wordstack_t* myMap = (wordstack_t*)mmap(NULL, sizeof(wordstack_t), PROT_WRITE, MAP_SHARED, shm_fd, 0);

    int i;
    sem_wait(&(myMap->full));
    sem_post(&(myMap->empty));
    myMap->items--;
    myMap->free--;
    pthread_mutex_lock(&(myMap->mutex));
    for (i = 0; i < MAX_WORD; i++) {
        myMap->heap[myMap->free][i] = '\0';
    }
    pthread_mutex_unlock(&(myMap->mutex));
    close(shm_fd);
}

void printAllWords(char* stackName) {
    int shm_fd = shm_open(stackName, O_RDONLY, PERMISSIONS);
    if (shm_fd < 0) {
        perror("shm_fd");
        exit(EXIT_FAILURE);
    }

    wordstack_t* myMap = (wordstack_t*)mmap(NULL, sizeof(wordstack_t), PROT_READ, MAP_SHARED, shm_fd, 0);

    int i;
    int j;
    for (i = ITEMS; i >= 0; i--) {
        for (j = 0; j < MAX_WORD; j++) {
            if (myMap->heap[i][j] == '\0')
                putchar('_');
            else
                putchar(myMap->heap[i][j]);
        }
        printf("\n");
    }
    close(shm_fd);
}

void createStack(char* stackName) {
    umask(0000);
    int shm_fd = shm_open(stackName, O_CREAT | O_RDWR, PERMISSIONS);
    if (shm_fd < 0) {
        perror("createStack shm_open shm_fd");
        exit(EXIT_FAILURE);
    }
    int status = ftruncate(shm_fd, sizeof(wordstack_t));
    if (status < 0) {
        perror("createStack ftruncate status");
        exit(EXIT_FAILURE);
    }
    wordstack_t* myMap = (wordstack_t*)mmap(NULL, sizeof(wordstack_t), PROT_WRITE, MAP_SHARED, shm_fd, 0);
    myMap->free = 0;
    myMap->items = 0;
    sem_init(&(myMap->empty), 1, ITEMS);
    sem_init(&(myMap->full), 1, 0);
    pthread_mutex_init(&(myMap->mutex), NULL);

    close(shm_fd);
}

void deleteStack(char* stackName) {
    int status = shm_unlink(stackName);
    if (status < 0) {
        perror("deleteStack status");
        exit(EXIT_FAILURE);
    }
}


int main(int argc, char* argv[])
{
    if (argc < 2) {
        usage(argv);
        exit(EXIT_FAILURE);
    }

    if (argv[1][0] != '-') {
        usage(argv);
        exit(EXIT_FAILURE);
    }

    char option = argv[1][1];

    switch (option) {
    case 'w':
        printf("Agregando %s a la pila %s.\n", argv[3], argv[2]);
        pushWord(argv[2], argv[3]);
        break;
    case 'r':
        printf("Sacando un elemento de la pila %s.\n", argv[2]);
        popWord(argv[2]);
        break;
    case 'p':
        printf("Imprimiendo la pila %s.\n", argv[2]);
        printAllWords(argv[2]);
        break;
    case 'c':
        printf("Creando la pila %s.\n", argv[2]);
        createStack(argv[2]);
        break;
    case 'd':
        printf("Eliminando la pila %s.\n", argv[2]);
        deleteStack(argv[2]);
        break;
    case 'h':
        usage(argv);
        break;
    default:
        fprintf(stderr, "-%c: opción desconocida.\n", option);
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
