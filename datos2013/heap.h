#ifndef _HEAP_H
#define _HEAP_H

#include <stdbool.h>  /* bool */
#include <stddef.h>	  /* size_t */

/* Prototipo de funci�n de comparaci�n que se le pasa como par�metro a las
 * diversas funciones del heap.
 * Debe recibir dos punteros del tipo de dato utilizado en el heap, y
 * debe devolver:
 *   menor a 0  si  a < b
 *       0      si  a == b
 *   mayor a 0  si  a > b
 */
typedef int (*cmp_func_t) (const void *a, const void *b);


/* Funci�n de heapsort gen�rica. Esta funci�n ordena mediante heap_sort
 * un arreglo de punteros opacos, para lo cual requiere que se
 * le pase una funci�n de comparaci�n. Modifica el arreglo "in-place".
 * Notar que esta funci�n NO es formalmente parte del TAD Heap.
 */
void heapsort(void *elementos[], size_t cant, cmp_func_t cmp);

/*
 * Implementaci�n de un TAD cola de prioridad, usando un max-heap.
 *
 * Notar que al ser un max-heap el elemento mas grande ser� el de mejor
 * prioridad. Si se desea un min-heap, alcanza con invertir la funci�n de
 * comparaci�n.
 */

/* Tipo utilizado para el heap. */
typedef struct heap_t heap_t;

/* Crea un heap. Recibe como �nico par�metro la funci�n de comparaci�n a
 * utilizar. Devuelve un puntero al heap, el cual debe ser destruido con
 * heap_destruir(). 
 */
heap_t *heap_crear(cmp_func_t cmp);

/* Elimina el heap, llamando a la funci�n dada para cada elemento del mismo.
 * El puntero a la funci�n puede ser NULL, en cuyo caso no se llamar�.
 * Post: se llam� a la funci�n indicada con cada elemento del heap. El heap
 * dej� de ser v�lido. */
bool heap_destruir(heap_t *heap, void destruir_elemento(void *e));

/* Devuelve la cantidad de elementos que hay en el heap. */
size_t heap_cantidad(const heap_t *heap);

/* Devuelve true si la cantidad de elementos que hay en el heap es 0, false en
 * caso contrario. */
bool heap_esta_vacio(const heap_t *heap);

/* Agrega un elemento al heap. El elemento no puede ser NULL.
 * Devuelve true si fue una operaci�n exitosa, o false en caso de error. 
 * Pre: el heap fue creado.
 * Post: se agreg� un nuevo elemento al heap.
 */
bool heap_encolar(heap_t *heap, void *elem);

/* Devuelve el elemento con m�xima prioridad. Si el heap esta vac�o, devuelve
 * NULL. 
 * Pre: el heap fue creado.
 */
void *heap_ver_max(const heap_t *heap);

/* Elimina el elemento con m�xima prioridad, y lo devuelve.
 * Si el heap esta vac�o, devuelve NULL.
 * Pre: el heap fue creado.
 * Post: el elemento desencolado ya no se encuentra en el heap. 
 */
void *heap_desencolar(heap_t *heap);

void mostrar_heap(heap_t* heap);

#endif // _HEAP_H

