#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <dirent.h>
#include "estructuras/heap.h"
#include "estructuras/arbol.h"
#include "estructuras/relativo.h"
#include "estructuras/lista.h"


#define NELEMS(x)  (sizeof(x) / sizeof(x[0])) //SIZE DEL ARRAY

char* mi_strcat(const char * str1, const char * str2){
   char * ret = malloc(strlen(str1)+strlen(str2)+1);
   if(ret!=NULL){
     sprintf(ret, "%s/%s", str1, str2);
     return ret;
   }
   return NULL;
}


int creador_relativo_archivos( int argc, char *argv[] ){
	remove("relativo_nombres"); //MEDIO VISHERO PERO ANDA
	DIR *dir;
	struct dirent *mi_dirent;

	if( argc != 2 ){
		printf( "%s: %s directorio\n", argv[0], argv[0] );

		exit( -1 );
	}

	if( (dir = opendir( argv[1] )) == NULL ){
		perror( "opendir" );
		exit( -1 );
	}

	int cantidad = 0;
	int long_max = 1;
	lista_t* lista_aux;
	lista_aux = lista_crear();

	while( (mi_dirent = readdir( dir )) != NULL ){
		int aux = NELEMS(mi_dirent->d_name);
		if (aux > long_max) long_max = aux;
		if(mi_dirent->d_name[0]!= '.'){ //NEGRADA. Y PROBLEMA SI HAY UNA CARPETA
			printf( "agrego a la lista: %s\n", mi_strcat(argv[1], mi_dirent->d_name) );
			//lista_insertar_ultimo(lista_aux, mi_strcat(argv[1], mi_dirent->d_name));
			lista_insertar_ultimo(lista_aux, mi_dirent->d_name);

			cantidad ++;
		}
	}
	printf("cantidad de archivos %d \n", cantidad);
	printf("largo de la lista %d \n", (int) lista_largo(lista_aux));


	int rel_nombres;
	char nombres[] = "relativo_nombres";
	rel_nombres = R_CREATE(nombres, long_max, cantidad);

	lista_iter_t* mi_iterador = lista_iter_crear(lista_aux);
	cantidad = 0;
	while( !lista_iter_al_final(mi_iterador) ){
		int res = R_WRITE (rel_nombres, cantidad, lista_iter_ver_actual(mi_iterador));
		if (res < 0){
			perror("Error de escritura en el archivo relativo de nombres");
			remove("relativo_nombres"); //FALTA LIBERAR MEMORIA ACA
			return -1;
		}
		printf( "ESCRIBO EN EL RELATIVO EN LA POSICION %d %s\n", cantidad, (char*) lista_iter_ver_actual(mi_iterador) );
		lista_iter_avanzar(mi_iterador);
		cantidad ++;
	}
	lista_iter_destruir(mi_iterador);
	lista_destruir(lista_aux, NULL);
	int res = R_DELETE (rel_nombres, cantidad); //NO SE QUE MIERDA HACE

	closedir( dir );

	return rel_nombres;
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
	abb_imprimir(arbol);
	free(iter);
	return vector;
}

int creador_shingles(char* nombre_archivo, int tamano, abb_t* arbol){
	remove("salida");
	remove("salida_ordenada");
	FILE* archivo;
	FILE* salida;
	FILE* salida_ordenada;
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
	abb_imprimir(arbol);
	fclose(archivo);
	return 0;
}

void llamar_a_creador(int handler , int tamano, abb_t* arbol){
	char registro[100];
	int status;
	if(R_SEEK(handler,0) == R_OK ){
		status = R_READNEXT(handler, registro);
		while (status != R_ERROR){
			printf("proceso los shingles del documento: %s \n", registro);
			creador_shingles(registro, tamano, arbol);
			status = R_READNEXT(handler, registro);
		}
	}
	printf("YA PROCESE TODOS LOS TEXTOS \n");
}


int main( int argc, char *argv[] ){

	int fd_relativo_nombres = creador_relativo_archivos( argc, argv);

	abb_t* arbol_shingles = abb_crear(strcmp,NULL);

	llamar_a_creador(fd_relativo_nombres , 7 , arbol_shingles );

	printf("YA PROCESE TODOS LOS TEXTOS2 \n");
	abb_imprimir(arbol_shingles);

	char* vector = pasar_a_vector(arbol_shingles);
	abb_destruir(arbol_shingles);
	free(vector); //hay que destruir cada shingle
	return 0;
}
