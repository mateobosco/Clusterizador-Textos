#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "heap.h"

int mi_cmp(char* string1, char* string2){
	if (string1 < string2) return 1;
	if (string2 < string1) return -1;
	else return 0;

	//return 0-strcmp(string1,string2);
}

int creador_shingles(char* nombre_archivo, int tamano){
	remove("salida");
	remove("salida_ordenada");
	FILE* archivo;
	FILE* salida;
	FILE* salida_ordenada;
	char shingle_old[tamano];
	char shingle_new[tamano];
	int i;
	for (i = 0 ; i < tamano ; i ++){
		shingle_old[i] = '*';
		shingle_new[i] = '*';
	}

	archivo = fopen(nombre_archivo , "r");
	salida = fopen("salida", "a");
	salida_ordenada = fopen("salida_ordenada", "a");
	if ( archivo == NULL){
		printf("ERROR DE LECTURA \n");
		return -1; //ERROR DE LECTURA
	}
	heap_t* mi_heap = heap_crear(mi_cmp);
	int t=0;
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
		//printf(" EL STRCMP DE ESTAS CADENAS ES %d \n", strcmp(shingle_new, shingle_old));
		printf(" EL MI_CMP DE ESTAS CADENAS ES %d \n", mi_cmp(shingle_new, shingle_old));

		printf("ESTE ES SHINGLE NEW %s \n", shingle_new);
		if (shingle_new[0] != '*' && shingle_new[tamano-1] != '*' && caracter != EOF ){
				fputs( shingle_new , salida );//ESCRIBIR SHINGLES EN SALIDA
				fputs( "\n" , salida);
				//char* shingle = malloc(sizeof(char)*tamano);
				//memcpy(shingle_new , shingle , tamano);
				bool res = heap_encolar(mi_heap, shingle_new);
				t ++;
		}

	}
	printf("ENCOLE %d cosas \n",t);
	int k=0;

	while (!heap_esta_vacio(mi_heap)){ // anda para el culo y no teng la mas puta idea de porque
		char* escribir;
		//escribir = malloc(sizeof(char)*tamano);
		escribir = heap_desencolar(mi_heap);
		printf("saco del heap %s \n", escribir);
		fputs( escribir , salida_ordenada );
		fputs( "\n" , salida_ordenada);
		k++;
	}
	printf(" DESENCOLE %d COSAS \n",k);
	heap_destruir(mi_heap, NULL);
	fclose(archivo);
	return 0;
}

/*int cmp_prueba(const void* arreglo1, const void* arreglo2){
	return strcmp((char*) arreglo1, (char*) arreglo2);
}


int prueba_heap(void){
	void* vector[10]={"2","3","4","1","6","9","8","7","5"};
	heap_t* elheap = heap_crear(cmp_prueba);
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

int main( int argc, char *argv[] ){
	return creador_shingles(argv[1] , 7); // 7 = TAMANO DE LOS SHINGLES
	//return prueba_heap();
}
