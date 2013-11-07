#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "heap.h"
#include "arbol.h"

int mi_cmp(char* string1, char* string2){
	return 0-strcmp(string1,string2);
}

char* pasar_a_vector(abb_t* arbol){
	size_t tamano = abb_cantidad(arbol);
	char* vector = malloc(tamano * sizeof(char*));
	abb_iter_t* iter = abb_iter_in_crear(arbol);
	int i = 0;
	while( !abb_iter_in_al_final(iter)){
		vector[i] = abb_iter_in_ver_actual(iter);
		abb_iter_in_avanzar(iter);
		i++;
	}
	for (i=0;i<tamano;i++){
		printf("IMPRIMO VECTOR\n");
		//printf(vector[i]);
		printf("\n");
	}
	//free(iter);
	//abb_destruir(arbol);
	return vector;
}

int creador_shingles(char* nombre_archivo, int tamano){
	remove("salida");
	remove("salida_ordenada");
	FILE* archivo;
	FILE* salida;
	FILE* salida_ordenada;
	char* shingle_old = malloc((tamano+1) * sizeof(char));
	char* shingle_new = malloc((tamano+1) * sizeof(char));
	char escribir_old[tamano+1];
	abb_t* arbol = abb_crear(mi_cmp,free);
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
		shingle_new[tamano - 1] = caracter;
		shingle_old[tamano - 1] = caracter;

		if (shingle_new[0] != '*' && shingle_new[tamano-1] != '*' && caracter != EOF && !abb_pertenece(arbol,shingle_new) ){
			fputs( shingle_new , salida );//ESCRIBIR SHINGLES EN SALIDA
			fputs( "\n" , salida);
			char* shingle = malloc((tamano+1) * sizeof(char));
			memcpy(shingle , shingle_new , tamano+1);
			bool res = abb_guardar(arbol, shingle, shingle);
		}

	}
	//abb_imprimir(arbol);
	char* vector = pasar_a_vector(arbol);
	fclose(archivo);
	return 0;
}

//void* incidencia_texto(void* vector, char* archivo, int tamano)

int main( int argc, char *argv[] ){
	return creador_shingles(argv[1] , 7); // 7 = TAMANO DE LOS SHINGLES

}
