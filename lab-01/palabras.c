#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    // Agregar código aquí.
	
	int letra;
	while((letra = getchar()) != EOF){
		printf("%c", letra);
		if(letra == ' '){
			printf("\n");
		}
	}

	
    // Termina la ejecución del programa.
    exit(EXIT_SUCCESS);
}
