#include <stdio.h>
#include <stdlib.h>

#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <mqueue.h>
#include <string.h>
#include <unistd.h>

#define USERNAME_MAXSIZE    15  // Máximo tamaño en caracteres del nombre del remitente.
#define TXT_SIZE            100 // Máximo tamaño del texto del mensaje.

/**
 * Estructura del mensaje:
 * - sender: nombre del usuario que envió el mensaje.
 * - text: texto del mensaje.
 */
struct msg {
    char sender[USERNAME_MAXSIZE];
    char text[TXT_SIZE];
};

typedef struct msg msg_t;

/**
 * Imprime información acerca del uso del programa.
 */
void usage(char* argv[])
{
    fprintf(stderr, "Uso: %s comando parametro\n", argv[0]);
    fprintf(stderr, "Comandos:\n");
    fprintf(stderr, "\t-s queue mensaje: escribe el mensaje en queue.\n");
    fprintf(stderr, "\t-r queue: imprime el primer mensaje en queue.\n");
    fprintf(stderr, "\t-a queue: imprime todos los mensaje en queue.\n");
    fprintf(stderr, "\t-l queue: vigila por mensajes en queue.\n");
    fprintf(stderr, "\t-c queue: crea una cola de mensaje queue.\n");
    fprintf(stderr, "\t-d queue: elimina la cola de mensajes queue.\n");
    fprintf(stderr, "\t-h imprime este mensaje.\n");
}

void createQueue(char* name, struct mq_attr attr) {
    umask(0000);
    mqd_t status = mq_open(name, O_CREAT, 0622, &attr);
    if (status < 0) {
        perror("status");
        exit(EXIT_FAILURE);
    }
}

void sendMsg(char* message, char* queue) {
    mqd_t fd = mq_open(queue, O_WRONLY);
    if (fd < 0) {
        perror("status");
        exit(EXIT_FAILURE);
    }

    msg_t msg;
    strncpy(msg.text, message, strlen(message) + 1);
    strncpy(msg.sender, "alan", strlen("alan"));

    mq_send(fd, (char*)&msg, sizeof(msg_t), 0);
}

void recieveMsg(char* queue) {
    mqd_t fd = mq_open(queue, O_RDONLY | O_NONBLOCK);
    if (fd < 0) {
        perror("status");
        exit(EXIT_FAILURE);
    }
    struct mq_attr attr;
    mq_getattr(fd, &attr);
    msg_t msgReceived;
    int status = mq_receive(fd, (char*)&msgReceived, attr.mq_msgsize, NULL);
    if (status < 0) {
        printf("No hay mensajes en la cola\n");
        exit(EXIT_SUCCESS);
    }
    printf("%s\n", msgReceived.text);
}

void recieveMsgUndefined(char* queue) {
    mqd_t fd = mq_open(queue, O_RDONLY);
    if (fd < 0) {
        perror("status");
        exit(EXIT_FAILURE);
    }
    struct mq_attr attr;
    mq_getattr(fd, &attr);
    msg_t msgReceived;
    mq_receive(fd, (char*)&msgReceived, attr.mq_msgsize, NULL);
    printf("%s\n", msgReceived.text);
}

void recieveAllMsg(char* queue) {
    mqd_t fd = mq_open(queue, O_RDONLY | O_NONBLOCK);
    if (fd < 0) {
        perror("status");
        exit(EXIT_FAILURE);
    }
    struct mq_attr attr;
    mq_getattr(fd, &attr);

    msg_t msgReceived;
    int status = mq_receive(fd, (char*)&msgReceived, attr.mq_msgsize, NULL);
    while (status > 0) {
        status = mq_receive(fd, (char*)&msgReceived, attr.mq_msgsize, NULL);
        printf("%s\n", msgReceived.text);
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
    struct mq_attr attr;

    attr.mq_maxmsg = 10;
    attr.mq_msgsize = sizeof(msg_t);

    switch (option) {
    case 's':
        printf("Enviar %s a la cola %s\n", argv[3], argv[2]);
        sendMsg(argv[3], argv[2]);
        break;
    case 'r':
        printf("Recibe el primer mensaje en %s\n", argv[2]);
        recieveMsg(argv[2]);
        break;
    case 'a':
        printf("Imprimer todos los mensajes en %s\n", argv[2]);
        recieveAllMsg(argv[2]);
        break;
    case 'l':
        printf("Escucha indefinidamente por mensajes\n");
        recieveMsgUndefined(argv[2]);
        break;
    case 'c':
        printf("Crea la cola de mensajes %s\n", argv[2]);
        createQueue(argv[2], attr);
        break;
    case 'd':
        printf("Borra la cola de mensajes %s\n", argv[2]);
        mq_unlink(argv[2]);
        break;
    case 'h':
        usage(argv);
        break;
    case 'i':
        printf("Info de la cola de mensajes %s\n", argv[2]);
        mqd_t fd = mq_open(argv[2], O_RDONLY | O_NONBLOCK);
        mq_getattr(fd, &attr);
        printf("\tMaximo numero de mensajes: %ld\n", attr.mq_maxmsg);
        printf("\tTamaño de los mensajes: %ld\n", attr.mq_msgsize);
        printf("\tCantidad actual de mensajes en la cola: %ld\n", attr.mq_curmsgs);
        break;
    default:
        fprintf(stderr, "Comando desconocido: %s\n", argv[1]);
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}
