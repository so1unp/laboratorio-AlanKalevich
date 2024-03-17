#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    // Agregar código aquí.

	if(argc != 2){
		printf("Error, se necesita el tamanno del histograma como param, ingrese un solo valor\n");
		exit(EXIT_FAILURE);	
	}

	int tamannoArray = atoi(argv[1]);
	
	if (tamannoArray <= 0){
		printf("Error, ingrese un valor valido para el tamanno del histograma(mayor que 0)\n");
		exit(EXIT_FAILURE);	
	}
	
	tamannoArray++;
	int array[tamannoArray];					//array[0] no se utiliza
	int k;
	for(k = 0; k < tamannoArray; k++)
		array[k] = 0;

	int cont = 0;							//contador
	int letra = getchar();						//array para el histograma
	
	while (letra != EOF){
		
		if(letra == ' ' || letra == '\n'){
			array[cont]++;
			cont = 0;	
		} else {
			cont++;		
		}

		letra = getchar();
	}

	for(k = 1; k < tamannoArray; k++){
		printf("\n%d: ", k);
		int i;
		for(i = 0; i < array[k]; i++){
			printf("*");
		}
	}

	printf("\n");
	
    // Termina la ejecución del programa.
    exit(EXIT_SUCCESS);
}
