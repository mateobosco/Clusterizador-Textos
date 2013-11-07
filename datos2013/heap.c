
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "heap.h"


#define CONSTANTE 500


/* ******************************************************************
 *                        ESTRUCTURAS
 * *****************************************************************/

 
struct heap_t{
	size_t cantidad;
	size_t tamanio;
	cmp_func_t cmp;
	void** vector;
	int ultimo;
};

/* ******************************************************************
 *                  PRIMITIVAS DEL HEAP
 * *****************************************************************/

/* Crea un heap. Recibe como único parámetro la función de comparación a
 * utilizar. Devuelve un puntero al heap, el cual debe ser destruido con
 * heap_destruir(). 
 */

heap_t* heap_crear(cmp_func_t cmp){
	heap_t* heap = malloc (sizeof (heap_t));
	if (!heap) return NULL;
	heap->cantidad=0;
	heap->tamanio= CONSTANTE;
	heap->cmp = cmp;
	heap->vector = malloc (CONSTANTE * sizeof (void*));
	return heap;
}

/* ******************       heap_destruir      ************************ */

/* Elimina el heap, llamando a la función dada para cada elemento del mismo.
 * El puntero a la función puede ser NULL, en cuyo caso no se llamará.
 * Post: se llamó a la función indicada con cada elemento del heap. El heap
 * dejó de ser válido. */

bool heap_destruir(heap_t *heap, void destruir_elemento(void *e)){
	int i = 0;
	if (!heap) return false;
	if (destruir_elemento){
		for (i=0; i < heap->cantidad; i++){
			destruir_elemento(heap->vector[i]);
		}
	}
	free(heap->vector);
	free(heap);
	return true;
}


/* ******************     heap_tamanio      ********************** */

/* Devuelve el tamanio de elementos que hay en el heap. */


size_t heap_tamanio( const heap_t* heap){
	return heap->tamanio;
}

/* ******************     heap_cantidad      ********************** */

/* Devuelve la cantidad de elementos que hay en el heap. */


size_t heap_cantidad(const heap_t *heap){
	return heap->cantidad;
}

/* ******************     heap_esta_vacio      ********************** */

bool heap_esta_vacio(const heap_t *heap){
	if (heap->cantidad == 0) return true;
	return false;
}

/* ******************     heap_redimensionar      ********************** */

void heap_redimensionar(heap_t* heap, double aumento){
	size_t tam_nuevo = (heap->tamanio) * aumento;
	void** datos_nuevo = realloc(heap->vector, tam_nuevo * sizeof(void*));
	heap->vector = datos_nuevo;
	heap->tamanio = tam_nuevo;
}
	
	
/* ******************     heap_encolar      ********************** */	
	
/* Agrega un elemento al heap. El elemento no puede ser NULL.
 * Devuelve true si fue una operación exitosa, o false en caso de error. 
 * Pre: el heap fue creado.
 * Post: se agregó un nuevo elemento al heap.
 */	
	
bool heap_encolar(heap_t* heap, void* elem){
	if (!elem) return false;

	if ( (double) heap_cantidad(heap) / (double) heap_tamanio(heap) >= 0.6 ){ //NUEVO
		heap_redimensionar(heap,2);
	}
	
	if (heap_cantidad(heap) == 0){
		heap->vector[0] = elem;
		heap->cantidad=1;
		return true;
	}
	//if (heap_pertenece(heap, elem)){
	//	printf("%s PERTENECE \n",elem);
	//	return false;
	//}
	printf("%s NO PERTENECE \n",elem);

	int posicion = (heap->cantidad);
	heap->vector[posicion]=elem; // ubico al final al elemento 
	int original = posicion;
	posicion = posicion /2;
	
	while (heap->cmp(elem, heap->vector[(posicion)]) > 0){ // comparo con el padre y si es mayor lo subo

		void* aux = heap->vector[posicion];
		heap->vector[posicion] = elem;
		heap->vector[original] = aux;
		original=posicion;
		posicion=posicion/2;
	}
	heap->cantidad++;	

	
	return true;
	
}
	
/* ******************     heap_ver_max      ********************** */	

/* Devuelve el elemento con máxima prioridad. Si el heap esta vacío, devuelve
 * NULL. 
 * Pre: el heap fue creado.
 */
	
void* heap_ver_max(const heap_t *heap){
	if (!heap) return NULL;
	if (heap_cantidad(heap) == 0 ) return NULL;
	return heap->vector[0];
}

/* ******************     mostrar_heap     ********************** */	

/* Imprime por pantalla el heap. */


void mostrar_heap(heap_t* heap){
	int i = 0;
	for (i = 0; i<= heap_cantidad(heap) ; i++){
		printf(" %d - %s \n", i, (char*) heap->vector[i]);
	}
}

/* ******************     downheap     ********************** */	

/* Funcion Downheap que recibe un heap y el comienzo desde donde hacer el downheap. */

void downheap(heap_t* heap, int i){
	while ( (i*2+2) < heap->cantidad ){ // mientras se este dentro del arreglo
		int hijo_mayor = -1;
		if (heap->cmp(heap->vector[i*2+1], heap->vector[(i*2)+2]) > 0){
			hijo_mayor = i*2+1;
		}
		if (heap->cmp(heap->vector[(i*2)+2], heap->vector[i*2+1]) > 0){
			hijo_mayor = (i*2)+2;
		}
		if (hijo_mayor == -1) return; // ninguno es mayor se queda ahi 
		
		if (heap->cmp(heap->vector[hijo_mayor], heap->vector[i]) > 0){
			void* aux = heap->vector[i];
			heap->vector[i] = heap->vector[hijo_mayor];
			heap->vector[hijo_mayor]= aux;
			i = hijo_mayor;
		}else return;
	}
}

/* ******************     heap_desencolar     ********************** */	

/* Elimina el elemento con máxima prioridad, y lo devuelve.
 * Si el heap esta vacío, devuelve NULL.
 * Pre: el heap fue creado.
 * Post: el elemento desencolado ya no se encuentra en el heap. 
 */

void* heap_desencolar(heap_t *heap){
	
	if (!heap) return NULL;
	if(heap_esta_vacio(heap)) return NULL;
	if(heap_cantidad(heap) == 1){
		void* aux =heap->vector[0];
		heap->vector[0] = NULL;
		heap->cantidad--;
		return aux;
	} 
	if ( (double) heap_cantidad(heap) / (double) heap_tamanio(heap) <= 0.2 && heap_tamanio(heap) > CONSTANTE){
		heap_redimensionar(heap, 0.5);

	}
	void*  primero = heap->vector[0];	
	void* ultimo = heap->vector[heap->cantidad-1];
	heap->vector[0] = ultimo;
	int i = 0;
	
	downheap(heap,i);
	
	heap->cantidad--;		
	heap->ultimo--;

	return primero;
}




/* ******************     downheap     ********************** */	

/* Funcion Downheap que recibe un arreglo, el comienzo desde donde hacer el downheap y el final. */

void downheap2(void *elementos[], int i, int fin, cmp_func_t cmp ){

	if (fin == 1 ){
		if (cmp(elementos[fin], elementos[i]) > 0 ) {
			void* aux = elementos[i];
			elementos[i] = elementos [fin];
			elementos[fin] = aux;
		}
	}
	while ( (i*2+2) <= fin ){ // mientras se este dentro del arreglo
		int hijo_mayor = -1;
		if (cmp(elementos[i*2+1], elementos[(i*2)+2]) > 0){
			hijo_mayor = i*2+1;
		}
		if (cmp(elementos[(i*2)+2],elementos[i*2+1]) > 0){
			hijo_mayor = (i*2)+2;
		}
		if (hijo_mayor == -1) return; // ninguno es mayor se queda ahi 
		
		if (cmp(elementos[hijo_mayor],elementos[i]) > 0){
			void* aux = elementos[i];
			elementos[i] = elementos[hijo_mayor];
			elementos[hijo_mayor]= aux;
			i = hijo_mayor;
		}else return;
	}
}

/* ******************     Heapify     ********************** */	

/* Función que se encarga de llevar a la forma de un heap a un arreglo*/
	

void heapify (void *elementos[], size_t cant, cmp_func_t cmp){
	
	int start = cant;
	//int ultimo = cant;
	while (start >= 0 ){
		downheap2(elementos, start, cant, cmp);
		start--;
	}


	
}

/* ******************     Heapsort     ********************** */	

/* Función de heapsort genérica. Esta función ordena mediante heap_sort
 * un arreglo de punteros opacos, para lo cual requiere que se
 * le pase una función de comparación. Modifica el arreglo "in-place".
 * Notar que esta función NO es formalmente parte del TAD Heap.
 */
	
	
void heapsort(void *elementos[], size_t cant, cmp_func_t cmp){

	cant--; // le resto uno a la cantidad porque la uso para marcar hasta donde va el vector y C incluye el 0.
	heapify(elementos, cant, cmp);
	while (cant>0){
		void* primero = elementos[0];
		
		void* ultimo = elementos[cant];
		
		elementos[0] = ultimo;
		elementos[cant] = primero;
		
		cant--;

		downheap2(elementos, 0, cant, cmp);
		
	}
	
}	
	
	
/* ******************     mostrar_vector     ********************** */	

/* Imprime por pantalla el vector. */


void mostrar_vector(void* elementos[], size_t cantidad){
	int i = 0;
	for (i=0; i <= cantidad; i++){
		printf (" %d - %s \n", i, (char*) elementos[i]);
	}
}
	

bool heap_pertenece(heap_t* heap, void* elem){
	if (heap->cantidad == 0) return false;
	int i = 0;
	mostrar_heap(heap);
	while (true){ // mientras se este dentro del arreglo

		int hijo_mayor = -1;
		if ( (i*2+1) >= heap->cantidad ) break;
		if (heap->cmp(heap->vector[i*2+1], elem) > 0  ){
			hijo_mayor = i*2+1;
		}
		if ( (i*2+2) >= heap->cantidad ) break;
		if (heap->cmp(heap->vector[(i*2)+2], elem) > 0){
			hijo_mayor = (i*2)+2;
		}
		if (heap->cmp(heap->vector[i*2+1], elem) == 0){
			return true;
		}
		if (heap->cmp(heap->vector[i*2+2], elem) == 0){
			return true;
		}
		if (hijo_mayor == -1){
			printf("devuelve false porque hijo mayor es menos 1 \n");
			return false ; // ninguno es mayor se queda ahi
		}
		if (heap->cmp(heap->vector[hijo_mayor], elem) == 0){
			return true;
		}
			i=hijo_mayor;
	}
	printf("devuelve false porque sale del while \n");
	return false;

}
	
	
	
	
