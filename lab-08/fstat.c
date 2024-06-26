#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>


void print_file_type(mode_t mode)
{
    switch (mode & S_IFMT) {
        case S_IFREG:
            printf("Tipo de archivo: Archivo regular\n");
            break;
        case S_IFDIR:
            printf("Tipo de archivo: Directorio\n");
            break;
        case S_IFCHR:
            printf("Tipo de archivo: Dispositivo de caracteres\n");
            break;
        case S_IFBLK:
            printf("Tipo de archivo: Dispositivo de bloques\n");
            break;
        case S_IFIFO:
            printf("Tipo de archivo: FIFO (tubería con nombre)\n");
            break;
        case S_IFLNK:
            printf("Tipo de archivo: Enlace simbólico\n");
            break;
        case S_IFSOCK:
            printf("Tipo de archivo: Socket\n");
            break;
        default:
            printf("Tipo de archivo: Desconocido\n");
            break;
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Cantidad de parametros incorrecta\n");
        exit(EXIT_FAILURE);
    }
    struct stat file_stat;
    if (stat(argv[1], &file_stat) == -1)
    {
        perror("stat");
        exit(EXIT_FAILURE);
    }

    printf("ID del usuario propietario: %u\n", file_stat.st_uid);
    print_file_type(file_stat.st_mode);
    printf("Número de i-nodo: %lu\n", file_stat.st_ino);
    printf("Tamaño en bytes: %ld\n", file_stat.st_size);
    printf("Número de bloques que ocupa en disco: %ld\n", file_stat.st_blocks);
    printf("Tiempo de última modificación: %s", ctime(&file_stat.st_mtime));
    printf("Tiempo de último acceso: %s", ctime(&file_stat.st_atime));

    exit(EXIT_SUCCESS);
}
