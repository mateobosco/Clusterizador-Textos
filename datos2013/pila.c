#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#include "pila.h"
#define constante2 50

/* ******************************************************************
 *                                      	    PILA
 * *****************************************************************/
struct _pila {
    void** datos;
    size_t tamanio;
    size_t cantidad;
};

/* ******************************************************************
 *                                      	    PILA_CREAR
 * *****************************************************************/

pila_t* pila_crear(){

	pila_t* pila = malloc(sizeof(pila_t));
	if (!pila) return NULL;
	pila->datos =  malloc (constante2 * sizeof(void*));
	if (!pila->datos){
		free(pila);
		return NULL;
	}
	pila->cantidad=0;
	pila->tamanio=constante2;
	return pila;
}

/* ******************************************************************
 *                                      	    PILA_DESTRUIR
 * *****************************************************************/

void pila_destruir(pila_t *pila){
		free(pila->datos); 
		free(pila);	
}

/* ******************************************************************
 *                                      	    PILA_ESTA_VACIA
 * *****************************************************************/

bool pila_esta_vacia(const pila_t *pila){
		if (pila->cantidad==0){
			return true;
		}
		return false;
}

/* ******************************************************************
 *                                      	    PILA_REDIMENSIONAR
 * *****************************************************************/

bool redimensionar(pila_t* pila, size_t tam_nuevo){
	
	//(pila->tamanio)+=tam_nuevo;
	void* aux = realloc( pila->datos , tam_nuevo * sizeof(void*));
	if ( aux == NULL ) {
		return false; //no se pudo aumentar el tamaño de la pila
	}else{
		pila->datos=aux;
		pila->tamanio=tam_nuevo;
	}
	return true;
}
	
/* ******************************************************************
 *                                      	    PILA_APILAR
 * *****************************************************************/

bool pila_apilar(pila_t *pila , void* valor){
	if (pila->cantidad >= pila->tamanio){
		if(redimensionar(pila, pila->tamanio*2)==false){
			return false; // comprueba que se haya podido redimensionar la pila
		}
	}
	pila->datos[pila->cantidad]=valor;
	pila->cantidad++;
	return true;
}

/* ******************************************************************
 *                                      	    PILA_VER_TOPE
 * *****************************************************************/			

void* pila_ver_tope(const pila_t *pila){
	if (pila->cantidad==0){
		return NULL;
	}	
	return pila->datos[pila->cantidad-1];
}

/* ******************************************************************
 *                                      	    PILA_DESAPILAR
 * *****************************************************************/

void* pila_desapilar(pila_t *pila){
	if (pila->cantidad==0){
		return NULL;
	}
	pila->cantidad-=1;
	if ( pila->cantidad < (pila->tamanio / 4)) {
		if(redimensionar(pila, pila->tamanio/2)==false){
			return false; // comprueba que se haya podido redimensionar la pila
		}
	}
	return pila->datos[pila->cantidad];
}
