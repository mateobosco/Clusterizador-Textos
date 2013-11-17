#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
 #include <unistd.h>
#include "relativo.h"

int R_CREATE (char* nfisico, int long_reg, int max_reg)
{
	int fd,ok,i,j;
	t_descriptorRel* nuevo_arch;
	t_cabecera* cabecera;
	t_datosAdm datosAdm;
	char *aux;

	i=0;
	while ((i<R_MAX_OPEN_FILES)&&(archivosAbiertos[i]!=NULL))
		i++;
 
	if (i>=R_MAX_OPEN_FILES) {
		return R_ERROR;
	}

	fd = open(nfisico, O_RDWR | O_CREAT | O_EXCL, 0666);

	if (fd<0) {
		return R_ERROR;
	}

	cabecera = (t_cabecera *)malloc(sizeof(t_cabecera));
	cabecera->long_reg = long_reg;
	cabecera->max_reg = max_reg;

	ok = write(fd,(void *)cabecera,sizeof(t_cabecera));
	if (ok != sizeof(t_cabecera)) {
		free(cabecera);
		return R_ERROR;
	}

	aux = (char*)malloc(long_reg);
	datosAdm.estado = R_LIBRE;

	for(j=0; j<max_reg; j++) {
		ok = write(fd, (char*)&datosAdm, sizeof(t_datosAdm));
		if (ok != sizeof(t_datosAdm)) {
			free(aux);
			return R_ERROR;
		}

		ok = write(fd, aux, long_reg);
		if (ok != long_reg) {
			free(aux);
			return R_ERROR;
		}
	}

	nuevo_arch=(t_descriptorRel *)malloc(sizeof(t_descriptorRel));
	nuevo_arch->fd=fd;
	nuevo_arch->long_reg=cabecera->long_reg;
	nuevo_arch->max_reg=cabecera->max_reg;
	archivosAbiertos[i]=nuevo_arch;

	free(aux);
	free(cabecera);
	return i;
}

int R_OPEN (char *nfisico, int modo)
{
	int fd,ok,i;
	t_descriptorRel* nuevo_arch;
	t_cabecera* cabecera;

	i=0;
	while ((i<R_MAX_OPEN_FILES)&&(archivosAbiertos[i]!=NULL)) {
 		i++;
	}
 
 	if (i>=R_MAX_OPEN_FILES) {
		return R_ERROR;
	}

	fd = open(nfisico,modo);
	if (fd<0) {
		return R_ERROR;
	}

	cabecera=(t_cabecera *)malloc(sizeof(t_cabecera));
	ok=read(fd,(void *)cabecera,sizeof(t_cabecera));
	if (ok<0) {
		free(cabecera);
		return R_ERROR;
	}

	nuevo_arch = (t_descriptorRel *)malloc(sizeof(t_descriptorRel));
	nuevo_arch->fd=fd;
	nuevo_arch->long_reg=cabecera->long_reg;
	nuevo_arch->max_reg=cabecera->max_reg;
	archivosAbiertos[i]=nuevo_arch;
	free(cabecera);
	return i;
}

int R_CLOSE (int handler)
{
	int ok;
	ok = close(archivosAbiertos[handler]->fd);
	if (ok<0) {
 		return R_ERROR;
	}

	free(archivosAbiertos[handler]);
	archivosAbiertos[handler]=NULL;

	return R_OK;
}

int R_READ(int handler, int nrec, char* reg)
{
	int pos, res, long_celda;
	t_descriptorRel *arch;
	t_datosAdm datosAdm;

	arch = archivosAbiertos[handler];
	if (nrec > arch->max_reg)	{
		return R_ERROR;
	}

	long_celda = sizeof(t_datosAdm) + arch->long_reg;
	pos = (long_celda*nrec) + sizeof(t_cabecera);
	res = lseek(arch->fd, pos, 0);
	if (res<0) {
		return R_ERROR;
	}

	res = read(arch->fd, (char *) &datosAdm, sizeof(t_datosAdm));
	if (res == sizeof(t_datosAdm) && datosAdm.estado == R_OCUPADO) {
		res = read(arch->fd, reg, arch->long_reg);
		if (res == arch->long_reg) {
			return R_OK;
		}
	}	

	return R_ERROR;
}

int R_READNEXT (int handler, char* reg)
{
	int res1, res2, long_celda;
	t_descriptorRel *arch;
	t_datosAdm datosAdm;

	arch = archivosAbiertos[handler];

	long_celda = sizeof(t_datosAdm) + arch->long_reg;
	do {
		res1 = read(arch->fd, (char *)&datosAdm, sizeof(t_datosAdm));
		res2 = read(arch->fd, reg, arch->long_reg);
	} while ((res1==sizeof(t_datosAdm)) && (datosAdm.estado!=R_OCUPADO));

	if (res2 == arch->long_reg) {
		return R_OK;
	}

	return R_ERROR;
}

int R_WRITE(int handler, int nrec, char* reg)
{
	int pos, res, long_celda;
	t_descriptorRel *arch;
	t_datosAdm datosAdm;

	arch = archivosAbiertos[handler];
	if (nrec > arch->max_reg)	{
		return R_ERROR;
	}

	long_celda = sizeof(t_datosAdm) + arch->long_reg;
	pos = (long_celda*nrec + sizeof(t_cabecera));
	res = lseek(arch->fd, pos, 0);
	if (res<0) {
		return R_ERROR;
	}

	res = read(arch->fd, (char *)&datosAdm, sizeof(t_datosAdm));
	if ((res == sizeof(t_datosAdm)) && (datosAdm.estado != R_OCUPADO)) {

		res = lseek(arch->fd, pos, 0);
		if (res<0) {
			return R_ERROR;
		}

		datosAdm.estado=R_OCUPADO;
		res = write(arch->fd, (char *)&datosAdm, sizeof(t_datosAdm));

		if (res == sizeof(t_datosAdm)) {
			res = write(arch->fd, reg, arch->long_reg);
			
			if (res==arch->long_reg) {
				return R_OK;
			}
		}
    }	

	return R_ERROR;
}
		   
int R_SEEK (int handler, int nrec)
{
	int pos, res, long_celda;
	t_descriptorRel *arch;

	arch = archivosAbiertos[handler];

	if (nrec > arch->max_reg) {
		return R_ERROR;
	}

	long_celda = sizeof(t_datosAdm) + arch->long_reg;
	pos = (long_celda*nrec) + sizeof(t_cabecera);
	res = lseek(arch->fd ,pos, 0);
	if (res<0) {
		return R_ERROR;
	}

	return R_OK;
}

int R_UPDATE(int handler, int nrec, char* reg)
{
	int pos, res, long_celda;
	t_descriptorRel *arch;
	t_datosAdm datosAdm;

	arch = archivosAbiertos[handler];
	if (nrec > arch->max_reg)	{
		return R_ERROR;
	}

	long_celda = sizeof(t_datosAdm) + arch->long_reg;
	pos = (long_celda*nrec + sizeof(t_cabecera));
	res = lseek(arch->fd, pos, 0);
	if (res<0) {
		return R_ERROR;
	}

	res = read(arch->fd, (char *)&datosAdm, sizeof(t_datosAdm));
	if ((res == sizeof(t_datosAdm)) && (datosAdm.estado == R_OCUPADO)) {

		res = lseek(arch->fd, pos, 0);
		if (res<0) {
			return R_ERROR;
		}

		datosAdm.estado=R_OCUPADO;
		res = write(arch->fd, (char *)&datosAdm, sizeof(t_datosAdm));

		if (res == sizeof(t_datosAdm)) {
			res = write(arch->fd, reg, arch->long_reg);
			
			if (res==arch->long_reg) {
				return R_OK;
			}
		}
    }	

	return R_ERROR;
}

int R_DELETE (int handler, int nrec)
{
	int pos, res, long_celda;
	t_descriptorRel *arch;
	t_datosAdm datosAdm;

	arch = archivosAbiertos[handler];
	if (nrec > arch->max_reg)	{
		return R_ERROR;
	}

	long_celda = sizeof(t_datosAdm) + arch->long_reg;
	pos = (long_celda*nrec + sizeof(t_cabecera));
	res = lseek(arch->fd, pos, 0);
	if (res<0) {
		return R_ERROR;
	}

	res = read(arch->fd, (char *)&datosAdm, sizeof(t_datosAdm));
	if ((res == sizeof(t_datosAdm)) && (datosAdm.estado == R_OCUPADO)) {

		res = lseek(arch->fd, pos, 0);
		if (res<0) {
			return R_ERROR;
		}

		datosAdm.estado=R_BORRADO;
		res = write(arch->fd, (char *)&datosAdm, sizeof(t_datosAdm));

		if (res == sizeof(t_datosAdm)) {
			return R_OK;
		}
    }	

	return R_ERROR;
}
