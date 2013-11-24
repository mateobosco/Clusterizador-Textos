#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <math.h>

#include <dirent.h>
#include "estructuras/heap.h"
#include "estructuras/arbol.h"
#include "estructuras/relativo.h"
#include "estructuras/lista.h"



#define NELEMS(x)  (sizeof(x) / sizeof(x[0])) //SIZE DEL ARRAY
#define TAM_SHINGLE 7
#define CANTIDAD_FUNCIONES 50

struct cluster{
	lista_t* lista_elementos;
	int centro;
};
typedef struct cluster cluster_t;

cluster_t* cluster_crear(){
	cluster_t* cluster= malloc (sizeof(cluster_t));
	if (!cluster) return NULL;
	cluster->centro=-1;
	lista_t* lista_elementos = lista_crear();
	cluster->lista_elementos = lista_elementos;
	return cluster;
}

void imprimir_relativo(int fd, int tam_registro){
        char* registro = malloc(tam_registro * sizeof(char));
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

char* mistrcat(const char * str1, const char * str2){
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

char* mi_strcat(const char * str1, const char * str2){
        int largo1 = strlen(str1);
        int largo2 = strlen(str2);
        char * fin = malloc (sizeof(char)*(largo1+largo2+1));
        char barra[]="/";
        strcat(fin,str1);
        strcat(fin,barra);
        strcat(fin,str2);
        return fin;
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
        // ELEGIR 'J' LIDERES EN ESTA PARTE.
        //int j = 20*log10(cantidad); // J lideres
        lista_t* lideres = lista_crear();
        rel_nombres = R_CREATE(nombres, 50, cantidad); //esta linea rompia todo, decia long_max y le puse 30, despues ver bien

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
                // decidir si es lider o no
                // agregarlo a la lista d lideres
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

char** pasar_a_vector(abb_t* arbol){
        size_t tamano = abb_cantidad(arbol);
        char** vector = malloc(tamano * sizeof(char*));
        abb_iter_t* iter = abb_iter_in_crear(arbol);
        int i = 0;
        while( !abb_iter_in_al_final(iter)){
                char* shingle = malloc(sizeof(char) * TAM_SHINGLE);
                strcpy(shingle, abb_iter_in_ver_actual(iter));
                vector[i] = shingle;
                abb_iter_in_avanzar(iter);
                //printf("posicion %d - %s \n",i ,vector[i]); imprime el vector, anda bien esto
                i++;
        }
        //abb_imprimir(arbol);
        free(iter);
        return vector;
}

int creador_shingles(char* nombre_archivo, abb_t* arbol){
        FILE* archivo;
        char* shingle_old = malloc((TAM_SHINGLE+1) * sizeof(char));
        char* shingle_new = malloc((TAM_SHINGLE+1) * sizeof(char));
        int i;
        for (i = 0 ; i < TAM_SHINGLE ; i ++){
                shingle_old[i] = '*';
                shingle_new[i] = '*';
        }
        shingle_old[TAM_SHINGLE] = '\0';
        shingle_new[TAM_SHINGLE] = '\0';
        archivo = fopen(nombre_archivo , "r");// ACA HAY UN ERROR Y LA PUTA MADRE
        if ( archivo == NULL){
                printf("ERROR DE LECTURA \n");
                return -1; //ERROR DE LECTURA
        }
        while (feof(archivo) == 0){
                for (i = 1 ; i < TAM_SHINGLE ; i ++){
                        shingle_new[i-1] = shingle_old[i];
                        shingle_old[i-1] = shingle_new[i-1];
                }
                char caracter;
                caracter = fgetc(archivo);
                while (caracter == '.' && caracter == ','){ //COMPROBAR QUE NO ESTE DENTRO DE UNA LISTA DE CARACTERES PROHIBIDOS
                        caracter = fgetc(archivo);
                }
                shingle_new[TAM_SHINGLE - 1] = caracter;
                shingle_old[TAM_SHINGLE - 1] = caracter;

                if (shingle_new[0] != '*' && shingle_new[TAM_SHINGLE-1] != '*' && caracter != EOF && !abb_pertenece(arbol,shingle_new) ){
                        char* shingle = malloc((TAM_SHINGLE+1) * sizeof(char));
                        memcpy(shingle , shingle_new , TAM_SHINGLE+1);
                        bool res = abb_guardar(arbol, shingle, shingle);
                }
        }
        fclose(archivo);
        return 0;
}

void llamar_a_creador(int fd_relativo_nombres , abb_t* arbol_shingles){
        char* registro = malloc(50 * sizeof(char));
        //char registro[100];
        int status;
        if(R_SEEK(fd_relativo_nombres,0) >= R_OK ){
                status = R_READNEXT(fd_relativo_nombres, registro);
                printf ("en la llamada a creador shingles: REGISTRO %s, STATUS %d \n", registro, status);
                while (status != R_ERROR ){
                        printf ("llamo al creador de shingles con el archivo: REGISTRO %s, STATUS %d \n", registro, status);
                        creador_shingles(registro, arbol_shingles);
                        status = R_READNEXT(fd_relativo_nombres, registro);
                        printf("LLAMO AL PROXIMO \n");
                }
        }
        printf("YA PROCESE TODOS LOS TEXTOS \n");
        free(registro);
}

int busq_binaria(char** vector_shingles, char* shingle, int tamano_vector){
        int inicio = 0;
        int final = tamano_vector - 1;
        int medio;
        while (inicio <= final){
                medio = (final + inicio) / 2;
                int res = strcmp(vector_shingles[medio], shingle);
                //printf("comparo %s con %s y da %d\n", vector_shingles[medio],shingle,res);
                if (res == 0) return medio;
                else if (res < 0) inicio = medio + 1;
                else if (res > 0) final = medio - 1;
        }
        return -1;
}


int hash(int funcion, int entrada){
        return (funcion * entrada)%32000;
}

char* vector_incidencia(char* nombre_archivo, char** vector_shingles, int tamano_vector, char* vector_vacio){
        char* incidencia = malloc(sizeof(char) * tamano_vector);
        strcpy(incidencia, vector_vacio);
        FILE* archivo;
        char* shingle_old = malloc((TAM_SHINGLE+1) * sizeof(char));
        char* shingle_new = malloc((TAM_SHINGLE+1) * sizeof(char));
        int i;
        for (i = 0 ; i < TAM_SHINGLE ; i ++){
                shingle_old[i] = '*';
                shingle_new[i] = '*';
        }
        shingle_old[TAM_SHINGLE] = '\0';
        shingle_new[TAM_SHINGLE] = '\0';
        //nombre_archivo[11] = "a";
        printf("nombre del archivo es %s \n", nombre_archivo);
        archivo = fopen(nombre_archivo , "r");
        if ( archivo == NULL){
                printf("ERROR DE LECTURA \n");
                //return -1; //ERROR DE LECTURA
        }
        while (feof(archivo) == 0){
                for (i = 1 ; i < TAM_SHINGLE ; i ++){
                        shingle_new[i-1] = shingle_old[i];
                        shingle_old[i-1] = shingle_new[i-1];
                }
                char caracter;
                caracter = fgetc(archivo);
                while (caracter == '.' && caracter == ','){ //COMPROBAR QUE NO ESTE DENTRO DE UNA LISTA DE CARACTERES PROHIBIDOS
                        caracter = fgetc(archivo);
                }
                shingle_new[TAM_SHINGLE - 1] = caracter;
                shingle_old[TAM_SHINGLE - 1] = caracter;
                if (shingle_new[0] != '*' && shingle_new[TAM_SHINGLE-1] != '*' && caracter != EOF){
                        printf("Busco este shingle en el vector: %s\n",shingle_new);
                        int posicion = busq_binaria(vector_shingles, shingle_new, tamano_vector);
                        printf("La busqueda binaria devuelve: %d \n", posicion);
                        if (posicion >= 0){
                                incidencia[posicion] = '1';
                                printf("Encuentro un shingle en vector \n");
                        }
                }
        }

        fclose(archivo);
        return incidencia;
}


int* generador_funciones_hasmin(void){
        int* funciones = malloc(sizeof(int) * CANTIDAD_FUNCIONES);
        printf("LAS FUNCIONES DE HASMIN SON :");
        for (int i=0; i<CANTIDAD_FUNCIONES;i++){
                funciones[i] = (rand() % 32000);
                printf(" &d ",funciones[i]);
        }
        printf("\n");
        return funciones;
}

short char_a_short(char* numero){
	short valor;
	valor = (short) 256 * numero[1] + (short) numero[0];
	return valor;
}

char* short_a_char(short numero){
	char* resultado = malloc(sizeof(char) * 3);
	resultado[0] = ( numero % 256);
	//printf("numero % 256 es %d \n", numero % 256);
	resultado[1] =  (numero - resultado[0])/256 ;
	//printf(" (numero - resultado[0])/256 %d \n", (numero - resultado[0]));
	resultado[2] = '-';
	//printf(" ESTO ES RESULTADO 0 %c \n", resultado[0]);
	//printf(" ESTO ES RESULTADO 1 %c \n", resultado[1]);
	//printf(" ESTO ES RESULTADO 2 %c \n", resultado[2]);
	//printf(" ESTO ES RESULTADO %d \n", resultado);
	return resultado;
}

char* creador_vector_hashmin(char* vector_incidencia, int cantidad, int* funciones){
        char* vector_hasmin = malloc (sizeof(char) * CANTIDAD_FUNCIONES * 3);
        for (int j = 0; j<CANTIDAD_FUNCIONES ; j++){
                short anterior = 32000;
                for (int k = 0 ; k < cantidad ; k++){
                        char valor = vector_incidencia[j];
                        if (valor == '1'){
                                short nuevo = hash( k , funciones[j] );
                                if (nuevo < anterior) anterior = nuevo;
                        }
                }
                char* numero = short_a_char(anterior);
                vector_hasmin[j * 3] = numero[0];
                vector_hasmin[j * 3 + 1] = numero[1];
                vector_hasmin[j * 3 + 2] = numero[2];
        }
        return vector_hasmin;
}

int jaccard (char* vector1,  char* vector2, int largo){
	int contador_diferentes;
	int contador_iguales;
	for(int i=0; i<largo; i++){
		if (vector1[i] == vector2[i]){
			contador_iguales++;
			contador_diferentes++;
		}
		else{
			contador_diferentes++;
		}
	}
	return (contador_iguales/contador_diferentes);

}

char* selector_lideres(int cantidad_clusters, int cantidad_archivos, int cantidad_lideres){
	char* lista_lideres = malloc(sizeof(int)*cantidad_lideres);
	for (int z=0; z < cantidad_lideres; z++){
		int lider = rand() % cantidad_archivos;
		lista_lideres[z]=lider;
	}
	return lista_lideres;
}


char* creador_clusters(int cantidad_clusters, int cantidad_archivos, char* lideres, int cantidad_lideres, int cantidad_centros){
	//elegir J lideres
	char* lista_clusters = malloc(sizeof(cluster_t*)*cantidad_clusters); // esta feo
	for(int i=0; i< cantidad_clusters; i++){
		cluster_t* cluster = cluster_crear();
		lista_clusters[i]=cluster;
		// creo dentro de la posicion 0, el primer cluster, que es una lista de documentos
		// lo mismo para 1,2,...,k

		for (int j=0; j < cantidad_centros; j++){
			int centro = rand() % cantidad_lideres;
			cluster->centro = centro;
		}
	}
	return lista_clusters;
	//elegir k clusters
}

short* creador_matriz_hashmin(int rel_hashmin, int cantidad_clusters, int cantidad_lideres, char* lista_clusters, char* lideres){
		short* matriz = malloc(sizeof(short*)*cantidad_clusters); // K
		int w=0;
		int status;
		int lider;
		//int rel_hashmin;
		char nombres[] = "relativo_hasmin";
		//rel_hashmin=R_OPEN(nombres, "read");
		//printf("REL_HASHMIN ES %d \n", rel_hashmin);

		char* registro = malloc(25 * sizeof(char));
		for(int i = 0; i< cantidad_lideres; i++){
			lider = lideres[i];
			printf("el lider es %d \n", lider);
			status = R_READ(rel_hashmin, lider, registro);
			matriz[w]= registro; //guardo en la matriz el vector hashmin del lider actual
		}
		return matriz;
}

asignar_documento_a_cluster(short* matriz, int fd_relativo_nombres, int cantidad_lideres, char* lideres, char* lista_clusters){
	char nombres[] = "relativo_hasmin";
	int similitud = 9999;
	int mas_parecido;
	char* registro = malloc(25 * sizeof(char));
	if(R_SEEK(fd_relativo_nombres,0) >= R_OK ){
		status = R_READNEXT(fd_relativo_nombres, registro);
		for (int i=0; i < cantidad_lideres; i++ ){
			similitud_aux = jaccard(registro, matriz[i], 25 );
			if (similitud_aux < similitud){
				similitud=similitud_aux;
				mas_parecido= i;
			}
		}
	}
}


int creador_relativo_hashmin(int fd_relativo_nombres, char** vector, int cantidad_archivos, int cantidad_shingles, int* funciones, int cantidad_lideres){
        remove("relativo_hasmin");
        int rel_hasmin;
        char nombres[] = "relativo_hasmin";
        rel_hasmin = R_CREATE(nombres, cantidad_shingles*sizeof(char), cantidad_archivos);
        char* vector_vacio = malloc(sizeof(char) * (cantidad_shingles+1));
       // char* matriz_hashmin = malloc(sizeof(char) * (cantidad_lideres));
        for (int i = 0 ; i < cantidad_shingles ; i++){
                vector_vacio[i] = '0';
        }
        vector_vacio[cantidad_shingles] = '\0';
        char* registro = malloc(25 * sizeof(char));
        int status;
        int i = 0;
        if(R_SEEK(fd_relativo_nombres,0) >= R_OK ){
                status = R_READNEXT(fd_relativo_nombres, registro); // ACA ESTA EL PROBLEMA
                printf ("REGISTRO %s, STATUS %d \n", registro, status);
                while (status != R_ERROR){
                        //printf ("REGISTRO %s, STATUS %d \n", registro, status);
                        char* incidencia = vector_incidencia(registro, vector, cantidad_shingles, vector_vacio);
                        printf("IMPRIMO EN EL VECTOR DE INCIDENCIA de %s: %s \n", registro, incidencia);
                        char* vector_hashmin = creador_vector_hashmin(incidencia, cantidad_shingles, funciones);
                        printf("IMPRIMO EN EL VECTOR DE HASMIN de %s: %s \n", registro, vector_hashmin);
                        //matriz[w]=vector_hashmin;
                        //w++;
                        int res = R_WRITE (rel_hasmin, i , vector_hashmin);
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
        return rel_hasmin;
}

int el_main( int argc, char *argv[] ){

        int cantidad_archivos = devuelve_cantidad_archivos(argc, argv);
        //int cantidad_lideres;
        int fd_relativo_nombres = creador_relativo_archivos( argc, argv);
        int cantidad_clusters = 3;
        //int cantidad_clusters = argv[2];
        int cantidad_lideres = 2;
        char* lideres = selector_lideres(cantidad_clusters, cantidad_archivos, cantidad_lideres);

        char* lista_clusters = creador_clusters(cantidad_clusters, cantidad_archivos, lideres);
        //int cantidad_lideres = 20*log10(cantidad_archivos);

        printf("CREO EL ARBOL \n");

        abb_t* arbol_shingles = abb_crear(strcmp,NULL);

        printf("LLAMO AL CREADOR DE SHINGLES \n");

        llamar_a_creador(fd_relativo_nombres , arbol_shingles );
        printf("PASO LOS SHINGLES AL VECTOR \n");
        char** vector = pasar_a_vector(arbol_shingles);
        int tamano_arbol = abb_cantidad(arbol_shingles);
        printf("EL ARBOL TIENE %d SHINGLES \n", tamano_arbol);
        abb_destruir(arbol_shingles);

        int* funciones = generador_funciones_hasmin();

        printf("CREO EL RELATIVO DE INCIDENCIA \n");

        int fd_relativo_hasmin = creador_relativo_hashmin(fd_relativo_nombres, vector, cantidad_archivos, tamano_arbol, funciones, cantidad_clusters);
        printf(" RELATIVO DE INCIDENCIA CREADO \n");
        printf("LIDERES ELEGIDOS SON: %d", lideres);
        for(int u = 0; u < cantidad_lideres; u++){
        	printf("LIDER %d es el doc numero %d \n", u, lideres[u]);
        }
        short* matriz = creador_matriz_hashmin(fd_relativo_hasmin, cantidad_clusters, cantidad_lideres, lista_clusters, lideres);
        printf("MATRIZ DE HASHMIN CREADA \n");
        asignar_documento_a_cluster(matriz, fd_relativo_hasmin,);

        //free(vector); //hay que destruir cada shingle
        return 0;
}

int main( int argc, char *argv[] ){

        el_main(argc, argv);
		//printf("%d", 101);
		//printf("%d", (char)101);
		//short t = 23523;
		//printf("ESTO ES DE SHORT A CHAR %d \n",(short_a_char(t)));
        return 0;
}

