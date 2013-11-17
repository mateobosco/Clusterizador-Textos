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

void imprimir_relativo(int fd){
	char* registro = malloc(50 * sizeof(char));
	int status;
	int i = 0;
	if(R_SEEK(fd,0) == R_OK ){
		status = R_READNEXT(fd, registro);
		while (status != R_ERROR){
			printf ("Posicion: %d - Registro: %s \n", i , registro);
			status = R_READNEXT(fd, registro);
			i ++;
		}
	}
	printf("FIN DEL RELATIVO \n");
	free(registro);
}

char* mi_strcat(const char * str1, const char * str2){
	int largo1 = strlen(str1);
	char aux1[largo1+2];
	strcpy(aux1,str1);
	aux1[largo1] = '/';
	char * ret = malloc(strlen(str1)+strlen(str2)+2);
	if(ret!=NULL){
		sprintf(ret, "%s%s", aux1, str2);
		return ret;
	}
	return NULL;
}

int devuelve_cantidad_archivos( int argc , char* argv[]){
	int cantidad = 0;
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
	while( (mi_dirent = readdir( dir )) != NULL ){
		int aux = NELEMS(mi_dirent->d_name);
		if(mi_dirent->d_name[0]!= '.'){ //NEGRADA. Y PROBLEMA SI HAY UNA CARPETA
			cantidad ++;
		}
	}
	return cantidad;
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
	lista_t* lista_aux = lista_crear();

	while( (mi_dirent = readdir( dir )) != NULL ){
		int aux = NELEMS(mi_dirent->d_name);
		if (aux > long_max) long_max = aux;
		if(mi_dirent->d_name[0]!= '.'){ //NEGRADA. Y PROBLEMA SI HAY UNA CARPETA
			printf( "agrego a la lista: %s\n", mi_strcat(argv[1], mi_dirent->d_name) );
			lista_insertar_ultimo(lista_aux, mi_strcat(argv[1], mi_dirent->d_name));
			cantidad ++;
		}
	}
	printf("cantidad de archivos %d \n", cantidad);
	printf("largo de la lista %d \n", (int) lista_largo(lista_aux));
	printf("longitud maxima %d \n", long_max);


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

	//imprimir_relativo(rel_nombres);

	printf("RELATIVO DE NOMBRES CREADO \n");

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
	FILE* archivo;
	char* shingle_old = malloc((tamano+1) * sizeof(char));
	char* shingle_new = malloc((tamano+1) * sizeof(char));
	int i;
	for (i = 0 ; i < tamano ; i ++){
		shingle_old[i] = '*';
		shingle_new[i] = '*';
	}
	shingle_old[tamano] = '\0';
	shingle_new[tamano] = '\0';
	archivo = fopen(nombre_archivo , "r");// ACA HAY UN ERROR Y LA PUTA MADRE
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
			char* shingle = malloc((tamano+1) * sizeof(char));
			memcpy(shingle , shingle_new , tamano+1);
			bool res = abb_guardar(arbol, shingle, shingle);
		}
	}
	fclose(archivo);
	return 0;
}

void llamar_a_creador(int fd_relativo_nombres , int tamano, abb_t* arbol_shingles){
	char* registro = malloc(50 * sizeof(char));
	//char registro[100];
	int status;
	if(R_SEEK(fd_relativo_nombres,0) == R_OK ){
		status = R_READNEXT(fd_relativo_nombres, registro);
		printf ("en la llamada a creador shingles: REGISTRO %s, STATUS %d \n", registro, status);
		while (status != R_ERROR ){
			printf ("llamo al creador de shingles con el archivo: REGISTRO %s, STATUS %d \n", registro, status);
			creador_shingles(registro, tamano, arbol_shingles);
			status = R_READNEXT(fd_relativo_nombres, registro);
			printf("LLAMO AL PROXIMO \n");
		}
	}
	printf("YA PROCESE TODOS LOS TEXTOS \n");
	free(registro);
}

int busq_binaria(char* vector_shingles, char* shingle, int tamano_vector){
	int inicio = 0;
	int final = tamano_vector - 1;
	int medio;
	while (inicio <= final){
		medio = (final - inicio) / 2;
		int res = strcmp(vector_shingles[medio], shingle);
		if (res == 0) return medio;
		else if (res < 0) inicio = medio;
		else if (res > 0) final = medio;
	}
	return -1;
}


char* vector_incidencia(char* nombre_archivo, char* vector_shingles, int tamano_vector, int tamano){
	char* incidencia = malloc(sizeof(char) * tamano_vector);
	FILE* archivo;
	char* shingle_old = malloc((tamano+1) * sizeof(char));
	char* shingle_new = malloc((tamano+1) * sizeof(char));
	int i;
	for (i = 0 ; i < tamano ; i ++){
		shingle_old[i] = '*';
		shingle_new[i] = '*';
	}
	shingle_old[tamano] = '\0';
	shingle_new[tamano] = '\0';

	archivo = fopen(nombre_archivo , "r");
	if ( archivo == NULL){
		printf("ERROR DE LECTURA \n");
		//return -1; //ERROR DE LECTURA
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
		if (shingle_new[0] != '*' && shingle_new[tamano-1] != '*' && caracter != EOF){
			int posicion = busq_binaria(vector_shingles, shingle_new, tamano_vector);
			 if (posicion >= 0) incidencia[posicion] = 1;
			}
		}
	fclose(archivo);
	return incidencia;
}

int creador_relativo_incidencia(int fd_relativo_nombres, char* vector, int cantidad_archivos, int cantidad_shingles, int tamano_shingle){
	remove("relativo_incidencia");
	int rel_incidencia;
	char nombres[] = "relativo_incidencia";
	rel_incidencia = R_CREATE(nombres, cantidad_shingles*sizeof(char), cantidad_archivos);
/*
	HAY QUE IR LEYENDO EL RELATIVO DE NOMBRES, DE AHI LLAMAR A UNA FUNCION QUE VAYA LEYENDO TODOS LOS SHINGLES
	DEL TEXTO Y FIJANDOSE SI ESTAN DENTRO DEL VECTOR PARA IR ARMANDO EL VECTOR DE INCIDENCIA
	DESPUES ESCRIBIR ESE VECTOR DENTRO DEL RELATIVO
*/
	char* registro = malloc(25 * sizeof(char));
	int status;
	int i = 0;
	if(R_SEEK(fd_relativo_nombres,0) == R_OK ){
		status = R_READNEXT(fd_relativo_nombres, registro); // ACA ESTA EL PROBLEMA
		printf ("REGISTRO %s, STATUS %d \n", registro, status);
		while (status != R_ERROR){
			char* incidencia = vector_incidencia(registro, vector, cantidad_shingles, tamano_shingle);
			printf("IMPRIMO EN EL VECTOR DE INCIDENCIA %s: %s \n", registro, incidencia);
			int res = R_WRITE (rel_incidencia, i , incidencia);
			if (res < 0){
				perror("Error de escritura en el archivo relativo de nombres");
				remove("relativo_nombres"); //FALTA LIBERAR MEMORIA ACA
				return -1;
			}
			status = R_READNEXT(fd_relativo_nombres, registro);
			i ++;
		}
	}
	//free(registro); no probe pero seguro se rompe, (igual que el anterior)
	return rel_incidencia;
}


int el_main( int argc, char *argv[] , int tamano_shingle ){

	int cantidad_archivos = devuelve_cantidad_archivos(argc, argv);

	int fd_relativo_nombres = creador_relativo_archivos( argc, argv);

	printf("CREO EL ARBOL \n");

	abb_t* arbol_shingles = abb_crear(strcmp,NULL);

	printf("LLAMO AL CREADOR DE SHINGLES \n");

	llamar_a_creador(fd_relativo_nombres , tamano_shingle , arbol_shingles );

	char* vector = pasar_a_vector(arbol_shingles);
	int tamano_arbol = abb_cantidad(arbol_shingles);
	abb_destruir(arbol_shingles);
	printf("CREO EL RELATVIVO DE INCIDENCIA \n");
	int fd_relativo_incidencia = creador_relativo_incidencia(fd_relativo_nombres, vector, cantidad_archivos, tamano_arbol, tamano_shingle);
	printf(" RELATIVO DE INCIDENCIA CREADO \n");
	free(vector); //hay que destruir cada shingle
	return 0;
}

int main( int argc, char *argv[] ){

	el_main(argc, argv, 7);

	return 0;
}
