#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdbool.h> //LIBRERIA PARA BINARIO AVERIGUAR
#include "relativo.h"

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

	int cantidad;

	while( (mi_dirent = readdir( dir )) != NULL ){
		if(mi_dirent->d_name[0]!= '.'){ //NEGRADA. Y PROBLEMA SI HAY UNA CARPETA
			printf( "%s\n", mi_dirent->d_name );
			cantidad ++;
		}
	}
	printf("cantidad de archivos %d \n", cantidad);
	closedir( dir );

	return 0;
}
