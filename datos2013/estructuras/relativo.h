#ifndef RELATIVO_H
#define RELATIVO_H

#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

/* Generales */

#define    R_MAX_OPEN_FILES  20

/* Resultados */
#ifndef		R_OK
#define		R_OK		0
#endif

#define    R_ERROR		-1
	
/* Estado de las celdas */
#define    R_LIBRE        0
#define    R_OCUPADO      1
#define    R_BORRADO	  2

typedef struct t_cabecera {
	int long_reg;
	int max_reg;
} t_cabecera;

typedef struct t_datosAdm {
  short int estado;
} t_datosAdm;

typedef struct t_descriptorRel {
	int fd;
	int long_reg;
	int max_reg;
} t_descriptorRel;

t_descriptorRel* archivosAbiertos[R_MAX_OPEN_FILES];

int R_CREATE (char * nfisico, int long_reg, int max_reg);
int R_OPEN (char* nfisico,int modo);
int R_CLOSE (int handler);
int R_READ (int handler, int nrec, char* reg);
int R_READNEXT (int handler, char* reg);
int R_WRITE (int handler, int nrec, char* reg);
int R_SEEK (int handler, int nrec);
int R_UPDATE (int handler, int nrec, char* reg);
int R_DELETE (int handler, int nrec);

#endif

