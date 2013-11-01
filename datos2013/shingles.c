#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


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


	while (feof(archivo) == 0){
		char caracter;
		caracter = fgetc(archivo);
		printf("LEO ESTE CARACTER %c \n", caracter);
		int i;
		for (i = 1 ; i < tamano ; i ++){
			shingle_new[i-1] = shingle_old[i];
			shingle_old[i-1] = shingle_new[i-1];
		}
		shingle_new[tamano - 1] = caracter;
		shingle_old[tamano - 1] = caracter;

		printf("ESTE ES SHINGLE NEW %s", shingle_new);
		if (shingle_new[0] != '*' && shingle_new[tamano-1] != '*' ){
				fputs( shingle_new , salida );//ESCRIBIR SHINGLES EN SALIDA
				fputs( "\n" , salida);
		}

	}

	fclose(archivo);
	return 0;
}




int main( int argc, char *argv[] ){
	return creador_shingles(argv[1] , 7);
}
