#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>
#include <string.h>


#define HEIGHT  25  // Altura en caracteres de la pizarra
#define WIDTH   25  // Ancho en caracteres de la pizarra
#define PERMISSIONS 0622

struct canvas {
    char canvas[HEIGHT * WIDTH];
};

typedef struct canvas canvas_t;

void usage(char* argv[])
{
    fprintf(stderr, "Uso: %s comando parametro\n", argv[0]);
    fprintf(stderr, "Comandos:\n");
    fprintf(stderr, "\t-w canvas mensaje x y [h|v]: escribe el mensaje en el canvas indicado en la posición (x,y) de manera [h]orizontal o [v]ertical.\n");
    fprintf(stderr, "\t-p canvas: imprime el canvas indicado.\n");
    fprintf(stderr, "\t-c canvas: crea un canvas con el nombre indicado.\n");
    fprintf(stderr, "\t-d canvas: elimina el canvas indicado.\n");
    fprintf(stderr, "\t-h imprime este mensaje.\n");
}

void createCanvas(char* name) {
    int shm_fd = shm_open(name, O_CREAT | O_RDWR, PERMISSIONS);
    if (shm_fd < 0) {
        perror("shm_fd");
        exit(EXIT_FAILURE);
    }
    int status = ftruncate(shm_fd, sizeof(canvas_t));
    if (status < 0) {
        perror("status");
        exit(EXIT_FAILURE);
    }
    close(shm_fd);
}

void deleteCanvas(char* canvasName) {
    int status = shm_unlink(canvasName);
    if (status < 0) {
        perror("deleteCanvas status");
        exit(EXIT_FAILURE);
    }
}

void printCanvas(char* canvasName) {
    int shm_fd = shm_open(canvasName, O_RDONLY, PERMISSIONS);
    if (shm_fd < 0) {
        perror("shm_fd");
        exit(EXIT_FAILURE);
    }

    canvas_t* myMap = (canvas_t*)mmap(NULL, sizeof(canvas_t), PROT_READ, MAP_SHARED, shm_fd, 0);

    int i;
    int j;
    for (i = 0; i < HEIGHT; i++) {
        for (j = 0; j < WIDTH; j++) {
            // printf("%c", myMap->canvas[i]);
            if (myMap->canvas[HEIGHT * i + j] == '\0')
                putchar('_');
            else
                putchar(myMap->canvas[HEIGHT * i + j]);
        }
        printf("\n");
    }
    close(shm_fd);
}

void addToCanvas(char* input, char* canvasName, int x, int y) {
    int shm_fd = shm_open(canvasName, O_RDWR, PERMISSIONS);
    if (shm_fd < 0) {
        perror("shm_fd");
        exit(EXIT_FAILURE);
    }

    canvas_t* myMap = (canvas_t*)mmap(NULL, sizeof(canvas_t), PROT_WRITE, MAP_SHARED, shm_fd, 0);
    int i;
    for (i = 0; i < strlen(input); i++) {
        myMap->canvas[(((HEIGHT * x) + y) + i) % (HEIGHT*WIDTH)] = input[i];
    }
    close(shm_fd);
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
        printf("Escribe %s en el canvas %s en la posición (%d, %d).\n", argv[3], argv[2], atoi(argv[4]), atoi(argv[5]));
        addToCanvas(argv[3], argv[2], atoi(argv[4]), atoi(argv[5]));
        break;
    case 'p':
        printf("Imprime canvas.\n");
        printCanvas(argv[2]);
        break;
    case 'c':
        printf("Crea canvas.\n");
        createCanvas(argv[2]);
        break;
    case 'd':
        printf("Borra canvas.\n");
        deleteCanvas(argv[2]);
        break;
    case 'h':
        usage(argv);
        break;
    default:
        fprintf(stderr, "Comando desconocido\n");
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
