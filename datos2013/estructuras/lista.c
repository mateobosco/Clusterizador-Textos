#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "lista.h"

/* ******************************************************************
 *                                      	    NODO
 * *****************************************************************/

struct nodo{
	void* dato;
	struct nodo *prox;
};

/* ******************************************************************
 *                                      	       LISTA
 * *****************************************************************/

struct lista{
	size_t largo;
	nodo_t* lista_primero;
	nodo_t* lista_ultimo;
};

/* ******************************************************************
 *                                      	       ITER
 * *****************************************************************/

struct iter{
	nodo_t* actual;
	nodo_t* anterior;
};

/* ******************************************************************
 *                                       Funciones Lista
 * *****************************************************************/

nodo_t* crear_nodo(void* valor){
	nodo_t* nodo= malloc (sizeof(nodo_t));
	if (!nodo) return NULL;
	nodo->prox=NULL;
	nodo->dato=valor;
	return nodo;
}

lista_t *lista_crear(){
	lista_t* lista = malloc (sizeof(lista_t));
	if (lista==NULL) return NULL;
	lista->largo=0;
	lista->lista_primero=NULL;
	lista->lista_ultimo=NULL;
	return lista;
}


bool lista_esta_vacia(const lista_t *lista){
	if (lista->largo==0)return true;
	return false;
}

bool lista_insertar_primero(lista_t* lista, void *dato){
	nodo_t* nodo=crear_nodo(dato);
	if (nodo==NULL) return false;
	if (lista->largo==0){
		lista->lista_ultimo=nodo;
		lista->lista_primero=nodo;
		lista->largo++;
		return true;
	}
	nodo_t* aux = lista->lista_primero;
	lista->lista_primero=nodo;
	lista->lista_primero->prox=aux;
	lista->largo++;
	return true;
}

bool lista_insertar_ultimo(lista_t* lista, void *dato){

	nodo_t* nodo=crear_nodo(dato);
	if (nodo==NULL) return false;
	if (lista->largo==0){
		lista->lista_primero=nodo;
		lista->lista_ultimo=nodo;
		lista->largo++;
		return true;
	}
	lista->lista_ultimo->prox=nodo;
	lista->lista_ultimo=nodo;
	lista->largo++;
	return true;
}

void *lista_borrar_primero(lista_t *lista){
	if (lista_esta_vacia(lista)) return NULL;
	nodo_t* aux=lista->lista_primero;
	void* el_dato=aux->dato;
	if (lista->largo == 1){
		lista->lista_ultimo = NULL;
	}
	lista->lista_primero=aux->prox;
	free(aux);
	lista->largo--;
	return el_dato;
}

void *lista_ver_primero(const lista_t *lista){
	if (lista->largo == 0) return NULL;
	return lista->lista_primero->dato;
}

size_t lista_largo(const lista_t *lista){
	return lista->largo;
}

void lista_destruir(lista_t *lista, void destruir_dato(void*)){
	if (lista->largo==0){
		free(lista);
		return;
	}
	while (!lista_esta_vacia(lista)){
		void *dato=lista_borrar_primero(lista);
		if (destruir_dato){
			destruir_dato(dato);
		}
	}
	free(lista);
	return;
}

/* ******************************************************************
 *           			FUNCIONES ITER
 * *****************************************************************/

lista_iter_t *lista_iter_crear(const lista_t *lista){
	lista_iter_t* iter=malloc(sizeof(lista_iter_t));
	if (!iter) return NULL;
	iter->anterior=NULL;
	iter->actual=lista->lista_primero;
	return iter;
}

bool lista_iter_avanzar(lista_iter_t *iter){
	
	// si no puede avanzar mas porque actual es NULL
	if (lista_iter_al_final(iter) || (!iter)) return false;
	nodo_t* siguiente=iter->actual;
	iter->anterior=iter->actual;
	iter->actual=siguiente->prox;
	return true;
}


void *lista_iter_ver_actual(const lista_iter_t *iter){
	if ((!iter) || !iter->actual) return NULL;
	return iter->actual->dato;
}

bool lista_iter_al_final (const lista_iter_t *iter){
	if((iter->actual==NULL)) return true;
	return false;
}

void lista_iter_destruir (lista_iter_t *iter){
	free(iter);
}

/* ******************************************************************
 *                           FUNCIONES ITER Y LISTA
 * *****************************************************************/

bool lista_insertar (lista_t *lista, lista_iter_t *iter, void *dato){
	if (lista == NULL) return false;
	
	if(lista_esta_vacia(lista)){
		lista_insertar_primero(lista,dato);
		iter->anterior=NULL;
		iter->actual=lista->lista_primero;
		return true;
	}
	if(lista_iter_al_final(iter)){
		lista_insertar_ultimo(lista, dato);
		iter->actual=lista->lista_ultimo;
		//printf("LLEGA HASTA ACA \n ");
		//iter->anterior->prox=lista->lista_ultimo;
		return true;
	}
	if (iter->anterior==NULL){
		lista_insertar_primero(lista, dato);
		iter->actual=lista->lista_primero;
		return true;
	}
	nodo_t* nuevo = crear_nodo(dato);
	nodo_t* aux = iter->actual;
	if (!nuevo) return false;
	iter->anterior->prox=nuevo;
	iter->actual=nuevo;
	iter->actual->prox=aux;
	lista->largo++;
	return true;
}

void *lista_borrar(lista_t *lista, lista_iter_t *iter){
	if (!lista || !iter) return false;
	if (lista_esta_vacia(lista)) return NULL;
	if (iter->actual == NULL) return NULL ;
	nodo_t* aux = iter->actual;
	void* dato= aux->dato;
	if (!iter->anterior){
		dato = lista_borrar_primero(lista);
		iter->actual=lista->lista_primero;
		return dato;
	}
	if (iter->actual->prox == NULL){
		iter->actual=iter->actual->prox;
		iter->anterior->prox=iter->actual;
		free(aux);
		lista->lista_ultimo=iter->anterior;
		lista->largo--;	
		return dato;
	}	

	iter->actual=iter->actual->prox;
	iter->anterior->prox=iter->actual;
	free(aux);
	lista->largo--;
	return dato;
}
