#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "arbol.h"
#include "pila.h"

/* ******************************************************************
 *                        ESTRUCTURAS
 * *****************************************************************/

typedef struct nodo_abb{
	char* clave;
	void* dato;
	struct nodo_abb *izq, *der;
}nodo_abb_t;

struct abb{
	struct nodo_abb* raiz;
	size_t cantidad;
	abb_comparar_clave_t cmp;
	abb_destruir_dato_t destruir;
};

struct abb_iter{
	const struct nodo_abb* actual;
	pila_t* pila;
};

/* ******************************************************************
 *                  PRIMITIVAS DEL ARBOL
 * *****************************************************************/


char* strdup(const char *str){
	size_t n = strlen(str) + 1;
	char *dup = malloc(n);
	if (dup) {
		strcpy(dup, str);
	}
return dup;
}


// Crea un arbol binaroio de busqueda, recibe la funcion de comparacion de las claves 
// y la funcion de destruir dato

abb_t* abb_crear(abb_comparar_clave_t cmp, abb_destruir_dato_t destruir_dato){
	abb_t* arbol = malloc (sizeof(abb_t));
	if (!arbol) return NULL;
	arbol->cantidad = 0;
	arbol->cmp = cmp;
	arbol->destruir = destruir_dato;
	arbol->raiz = NULL;
	return arbol;
}

// Crea un nodo de arbol binario, recibe la clave y el dato del nodo

nodo_abb_t* nodo_abb_crear(const char* clave, void* dato){
	nodo_abb_t* nodo = malloc(sizeof(nodo_abb_t));
	if (!nodo) return NULL;
	nodo->der = NULL;
	nodo->izq = NULL;
	nodo->clave = strdup(clave);
	nodo->dato = dato;
	return nodo;
}

// Crea un nodo con su clave y su dato y lo guarda en el arbol.
// En caso de que la clave ya se encuentre en el arbol, reemplaza el dato por el nuevo
// Pre: El arbol fue creado

bool abb_guardar(abb_t* arbol, const char* clave, void* dato){
	if (!arbol->cmp) return NULL;
	
	if (arbol->cantidad == 0){
	//	printf("\t vacio-GUARDAR Clave %s\n",clave);
		nodo_abb_t* creado = nodo_abb_crear(clave, dato);
		arbol->raiz = creado;
		arbol->cantidad ++;
		return true;
	}
	
	nodo_abb_t* nodo = arbol->raiz;
	while ( arbol->cmp(nodo->clave, clave) != 0 ){
		if ( arbol->cmp(nodo->clave , clave) > 0){
			if (!nodo->izq){
		//		printf("\t 1-GUARDAR Clave %s\n",clave);
				nodo_abb_t* creado = nodo_abb_crear(clave, dato);
				if (!creado) return false;
				nodo->izq = creado;
				arbol->cantidad ++;
				return true;
			}
			nodo = nodo->izq;
		}
		else if (arbol->cmp(nodo->clave , clave) < 0){
			if (!nodo->der){
		//		printf("\t 2-GUARDAR Clave %s\n",clave);
				nodo_abb_t* creado = nodo_abb_crear(clave, dato);
				if (!creado) return false;
				nodo->der = creado;
				arbol->cantidad ++;
				return true;
			}
			nodo = nodo->der;
		}
	}
	if (arbol->cmp(nodo->clave, clave) == 0){
	//	printf("\t 0-GUARDAR Clave %s\n",clave);
		if (arbol->destruir){
			arbol->destruir(nodo->dato);
		}
		nodo->dato = dato;
		return true;
	}
	return false;
}	

// Devuelve la cantidad de nodos que tiene el arbol	
// Pre: el arbol fue creado
	
size_t abb_cantidad(abb_t* arbol){
	return arbol->cantidad;
}

void* abb_obtener(const abb_t* arbol, const char* clave){
	if (arbol->cantidad == 0) return NULL;
	nodo_abb_t* nodo = arbol->raiz;
	while ( arbol->cmp(nodo->clave, clave) != 0){
		if ( arbol->cmp(nodo->clave, clave) > 0){
		//	printf("\t OBTENER-IZQ Clave %s\n",clave);
			if (!nodo->izq) return NULL;
			nodo = nodo->izq;
		}
		
		else if ( arbol->cmp(nodo->clave, clave) < 0){
		//	printf("\t OBTENER-der Clave %s\n",clave);
			if (!nodo->der) return NULL;
			nodo = nodo->der;
		}
	}
	
	if ( arbol->cmp(nodo->clave, clave) == 0){
	//	printf("\t OBTENER Clave %s\n",clave);
		return nodo->dato;
	}
	return NULL;
}

// Devuelve true si la clave pertenece al arbol. False en caso contrario
// Pre: El arbol fue creado.

bool abb_pertenece(const abb_t *arbol, const char *clave){
	if (arbol->cantidad == 0) return false;
	nodo_abb_t* nodo = arbol->raiz;
	
	while ( arbol->cmp(nodo->clave, clave) != 0){
		if ( arbol->cmp(nodo->clave, clave) > 0){
			if (!nodo->izq) return false;
			nodo = nodo->izq;
		}
		
		else if ( arbol->cmp(nodo->clave, clave) < 0){
			if (!nodo->der) return false;
			nodo = nodo->der;
		}
	}
	
	if ( arbol->cmp(nodo->clave, clave) == 0){
		return true;
	}
	return false;
}

// Funcion recursiva que destruye el nodo y llama a la misma funcion para sus hijos
// Destruye tambien su dato llamando a la funcion de destruir dato del arbol.

void nodo_abb_destruir(abb_t* arbol, nodo_abb_t* nodo){
	if (nodo->izq) nodo_abb_destruir(arbol, nodo->izq);
	if (nodo->der) nodo_abb_destruir(arbol, nodo->der);
	if (arbol->destruir) arbol->destruir(nodo->dato);
	free(nodo->clave);
	free(nodo);
}

// Destruye el arbol llamando a la funcion nodo_abb_destruir

void abb_destruir(abb_t* arbol){
	if (arbol->cantidad == 0){
		free(arbol);
		return;
	}
	nodo_abb_destruir(arbol, arbol->raiz);
	free(arbol);
}

// Destruye un nodo del arbol y devuelve el dato.
// Recive la clave del nodo a destruir.
// En caso de que no exista ningun nodo con esa clave, devuelve NULL

void* abb_borrar(abb_t* arbol, const char* clave){
	if (!arbol->raiz) return NULL;
	nodo_abb_t* padre = NULL;
	nodo_abb_t* nodo = arbol->raiz;
	if (!nodo) return NULL;
	while ( arbol->cmp(nodo->clave, clave) != 0){
		if ( arbol->cmp(nodo->clave, clave) > 0){
			if (!nodo->izq) break;
			padre = nodo;
			nodo = nodo->izq;
		}
		
		else if ( arbol->cmp(nodo->clave, clave) < 0){
			if (!nodo->der) break;
			padre = nodo;
			nodo = nodo->der;
		}
	}
	
	if ( arbol->cmp(nodo->clave , clave) == 0){
		void* dato = nodo->dato;
		nodo_abb_t* borrar = nodo;
		if (!nodo->der && !nodo->izq){
			if (arbol->cmp(arbol->raiz->clave, clave) == 0 ) arbol->raiz = NULL;
			if (padre){
				if (padre->der == nodo) padre->der = NULL;
				if (padre->izq == nodo) padre->izq = NULL;
			}
			free(borrar->clave);
			free(borrar);
			nodo = NULL;
			arbol->cantidad--;
			return dato;
		}
		else if (nodo->izq != NULL && nodo->der == NULL){
			if (arbol->cmp(arbol->raiz->clave, clave) == 0 ) arbol->raiz = nodo->izq;
			if (padre){
				if (padre->der == nodo) padre->der = nodo->izq;
				if (padre->izq == nodo) padre->izq = nodo->izq;
			}
			nodo = nodo->izq;
			free(borrar->clave);
			free(borrar);
			arbol->cantidad--;
			return dato;
		}
		else if (nodo->der != NULL && nodo->izq == NULL){
			if (arbol->cmp(arbol->raiz->clave, clave) == 0 ) arbol->raiz = nodo->der;
			if (padre){
				if (padre->der == nodo) padre->der = nodo->der;
				if (padre->izq == nodo) padre->izq = nodo->der;
			}
			nodo = nodo->der;
			free(borrar->clave);
			free(borrar);
			arbol->cantidad--;
			return dato;
		}
		else if (nodo->izq != NULL && nodo->der != NULL){ // COPIANDO LA CLAVE Y EL DATO
			nodo_abb_t* reemplazo = nodo->der;
			nodo_abb_t* padre_reemplazo = nodo;
			while (reemplazo->izq){ //mientras tenga un hijo izquierdo
				padre_reemplazo = reemplazo;
				reemplazo = reemplazo->izq;
				if (!reemplazo) break;
			}
			if (reemplazo){
				padre_reemplazo->izq = reemplazo->der;
			}
			free(borrar->clave);
			borrar->clave = strdup(reemplazo->clave);
			free(reemplazo->clave);
			borrar->dato = reemplazo->dato;
			free(reemplazo);
			reemplazo = NULL;
			arbol->cantidad --;
			return dato;
		}
		return dato;
	}
	return NULL;
}

// Llama a la funcion recibida por paramatro para el nodo y para sus hijos.

void nodo_abb_in_order(nodo_abb_t* nodo, bool funcion(const char *, void *, void *), void *extra){
	if (!nodo) return;
	nodo_abb_in_order(nodo->izq, funcion, extra);
	funcion(nodo->clave, extra, nodo->dato);
	nodo_abb_in_order(nodo->der, funcion, extra);
}

// Llama a la funcion recibida por parametro para la raiz del arbol.

void abb_in_order(abb_t *arbol, bool funcion(const char *, void *, void *), void *extra){
	nodo_abb_in_order(arbol->raiz, funcion, extra);
}


/* ******************************************************************
 *               PRIMITIVAS DEL ITERADOR DE ARBOL
 * *****************************************************************/

// Crea un iterador de arbol y lo posiciona en el nodo mas a la derecha.
// Pre: El arbol fue creado.

abb_iter_t *abb_iter_in_crear(const abb_t *arbol){
	abb_iter_t* iter = malloc(sizeof(abb_iter_t));
	iter->actual = arbol->raiz;
	iter->pila = pila_crear();
	if (arbol->cantidad == 0) return iter;
	while (iter->actual->izq){ // Ubico el actual en la raiz mas a la izquierda
		pila_apilar(iter->pila , (void*) iter->actual);
		iter->actual = iter->actual->izq;
	}
	return iter;
}

// Avanza un nodo segun el recorrido in order.
// Pre: el iterador fue creado

bool abb_iter_in_avanzar(abb_iter_t *iter){
	if( !iter->actual) return false;
	if (!iter->actual->der){
		iter->actual = pila_desapilar(iter->pila);
		return true;
	}
	if (iter->actual->der->izq){
		pila_apilar(iter->pila, (void*) iter->actual->der);
		iter->actual = iter->actual->der->izq;
		while (iter->actual->izq){
			pila_apilar(iter->pila , (void*) iter->actual);
			iter->actual = iter->actual->izq;
		}
		return true;
	}
	else{
		iter->actual = iter->actual->der;
		return true;
	}
}

// Devuelve la clave del nodo donde esta parado el iterador.
// Si se encuentra al final, devuelve NULL.
// Pre: el iterador fue creado.
	
const char *abb_iter_in_ver_actual(const abb_iter_t *iter){
	if (!iter->actual) return NULL;
	return iter->actual->clave;
}

// Devuelve true si el iterador del arbol se encuentra al final
// En caso contrario, devuelve false.
// Pre: el iterador fue creado

bool abb_iter_in_al_final(const abb_iter_t *iter){
	if( !iter->actual) return true; 
	if (pila_esta_vacia(iter->pila) && iter->actual == NULL){
		return true;
	}
	return false;
}

// Destruye el iterador de arbol.
// Pre: el iterador fue creado.

void abb_iter_in_destruir(abb_iter_t* iter){
	pila_destruir(iter->pila);
	free(iter);
}

void abb_imprimir(abb_t* arbol){
	abb_iter_t* iter = abb_iter_in_crear(arbol);
	while (!abb_iter_in_al_final(iter)){
		printf(abb_iter_in_ver_actual(iter));
		printf("\n");
		abb_iter_in_avanzar(iter);
	}
}

