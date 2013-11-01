#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


int creador_shingles(char* nombre_archivo, int tamano){
	remove("salida");
	FILE* archivo;
	FILE* salida;
	int contadores[7];
	char* shingles[7];
	int i;
	for (i = 0; i < tamano; i++){
		contadores[i] = i;
		char shingle[7] = "*******";
		shingles[i] = shingle;
	}

	archivo = fopen(nombre_archivo , "r");
	salida = fopen("salida", "a");
	if ( archivo == NULL){
		printf("ERROR DE LECTURA \n");
		return -1; //ERROR DE LECTURA
	}

	i = 0;
	while (feof(archivo) == 0){
		int j;
		for (j = 0 ; j < tamano ; j++){
			if (contadores[j] >= tamano) contadores[j] = 0; //REINICIAR CONTADORES
		}
		char caracter;
		caracter = fgetc(archivo);
		printf("LEO ESTE CARACTER %c \n", caracter);

		for (j = 0 ; j < tamano ; j++){
			shingles[j][contadores[j]] = caracter; //MODIFICAR SHINGLES
		}

		for (j = 0 ; j < tamano ; j++){
			if (shingles[j][0] != '*' && shingles[j][tamano-1] != '*' ){
				fputs( shingles[j] , salida );//ESCRIBIR SHINGLES EN SALIDA
				fputs( "\n" , salida);
			}
		}
		for (j = 0 ; j < tamano ; j++){
			contadores[j] ++ ; //AUMENTO CONTADORES
		}


	}

	fclose(archivo);
	return 0;
}





int main( int argc, char *argv[] ){
	return creador_shingles(argv[1] , 7);
}
