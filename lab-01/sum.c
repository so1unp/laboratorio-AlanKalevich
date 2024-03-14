#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    // Agregar código aquí.

	int i;
	int result = 0;
	for(i = 1; i < argc; i++){
		result += atoi(argv[i]);
	}
	
	printf("%d\n", result);
	
    // Termina la ejecución del programa.
    exit(EXIT_SUCCESS);
}
