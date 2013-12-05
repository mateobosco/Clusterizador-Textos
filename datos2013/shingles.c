#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include <math.h>
#include <getopt.h>

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

cluster_t* cluster_crear(int centro){
	cluster_t* cluster= malloc (sizeof(cluster_t));
	if (!cluster) return NULL;
	cluster->centro=centro;
	lista_t* lista_elementos = lista_crear();
	cluster->lista_elementos = lista_elementos;
	bool status = lista_insertar_primero(lista_elementos, centro);
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

char* mi_strcat(const char * str1, const char * str2){
        int largo1 = strlen(str1);
        int largo2 = strlen(str2);
        char* fin = malloc (sizeof(char)*(largo1+largo2+10));
        char barra[]="/";
        strcat(fin,str1);
        strcat(fin,barra);
        strcat(fin,str2);
        return fin;
}




int devuelve_cantidad_archivos( int argc , char* directorio){
        int cantidad = 0;
        DIR *dir;
        struct dirent *mi_dirent;
        //if( argc != 3 ){
        //		//printf("ENTRA ACAA \n");
         //       printf( "%s: %s directorio\n", argv[0], argv[0] );
          //      exit( -1 );
        //}
        if( (dir = opendir( directorio )) == NULL ){
        		//printf("ENTRA ACAA2 \n");
        		perror( "opendir" );
                exit( -1 );
        }
        while( (mi_dirent = readdir( dir )) != NULL ){
        		//printf("ENTRA ACAA3 \n");
        		int aux = NELEMS(mi_dirent->d_name);
                if(mi_dirent->d_name[0]!= '.'){ //NEGRADA. Y PROBLEMA SI HAY UNA CARPETA
                        cantidad ++;
                }
        }
        //printf("termina bien \n");
        return cantidad;
}

int creador_relativo_archivos( int argc, char* directorio ){
        remove("relativo_nombres"); //MEDIO VISHERO PERO ANDA
        DIR *dir;
        struct dirent *mi_dirent;

        //if( argc != 3 ){
         //       printf( "%s: %s directorio\n", argv[0], argv[0] );
//
  //              exit( -1 );
    //    }

        if( (dir = opendir( directorio )) == NULL ){
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
                        printf( "agrego a la lista: %s\n", mi_strcat(directorio, mi_dirent->d_name) );
                        lista_insertar_primero(lista_aux, mi_strcat(directorio, mi_dirent->d_name));
                        cantidad ++;
                }
        }
        printf("cantidad de archivos %d \n", cantidad);
        printf("largo de la lista %d \n", (int) lista_largo(lista_aux));
        printf("longitud maxima %d \n", long_max);


        int rel_nombres;
        char nombres[] = "relativo_nombres";
        // ELEGIR 'J' LIDERES EN ESTA PARTE.
        //int j = 20*log10(cantidad); // J lideres1
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
    int numero = (funcion * entrada)%1000000000;
	return numero;
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
                        //printf("Busco este shingle en el vector: %s\n",shingle_new);
                        int posicion = busq_binaria(vector_shingles, shingle_new, tamano_vector);
                        //printf("La busqueda binaria devuelve: %d \n", posicion);
                        if (posicion >= 0){
                                incidencia[posicion] = '1';
                                //printf("Encuentro un shingle en vector \n");
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
                funciones[i] = (rand() % 1000000000);
                printf(" %d ",funciones[i]);
        }
        printf("\n");
        return funciones;
}

int char_a_int(char* numero){
	int valor;
	unsigned char num0 = numero[0];
	unsigned char num1 = numero[1];
	unsigned char num2 = numero[2];
	unsigned char num3 = numero[3];
	valor =(int) pow(2,24) * num0 + (int) pow(2,16) * num1 + (int) pow(2,8) * num2 + (int) num3;
	return valor;
}

char* int_a_char(int numero){
	char* resultado = malloc(sizeof(char) * 5);
	resultado[0] = ( numero / pow(2,24));
	resultado[1] = ( ( numero - (resultado[0] * pow(2,24))) / pow(2,16));
	resultado[2] = ( ( numero - (resultado[0] * pow(2,24)) - (resultado[2] * pow(2,16)) ) / pow(2,8));
	resultado[3] =  numero - (resultado[0] * pow(2,24)) - (resultado[2] * pow(2,16)) - (resultado[3] * pow(2,8))  ;
	resultado[4] = '-';
//	int aux = numero;
//	for(int i = 0; i<4 ; i++){
//		int divisor = pow(2,(3-i)*8);
//		resultado[i] = aux / divisor;
//		aux = aux - resultado[i];
//	}

	return resultado;
}

char* creador_vector_hashmin(char* vector_incidencia, int cantidad_shingles, int* funciones){
        char* vector_hasmin = malloc (sizeof(char) * CANTIDAD_FUNCIONES * 4 *10);
        for (int j = 0; j<CANTIDAD_FUNCIONES ; j++){
                int anterior = 1000000000;
                for (int k = 0 ; k < cantidad_shingles ; k++){
                        char valor = vector_incidencia[k];
                        //printf("VECTOR INCIDENCIA J ES : %d \n", vector_incidencia[j]);
                        if (valor == '1'){
                                int nuevo = hash( k+1 , funciones[j] );
                        		//printf("nuevo ESSSSSSSSSS: %d", nuevo);
                                if (nuevo < anterior && nuevo>0){
                                	anterior = nuevo;
                                }
                        }
                }
                char* numero = int_a_char(anterior);
                vector_hasmin[j * 3] = numero[0];
                vector_hasmin[j * 3 + 1] = numero[1];
                vector_hasmin[j * 3 + 2] = numero[2];
                vector_hasmin[j * 3 + 3] = numero[3];
                vector_hasmin[j * 3 + 4] = numero[4];
        }
        printf("EL VECTOR HASHMIN QUEDA ASI:  \n");
        for(int i =0 ;i<CANTIDAD_FUNCIONES*5 ; i++){
        	printf("%d " , vector_hasmin[i]);
        }
        printf("\n");
        return vector_hasmin;
}

float jaccard (int* vector1, int* vector2){
	int iguales=0;

	for(int i=0; i<CANTIDAD_FUNCIONES; i++){
		if (vector1[i] == vector2[i]){
			iguales++;
		}
	}
	return (iguales*1.0)/CANTIDAD_FUNCIONES;
}

int* selector_lideres(int cantidad_clusters, int cantidad_archivos, int cantidad_lideres){
	int* lista_lideres = malloc(sizeof(int)*cantidad_lideres);
	for (int z=0; z < cantidad_lideres; z++){
		int lider = rand() % cantidad_archivos;
		lista_lideres[z]=lider;
	}
	return lista_lideres;
}


void** creador_vector_clusters(int cantidad_clusters, int cantidad_archivos, int* lideres, int cantidad_lideres){
	//elegir J lideres
	/**********************************************
	 * ESTO DEBERIA DEVOLVER UN VOID** Y NO TENGO LA MAS PUTA IDEA DE PORQUE NO SE PUEDE
	 */

	int centro;
	void** vector_clusters = malloc(cantidad_clusters * sizeof(cluster_t*));
	for(int i=0; i< cantidad_clusters; i++){
		centro = lideres[i];
		cluster_t* cluster = cluster_crear(centro);
		void* puntero = vector_clusters[i];
		vector_clusters[i] = cluster;
	}
	return vector_clusters;
	//elegir k clusters
}

int obtener_centro(cluster_t* cluster){
	//int centro = cluster->centro;
	return (cluster->centro);
}

lista_t* obtener_lista_elementos(cluster_t* cluster){
	//int centro = cluster->centro;
	return (cluster->lista_elementos);
}

int* vector_a_int(char* vector){
	int* salida = malloc(sizeof(int) * CANTIDAD_FUNCIONES);
	char numero[5];
	for (int i = 0 ; i < CANTIDAD_FUNCIONES ; i++){
		numero[0] = vector[i*3];
		numero[1] = vector[i*3+1];
		numero[2] = vector[i*3+2];
		numero[3] = vector[i*3+3];
		numero[4] = vector[i*3+4];
		salida[i] = char_a_int(numero);
	}
	return salida;
}

int** creador_matriz_hashmin(int rel_hashmin, void** lista_clusters, int cantidad_clusters, int cantidad_lideres,  int* lideres){
		int** matriz = malloc(sizeof(int*)*cantidad_clusters); // K
		int status;
		int centro;
		cluster_t* cluster;
		char* registro = malloc(CANTIDAD_FUNCIONES * 5 * 10 * sizeof(char)); // hiper mega villero;
		for(int i = 0; i< cantidad_clusters; i++){
			cluster = lista_clusters[i];
			printf("ESTE ES EL CENTRO DE CLUSTER %d,  %d \n", i, obtener_centro(cluster));
			centro = obtener_centro(cluster);
			status = R_READ(rel_hashmin, centro, registro);
			printf("escribo en la matriz esto %s en la posicion %d \n", registro, i);
			int* registro_int = vector_a_int(registro);
			matriz[i] = registro_int;

		}
		free(registro);
		//printf("MATRIZ[0] ES %d \n", matriz[0]);
		return matriz;
}

void asignar_documentos_a_clusters(int** matriz, int rel_hashmin, int cantidad_clusters, int* lideres, void** lista_clusters, bool agregar_doble){
	int similitud = 0;
	int status;
	int mas_parecido;
	int mas_parecido2;
	float similitud_aux;
	int j = 0;
	bool status2;
	char* registro_char = malloc (sizeof(char)* CANTIDAD_FUNCIONES * 5 *10);
	int* registro_int;
	status = R_READ(rel_hashmin, j, registro_char);
	registro_int = malloc(sizeof(int) * CANTIDAD_FUNCIONES * 10);
	while (status != R_ERROR){
		for (int i=0; i < cantidad_clusters; i++ ){
			registro_int = vector_a_int(registro_char);

			similitud_aux = jaccard(registro_int, matriz[i]);
			printf(" JACCARD DEVUELVE ESTO %f con el cluster %d \n ",similitud_aux, i);

			if (similitud_aux > similitud){
				similitud = similitud_aux;
				mas_parecido =  i;
				mas_parecido2 = i;
			}
			if(similitud == similitud_aux){
				mas_parecido2=i;
			}
		}
		if (similitud == 0){
			mas_parecido=rand()%(cantidad_clusters-1);
			printf("NO SE PARECE A NINGUNO, ASIGNO RANDOM A %d \n",mas_parecido);
		}
		similitud = 0;
		cluster_t* cluster = lista_clusters[mas_parecido];
		lista_t* lista_elementos = obtener_lista_elementos(cluster);
		int centro = obtener_centro(cluster);
		printf("CENTRO DEL CLUSTER %d\n",centro);
		if(centro != j){
			status2 = lista_insertar_primero(lista_elementos, j );
		}
		status2 = lista_insertar_primero(lista_elementos, j );

		if (agregar_doble == true && mas_parecido!=mas_parecido2){
			cluster_t* cluster2 = lista_clusters[mas_parecido];
			lista_t* lista_elementos2 = obtener_lista_elementos(cluster2);
			int centro2 = obtener_centro(cluster2);
			if(centro2 != j){
				status2 = lista_insertar_primero(lista_elementos2, j );
			}
		}
		printf ("AGREGO AL CLUSTER %d, el elemento %d \n", mas_parecido, j);
		j++;
		status = R_READNEXT(rel_hashmin, registro_char);
	}
	//free(registro_char); //free del diablo
	free(registro_int);
}


int creador_relativo_hashmin(int fd_relativo_nombres, char** vector, int cantidad_archivos, int cantidad_shingles, int* funciones, int cantidad_lideres){
        remove("relativo_hasmin");
        int rel_hasmin;
        char nombres[] = "relativo_hasmin";
        printf("cantidad de archivos %d \n" ,cantidad_archivos);
        rel_hasmin = R_CREATE(nombres, CANTIDAD_FUNCIONES*sizeof(char)*5*10, cantidad_archivos);
        char* vector_vacio = malloc(sizeof(char) * (cantidad_shingles+1));
       // char* matriz_hashmin = malloc(sizeof(char) * (cantidad_lideres));
        for (int i = 0 ; i < cantidad_shingles ; i++){
                vector_vacio[i] = '0';
        }
        vector_vacio[cantidad_shingles] = '\0';
        char* registro = malloc(100 * sizeof(char));
        int status;
        int i = 0;
        if(R_SEEK(fd_relativo_nombres,0) >= R_OK ){
                status = R_READNEXT(fd_relativo_nombres, registro); // ACA ESTA EL PROBLEMA
                printf ("REGISTRO %s, STATUS %d \n", registro, status);
                while (status != R_ERROR){
                        //printf ("REGISTRO %s, STATUS %d \n", registro, status);
                        char* incidencia = vector_incidencia(registro, vector, cantidad_shingles, vector_vacio);
                        //printf("IMPRIMO EN EL VECTOR DE INCIDENCIA de %s: %s \n", registro, incidencia);
                        char* vector_hashmin = creador_vector_hashmin(incidencia, cantidad_shingles, funciones);
                        //printf("IMPRIMO EN EL VECTOR DE HASMIN de %s: %s \n", registro, vector_hashmin);
                        //matriz[w]=vector_hashmin;
                        //w++;
                        printf("largo del relativo %d, escribo en la posicion %d \n" ,cantidad_archivos,i);
                        int res = R_WRITE (rel_hasmin, i , vector_hashmin);
                        if (res < 0){
                                perror("Error de escritura en el archivo relativo de hasmin");
                                //remove("relativo_nombres"); //FALTA LIBERAR MEMORIA ACA
                                //return -1;
                        }
                        status = R_READNEXT(fd_relativo_nombres, registro);
                        i ++;
                }
        }
        //free(registro); // no probe pero seguro se rompe, (igual que el anterior)
        free(vector_vacio);
        //free(registro);
        return rel_hasmin;
}

bool recalcular_centros(int fd_relativo_hasmin, void** vector_clusters, int cantidad_clusters){
	bool iterar = true;

	char* registro_char = malloc (sizeof(char)* CANTIDAD_FUNCIONES * 5 *10);
	int* registro_int;
	int* acumulador = malloc (sizeof(int)* CANTIDAD_FUNCIONES * 10);

	for (int i=0; i < cantidad_clusters; i++){
		for(int w=0; w< CANTIDAD_FUNCIONES; w++){
			acumulador[w]= 0;
		}
		int status;
		int similitud=9999;
		int mas_parecido;
		cluster_t* cluster = vector_clusters[i];
		int centro = obtener_centro(cluster);
		lista_t* lista_elementos = obtener_lista_elementos(cluster);
		size_t cantidad = lista_largo(lista_elementos);
		lista_iter_t* mi_iterador = lista_iter_crear(lista_elementos);
		while(!lista_iter_al_final(mi_iterador) ){
	    	//

			int numero = lista_iter_ver_actual(mi_iterador);
			//printf("HOLA PUTO2 \n");
			status = R_READ(fd_relativo_hasmin, numero , registro_char);
			int documento = lista_iter_ver_actual(mi_iterador);
		    registro_int = vector_a_int(registro_char);

		    //printf("IMPRIMO EL VECTOR HASHMIN: ");
		    //for(int j=0; j< CANTIDAD_FUNCIONES; j++){
		    //	printf("%d ", registro_int[j]);
		    //	acumulador[j]+= registro_int[j];
		    //}
		    //printf("\n");
		    //printf("\n");

			lista_iter_avanzar(mi_iterador);
		}
		for(int x=0; x< CANTIDAD_FUNCIONES; x++){
			//printf("SE ROMPE ACA LOCOOOOO \n");
			acumulador[x]= acumulador[x]/cantidad;

		}

		free(mi_iterador);
		lista_iter_t* iter = lista_iter_crear(lista_elementos);

		while(!lista_iter_al_final(iter) ){

			status = R_READ(fd_relativo_hasmin,lista_iter_ver_actual(mi_iterador), registro_char);
			registro_int = vector_a_int(registro_char);

			int similitud_aux = jaccard(registro_int, acumulador );
			//printf("LLEGA HASTA ACA \n");
			if (similitud_aux < similitud){
				similitud = similitud_aux;
				mas_parecido =  i;
			}
			if (centro != mas_parecido){
				iterar=false;
			}
			lista_iter_avanzar(iter);
		}
		//printf("LLEGA HASTA ACAaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa \n");
		free(iter);
		cluster->centro = mas_parecido;


	}

	free(registro_int);
	free(registro_char);
	return iterar;
}


int el_main( int argc, char* directorio, int cantidad_clusters, bool agregar_doble ){
		//bool agregar_doble = false; //ACA HAY QUE CAMBIAR CON LO QUE SE RECIBE POR PARAMETRO
		printf("ARGC es %d", argc);
		printf("La Cantidad de Clusters es: %d \n", cantidad_clusters);
		//cantidad_clusters = atoi(cantidad_clusters);;
		bool iterar;


		//if (argc == 3){
		//	k = (int)atoi(cantidad_clusters);/
//
	//	}
		//else{
		//	k = 4;
		//}
		//int j = 20*log10(cantidad);s
		printf("La Cantidad de Clusters es: %d \n", cantidad_clusters);

        int cantidad_archivos = devuelve_cantidad_archivos(argc, directorio);
        printf("La Cantidad de archivos es: %d \n", cantidad_archivos);
        int fd_relativo_nombres = creador_relativo_archivos( argc, directorio);
        //cantidad_clusters = k;
        //int cantidad_lideres = 3;
        int cantidad_lideres = 20*log10(cantidad_archivos);
        if (cantidad_lideres > cantidad_archivos){
        	printf("CAMBIA LA CANTIDAD D LIDERES \n");
        	cantidad_lideres = cantidad_archivos;
        }
        if (cantidad_clusters > cantidad_archivos){
        	printf("CAMBIA LA CANTIDAD D CLUSTERS \n");
        	cantidad_clusters = cantidad_archivos;
        }
        /*
        int cantidad_lideres = 20*log10(cantidad_archivos);
        if (cantidad_lideres > cantidad_archivos){
        	cantidad_lideres = cantidad_archivos;
        }
        */
        int* lideres = selector_lideres(cantidad_clusters, cantidad_archivos, cantidad_lideres);
        void** vector_clusters = creador_vector_clusters(cantidad_clusters, cantidad_archivos, lideres, cantidad_lideres);
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
        printf("CREO EL RELATIVO DE hashmin \n");
        int fd_relativo_hasmin = creador_relativo_hashmin(fd_relativo_nombres, vector, cantidad_archivos, tamano_arbol, funciones, cantidad_clusters);
        printf(" RELATIVO DE hashmin CREADO \n");
       // printf("LIDERES ELEGIDOS SON: %d", lideres);
        for(int u = 0; u < cantidad_lideres; u++){
        	printf("LIDER %d es el doc numero %d \n", u, lideres[u]);
        }
        // HACER EL WHILE NO CAMBIEN LOS CENTROS/
        // crear vector centros

        int** matriz = creador_matriz_hashmin(fd_relativo_hasmin, vector_clusters, cantidad_clusters, cantidad_lideres, lideres);
        printf("MATRIZ DE HASHMIN CREADA \n");
        asignar_documentos_a_clusters(matriz, fd_relativo_hasmin, cantidad_clusters, lideres, vector_clusters, agregar_doble);
        //free(vector); //hay que destruir cada shingle
        printf("LOS TEXTOS QUEDARON ASIGNADOS DE LA SIGUIENTE MANERA: \n");
        for (int i = 0; i<cantidad_clusters; i++){
        	printf("CLUSTER %d \n", i);
        	cluster_t* cluster = vector_clusters[i];
        	lista_t* elementos = obtener_lista_elementos(cluster);
        	lista_iter_t* mi_iterador = lista_iter_crear(elementos);
        	while( !lista_iter_al_final(mi_iterador) ){
        		printf("texto%d ", lista_iter_ver_actual(mi_iterador));
        		lista_iter_avanzar(mi_iterador);
        	}
        	printf("\n");

        }

        iterar = recalcular_centros(fd_relativo_hasmin, vector_clusters, cantidad_clusters);
        return 0;
}

int main( int argc, char *argv[] ){

		/*int numero = 1000000000;
		char* vector = int_a_char(numero);
		printf("paso a char %d \n",vector[0]);
		printf("paso a char %d \n",vector[1]);
		printf("paso a char %d \n",vector[2]);
		printf("paso a char %d \n",(unsigned char)vector[3]);
		printf("paso a char %d \n",vector[4]);
		printf("EL NUMERO ES %d \n", char_a_int(int_a_char(numero)));
		printf("EL NUMERO ES %d \n", char_a_int(int_a_char(char_a_int(int_a_char(numero)))));
		*/
		bool agregar_doble;
        int siguiente_opcion;
        int cantidad_clusters;
        char* directorio;
		for (int i = 0; i < argc; i++){
			printf("argv[%d] es %s \n",i, argv[i]);
		}
		//printf("argv[0] es %s \n", argv[0]);
		//printf("argv[1] es %s \n", argv[1]);
		const char* const op_cortas = "dcolahg";
		const struct option op_largas[] = {
				{ "dir", 1, NULL, 'd' },
				{ "cantidad", 1, NULL, 'c' },
				{ "repetido", 1, NULL, 'o' },
				{ "listar", 0, NULL, 'l' },
				{ "agregar", 1, 0, 'a' },
				{"listargrupos", 0, 0, 'g' },
				{ NULL, 0, NULL, 0 }
		};

		while (1) {
				/* Llamamos a la función getopt */
				siguiente_opcion = getopt_long(argc, argv, "d:c:o:", op_largas, NULL);

				if (siguiente_opcion == -1)
					break; /* No hay más opciones. Rompemos el bucle */

				switch (siguiente_opcion) {
				case 'd': /* -d lee el path donde estan almacenados los docs */
					printf("caso d con %s como argumento \n", optarg);
					directorio = optarg;
					break;

				case 'c': /* -c recibe cantidad d clusters por parametro */
					//imprime_ayuda();
					cantidad_clusters = atoi(optarg);
					printf("caso c con %d como argumento \n", cantidad_clusters);
					//exit(EXIT_SUCCESS);
					break;

				case 'o': /* indica si un documento puede estar en mas de un grupo */
					printf("caso o con %s como argumento \n", optarg);
					if(optarg == "Y"){
						agregar_doble = true;
					}
					else{
						agregar_doble = false;
					}
					break;
					// HACER QE UN DOC PUEDA ESTAR EN MAS D UN GRUPO O NO
					//exit(EXIT_FAILURE);

				case 'l': /* LISTA TODOS LOS DOCS Y CLUSTER AL CUAL PERTENECE */
					break;


				case 'g': /* LISTA LOS GRUPOS EXISTENTES Y LOS DOCS DENTRO DE CADA GRUPO */
					break;

				default: /* Algo más? No esperado. Abortamos */
					abort();
					break;
				}
		}
		printf("LLEGA HASTA ACA \n");
		el_main(argc, directorio, cantidad_clusters, agregar_doble);
		return 0;

}

