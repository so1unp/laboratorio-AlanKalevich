#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    // Agregar código aquí.
	printf("hola mundon't\n");
	
	printf("Cantidad de parametros: %d\n", argc);

	printf("Comando ejecutado: %s\n", argv[0]);	

	int i;
	for(i = 1; i < argc; i++){
		char* letra = argv[i];
		while(*letra != '\0'){
			printf("%c", *letra);
			letra++;
		}
		printf ("\n");
	}
	printf("\n");
	
    // Termina la ejecución del programa.
    exit(EXIT_SUCCESS);
}
