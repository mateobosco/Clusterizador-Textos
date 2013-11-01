#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "heap.h"

int mi_cmp(char* string1, char* string2){
	if (string1 < string2) return 1;
	if (string2 < string1) return -1;
	else return 0;
}

int creador_shingles(char* nombre_archivo, int tamano){
	remove("salida");
	FILE* archivo;
	FILE* salida;
	char shingle_old[7] = "*******";
	char shingle_new[7] = "*******";
	archivo = fopen(nombre_archivo , "r");
	salida = fopen("salida", "a");
	if ( archivo == NULL){
		printf("ERROR DE LECTURA \n");
		return -1; //ERROR DE LECTURA
	}
	heap_t* mi_heap = heap_crear(mi_cmp);

	while (feof(archivo) == 0){
		int i;
		for (i = 1 ; i < tamano ; i ++){
			shingle_new[i-1] = shingle_old[i];
			shingle_old[i-1] = shingle_new[i-1];
		}
		char caracter;
		caracter = fgetc(archivo);
		while (caracter == '.' && caracter == ','){ //COMPROBAR QUE NO ESTE DENTRO DE UNA LISTA DE CARACTERES PROHIBIDOS
			caracter = fgetc(archivo);
		}
		printf("LEO ESTE CARACTER %c \n", caracter);
		shingle_new[tamano - 1] = caracter;
		shingle_old[tamano - 1] = caracter;

		printf("ESTE ES SHINGLE NEW %s", shingle_new);
		if (shingle_new[0] != '*' && shingle_new[tamano-1] != '*' && caracter != EOF ){
				fputs( shingle_new , salida );//ESCRIBIR SHINGLES EN SALIDA
				fputs( "\n" , salida);
		}

	}

	heap_destruir(mi_heap, NULL);
	fclose(archivo);
	return 0;
}





int main( int argc, char *argv[] ){
	return creador_shingles(argv[1] , 7);
}
