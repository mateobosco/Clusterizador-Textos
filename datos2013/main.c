#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdbool.h> //LIBRERIA PARA BINARIO, AVERIGUAR
#include "relativo.h"
#include "lista.h"


#define NELEMS(x)  (sizeof(x) / sizeof(x[0])) //SIZE DEL ARRAY


int main( int argc, char *argv[] ){
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
			printf( "%s\n", mi_dirent->d_name );
			bool res = lista_insertar_ultimo(lista_aux, mi_dirent->d_name);

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
	remove("relativo_nombres"); //MEDIO VISHERO PERO ANDA

	closedir( dir );

	return 0;
}
