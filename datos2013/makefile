CFLAGS= -Wall --pedantic -g --std=c99
EXEC=shingles
OBJ=lista.o pila.o arbol.o heap.o relativo.o shingles.o
CC=gcc

all: $(EXEC)

lista.o: estructuras/lista.c estructuras/lista.h
	$(CC) $(CFLAGS) -c estructuras/lista.c

pila.o: estructuras/pila.c estructuras/pila.h
	$(CC) $(CFLAGS) -c estructuras/pila.c

heap.o: estructuras/heap.c estructuras/heap.h
	$(CC) $(CFLAGS) -c estructuras/heap.c
	
arbol.o: estructuras/arbol.c estructuras/arbol.h
	$(CC) $(CFLAGS) -c estructuras/arbol.c

relativo.o: estructuras/relativo.c estructuras/relativo.h
	$(CC) $(CFLAGS) -c estructuras/relativo.c



shingles.o: shingles.c
	$(CC) $(CFLAGS) -c shingles.c



$(EXEC): $(OBJ) shingles.c
	$(CC) $(CFLAGS) $(OBJ) shingles.c -o $(EXEC)

clean:
	-rm *.o
