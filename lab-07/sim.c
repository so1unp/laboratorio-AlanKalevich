#include <stdio.h>
#include <stdlib.h>

#define VIRTUAL 16
#define FISICA 8
#define SWAP 16
#define MAX_PROCESOS 100
#define MEM_RAM 0
#define MEM_SWAP 1
#define FIFO 0
#define LRU 1

struct PaginaVirtual {
    int pid;
    int nroPagFisica;
    int memoria;
};
typedef struct PaginaVirtual paginaVirtual_t;

struct PaginaFisica {
    int pid;
    int nroPagVirtual;
    int LRU_Index;
};
typedef struct PaginaFisica paginaFisica_t;

struct Proceso {
    int pid;
    paginaVirtual_t paginas[VIRTUAL];
};
typedef struct Proceso process_t;

struct Swap {
    paginaFisica_t paginas[SWAP];
};
typedef struct Swap swap_t;

struct RAM {
    paginaFisica_t paginas[FISICA];
};
typedef struct RAM ram_t;

process_t procesos[MAX_PROCESOS];
ram_t ram;
int proxPagRam_FIFO = 0;
int LRU_Index_Global = 0;
swap_t memSwap;
int proxPagSwap = 0;

void imprimirProcesos()
{
    int i;
    int j;
    for (i = 0; i < MAX_PROCESOS; i++)
    {
        if (procesos[i].pid != -1)
        {
            printf("Proceso %d: ", procesos[i].pid);
            for (j = 0; j < VIRTUAL; j++)
            {
                if (procesos[i].paginas[j].memoria != -1)
                {
                    if (procesos[i].paginas[j].memoria == MEM_SWAP) {
                        printf("S%d ", procesos[i].paginas[j].nroPagFisica);
                    }
                    else {
                        printf("R%d ", procesos[i].paginas[j].nroPagFisica);
                    }
                }
                else
                {
                    printf("- ");
                }
            }
            printf("\n");
        }
    }
}

void imprimirMemoriaPrincipal()
{
    int i;
    printf("Memoria fisica: ");
    for (i = 0; i < FISICA; i++)
    {
        if (ram.paginas[i].pid != -1)
        {
            printf("%d.%d ", ram.paginas[i].pid, ram.paginas[i].nroPagVirtual);
        }
        else
        {
            printf("- ");
        }
    }
}

void imprimirMemoriaSecundaria()
{
    printf("\nMemoria secundaria: ");
    int i;
    for (i = 0; i < VIRTUAL; i++)
    {
        if (memSwap.paginas[i].pid != -1)
        {
            printf("%d.%d ", memSwap.paginas[i].pid, memSwap.paginas[i].nroPagVirtual);
        }
        else
        {
            printf("- ");
        }
    }
    printf("\n");
}

void printAll() {
    imprimirProcesos();
    imprimirMemoriaPrincipal();
    imprimirMemoriaSecundaria();
}

void initStructs() {
    int i;
    int j;
    for (i = 0; i < MAX_PROCESOS; i++) {
        procesos[i].pid = -1;
        for (j = 0; j < VIRTUAL; j++) {
            procesos[i].paginas[j].pid = -1;
            procesos[i].paginas[j].memoria = -1;
        }
    }
    for (i = 0; i < FISICA; i++) {
        ram.paginas[i].pid = -1;
        ram.paginas[i].LRU_Index = -1;
    }
    for (i = 0; i < SWAP; i++) {
        memSwap.paginas[i].pid = -1;
    }
}

void crearProceso(int pid, int nroPagProc) {
    procesos[pid].pid = pid;
    procesos[pid].paginas[nroPagProc].pid = pid;
}

void cargarEnLaSwap(int pid, int nroPagProc) {
    memSwap.paginas[proxPagSwap].pid = pid;
    memSwap.paginas[proxPagSwap].nroPagVirtual = nroPagProc;

    procesos[pid].paginas[nroPagProc].memoria = MEM_SWAP;
    procesos[pid].paginas[nroPagProc].nroPagFisica = proxPagSwap;
    proxPagSwap++;
    if (proxPagSwap == SWAP) {
        printAll();
        exit(EXIT_SUCCESS);
    }
}

void cargarEnLaRam(int pid, int nroPagProc, int mode) {
    if (mode == FIFO) {
        if (ram.paginas[proxPagRam_FIFO].pid != -1)
            cargarEnLaSwap(ram.paginas[proxPagRam_FIFO].pid, ram.paginas[proxPagRam_FIFO].nroPagVirtual);

        ram.paginas[proxPagRam_FIFO].pid = pid;
        ram.paginas[proxPagRam_FIFO].nroPagVirtual = nroPagProc;

        procesos[pid].paginas[nroPagProc].nroPagFisica = proxPagRam_FIFO;
        proxPagRam_FIFO = (proxPagRam_FIFO + 1) % FISICA;
    }
    else {
        int i;
        int index = 0;
        int minLRU = ram.paginas[0].LRU_Index;
        for (i = 0; i < FISICA; i++) {
            if (ram.paginas[i].LRU_Index < minLRU) {
                index = i;
                minLRU = ram.paginas[i].LRU_Index;
            }
        }
        if (ram.paginas[index].pid != -1)
            cargarEnLaSwap(ram.paginas[index].pid, ram.paginas[index].nroPagVirtual);

        ram.paginas[index].pid = pid;
        ram.paginas[index].nroPagVirtual = nroPagProc;
        ram.paginas[index].LRU_Index = LRU_Index_Global;
        LRU_Index_Global++;
        procesos[pid].paginas[nroPagProc].nroPagFisica = index;
    }

    procesos[pid].paginas[nroPagProc].memoria = MEM_RAM;
}

void intercambiarEntreSwapYRam(int pid, int nroPagProc, int mode) {
    int i;
    for (i = 0; i < SWAP; i++) {
        if (memSwap.paginas[i].pid == pid && memSwap.paginas[i].nroPagVirtual == nroPagProc) {
            if (mode == FIFO) {
                memSwap.paginas[i].pid = ram.paginas[proxPagRam_FIFO].pid;
                memSwap.paginas[i].nroPagVirtual = ram.paginas[proxPagRam_FIFO].nroPagVirtual;
                procesos[memSwap.paginas[i].pid].paginas[memSwap.paginas[i].nroPagVirtual].memoria = MEM_SWAP;
                procesos[memSwap.paginas[i].pid].paginas[memSwap.paginas[i].nroPagVirtual].nroPagFisica = i;

                ram.paginas[proxPagRam_FIFO].pid = pid;
                ram.paginas[proxPagRam_FIFO].nroPagVirtual = nroPagProc;
                procesos[pid].paginas[nroPagProc].memoria = MEM_RAM;
                procesos[pid].paginas[nroPagProc].nroPagFisica = proxPagRam_FIFO;
                
                proxPagRam_FIFO = (proxPagRam_FIFO + 1) % FISICA;
            }
            else {
                int j;
                int index = 0;
                int minLRU = ram.paginas[0].LRU_Index;
                for (j = 0; j < FISICA; j++) {
                    if (ram.paginas[j].LRU_Index < minLRU) {
                        index = j;
                        minLRU = ram.paginas[j].LRU_Index;
                    }
                }
                memSwap.paginas[i].pid = ram.paginas[index].pid;
                memSwap.paginas[i].nroPagVirtual = ram.paginas[index].nroPagVirtual;
                procesos[ram.paginas[index].pid].paginas[ram.paginas[index].nroPagVirtual].memoria = MEM_SWAP;
                procesos[ram.paginas[index].pid].paginas[ram.paginas[index].nroPagVirtual].nroPagFisica = i;

                ram.paginas[index].pid = pid;
                ram.paginas[index].nroPagVirtual = nroPagProc;
                procesos[pid].paginas[nroPagProc].memoria = MEM_RAM;
                procesos[pid].paginas[nroPagProc].nroPagFisica = index;

                ram.paginas[index].LRU_Index = LRU_Index_Global;
                LRU_Index_Global++;
            }
            return;
        }
    }
}

void actualizarLRUIndex(int pid, int nroPagProc) {

    int i;
    for (i = 0; i < FISICA; i++) {
        if (ram.paginas[i].pid == pid && ram.paginas[i].nroPagVirtual == nroPagProc) {
            ram.paginas[i].LRU_Index = LRU_Index_Global;
            LRU_Index_Global++;
        }
    }

}

int main(int argc, char* argv[])
{
    if (argc != 2) {
        printf("Usage:\n\t-f: FIFO\n\t-l: LRU\n");
        exit(EXIT_FAILURE);
    }

    if (argv[1][0] != '-') {
        printf("noseusaasi\n");
        exit(EXIT_FAILURE);
    }
    int mode;
    if (argv[1][1] == 'f') {
        mode = FIFO;
    }
    else {
        mode = LRU;
    }

    initStructs();
    int pid;
    int nroPagProc;
    while (scanf("%d\n%d", &pid, &nroPagProc) != EOF) {
        if (procesos[pid].pid != -1 && procesos[pid].paginas[nroPagProc].pid != -1) {
            if (procesos[pid].paginas[nroPagProc].pid != -1) {
                if (procesos[pid].paginas[nroPagProc].memoria == MEM_SWAP) {
                    intercambiarEntreSwapYRam(pid, nroPagProc, mode);
                    continue;
                }
                else {
                    if (mode == LRU)
                        actualizarLRUIndex(pid, nroPagProc);
                    continue;
                }
            }
        }
        else {
            crearProceso(pid, nroPagProc);
        }
        cargarEnLaRam(pid, nroPagProc, mode);
    }

    printAll();
    exit(EXIT_SUCCESS);
}
