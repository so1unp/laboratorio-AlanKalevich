#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
    // Agregar código aquí.

	int tamannoArray = 10; 		//default
	
	if(argc > 2){
		printf("Error, se necesita el tamanno del histograma como param, ingrese un solo valor\n");
		exit(EXIT_FAILURE);	
	}

	if(argc == 2){
		tamannoArray = atoi(argv[1]);
		if (tamannoArray <= 0){
			printf("Error, ingrese un valor valido para el tamanno del histograma(mayor que 0)\n");
			exit(EXIT_FAILURE);	
		}
	}
	
	int array[tamannoArray + 1];					//array[0] no se utiliza
	int k;
	for(k = 0; k < tamannoArray + 1; k++)
		array[k] = 0;

	int cont = 0;							//contador
	int letra = getchar();						//array para el histograma
	
	while (letra != EOF){
		
		if(letra == ' ' || letra == '\n'){			//fin de palabra, guardado en el array
			if(cont <= tamannoArray){
				array[cont]++;
			}
			cont = 0;
		} else {						//cuenta caracteres
			cont++;		
		}

		letra = getchar();					//siguiente caracter
	}
	
	printf("Tamanno del array: %d", tamannoArray);	

	for(k = 1; k < tamannoArray + 1; k++){
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
