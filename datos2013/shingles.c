#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "heap.h"

int mi_cmp(char* string1, char* string2){
	//if (string1 < string2) return 1;
	//if (string2 < string1) return -1;
	//else return 0;

	return 0-strcmp(string1,string2);
}

int creador_shingles(char* nombre_archivo, int tamano){
	remove("salida");
	remove("salida_ordenada");
	FILE* archivo;
	FILE* salida;
	FILE* salida_ordenada;
	//char shingle_old[tamano+1];
	//char shingle_new[tamano+1];
	char* shingle_old = malloc((tamano+1) * sizeof(char));
	char* shingle_new = malloc((tamano+1) * sizeof(char));
	char escribir_old[tamano+1];
	int i;
	for (i = 0 ; i < tamano ; i ++){
		shingle_old[i] = '*';
		shingle_new[i] = '*';
		escribir_old[i] = '*';
	}
	shingle_old[tamano] = '\0';
	shingle_new[tamano] = '\0';

	archivo = fopen(nombre_archivo , "r");
	salida = fopen("salida", "a");
	salida_ordenada = fopen("salida_ordenada", "a");
	if ( archivo == NULL){
		printf("ERROR DE LECTURA \n");
		return -1; //ERROR DE LECTURA
	}
	heap_t* mi_heap = heap_crear(mi_cmp);
	while (feof(archivo) == 0){
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

		printf("ESTE ES SHINGLE NEW %s \n", shingle_new);
		if (shingle_new[0] != '*' && shingle_new[tamano-1] != '*' && caracter != EOF ){
				fputs( shingle_new , salida );//ESCRIBIR SHINGLES EN SALIDA
				fputs( "\n" , salida);
				char* shingle = malloc((tamano+1) * sizeof(char));
				memcpy(shingle , shingle_new , tamano+1);
				bool res = heap_encolar(mi_heap, shingle);
		}

	}
	while (!heap_esta_vacio(mi_heap)){ // sacar repetidos
		char* escribir_new;
		escribir_new = heap_desencolar(mi_heap);
		printf("saco del heap %s \n", escribir_new);
		while(strcmp(escribir_new, escribir_old)== 0){
			printf("REPETIDO, NO LO AGREGO \n");
			escribir_new = heap_desencolar(mi_heap);
			//free(escribir_new);
		}
		memcpy(escribir_old, escribir_new, tamano+1);
		fputs( escribir_new , salida_ordenada );
		fputs( "\n" , salida_ordenada);
		free(escribir_new);
	}
	heap_destruir(mi_heap, NULL);
	fclose(archivo);
	return 0;
}


/*
int prueba_heap(void){
	void* vector[10]={"hola","chau","puto","el","que","lee","8","7","5"};
	heap_t* elheap = heap_crear(mi_cmp);
	int i;
	for (i=0; i<10;i++){
		heap_encolar(elheap, vector[i]);
	}
	int numero;
	while(!heap_esta_vacio(elheap)){
		printf(heap_desencolar(elheap));
		printf("\n");
	}

	return 0;
}
*/

/*
int prueba_cmp(void){
	char cadena2[5] = "hola";
	char cadena1[5] = "chau";
	char vector2[5] = {'h','o','l','a','\0'};
	char vector1[5] = {'c','h','a','u','\0'};
	printf("STRCMP DE CADENA1 Y CADENA2 %d \n", strcmp(cadena1,cadena2));
	printf("MICMP DE CADENA1 Y CADENA2 %d \n", mi_cmp(cadena1,cadena2));
	printf("STRCMP DE vector1 Y vector2 %d \n", strcmp(vector1,vector2));
	printf("MICMP DE vector1 Y vector2 %d \n", mi_cmp(vector1,vector2));
	return 0;
}
*/


int main( int argc, char *argv[] ){
	return creador_shingles(argv[1] , 7); // 7 = TAMANO DE LOS SHINGLES
	//return prueba_heap();
	//return prueba_cmp();
}
