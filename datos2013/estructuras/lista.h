
#ifndef LISTA_H
#define LISTA_H

#include <stdbool.h>
#include <stddef.h>

/* ******************************************************************
 *                DEFINICION DE LOS TIPOS DE DATOS
 * *****************************************************************/

/*La lista esta planteada como varios nodos que apuntan a punteros genericos y a su proximo nodo */

typedef struct nodo nodo_t;
typedef struct lista lista_t;
typedef struct iter lista_iter_t;

// Crea una lista y la devuevlve
// Post: devuelve una nueva lista vacía.
lista_t *lista_crear();

// Devuelve verdadero o falso, según si la lista tiene o no elementos.
// Pre: la lista fue creada.
bool lista_esta_vacia(const lista_t *lista);

// Devuelve verdadero o falso, segun si se puede insertar un nodo al principio de la lista.
// Pre: la lista fue creada.
bool lista_insertar_primero(lista_t* lista, void *dato);

// Devuelve verdadero o falso, segun si se puede insertar un elemento al final de la lista.
// Pre: la pila fue creada.
bool lista_insertar_ultimo(lista_t* lista, void *dato);

// Elimina el primer elemento de la lista y devuelve un puntero al dato del primer nodo.
// Pre: la lista fue creada.
void *lista_borrar_primero(lista_t *lista);

// Devuelve un puntero al primer elemento de la lista. Si la lista está vacia devuelve NULL.
// Pre: la lista fue creada.
void *lista_ver_primero(const lista_t *lista);

// Devuelve el largo de la lista.
// Pre: la lista fue creada.
size_t lista_largo(const lista_t *lista);

// Destruye la lista. Si se recibe la función destruir_dato por parámetro,
// para cada uno de los elementos de la lista llama a destruir_dato.
// Pre: la lista fue creada. destruir_dato es una función capaz de destruir
// los datos de la lista, o NULL en caso de que no se la utilice.
// Post: se eliminaron todos los elementos de la lista.
void lista_destruir(lista_t *lista, void destruir_dato(void*));

// Crea un iterador y lo devuelve. Recibe una lista creada
lista_iter_t *lista_iter_crear(const lista_t *lista);

// Devuelve true o false según si se puede hacer avanzar al iterador.
// Pre: el iterador fue creado.
bool lista_iter_avanzar(lista_iter_t *iter);

// Devuelve un puntero al elemento actual del iterador
// Pre: el iterador fue creado.
void *lista_iter_ver_actual(const lista_iter_t *iter);

// Devuelve true o false segun si el iterador se encuentra al final.
// Pre: el iterador fue creado.
bool lista_iter_al_final (const lista_iter_t *iter);

// Destruye el iterador
// Pre: el iterador fue creado.
void lista_iter_destruir (lista_iter_t *iter);

// Devuelve true o false segun si se puede insertar un elemento en la posicion actual del iterador
// Pre: el iterador fue creado.
bool lista_insertar (lista_t *lista, lista_iter_t *iter, void *dato);

// Devuelve un puntero al elemento en la posicion actual del iterador y lo saca de la lista.
// Pre: el iterador fue creado.
void *lista_borrar(lista_t *lista, lista_iter_t *iter);


#endif // LISTA_H
