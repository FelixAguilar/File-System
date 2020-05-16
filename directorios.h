// Autores: Felix Aguilar, Adrian Bennasar, Alvaro Bueno.
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "fichero.h"

// Imprime los mensajes de ejecucion.
#define DEBUG 0

// Errores de directorios.
#define ERROR_CAMINO_INCORRECTO -1
#define ERROR_PERMISO_LECTURA -2
#define ERROR_NO_EXISTE_ENTRADA_CONSULTA -3
#define ERROR_NO_EXISTE_DIRECTORIO_INTERMEDIO -4
#define ERROR_PERMISO_ESCRITURA -5
#define ERROR_ENTRADA_YA_EXISTENTE -6
#define ERROR_NO_ES_UN_DIRECTORIO -7
#define ERROR_ACCESO_DISCO -8
#define ERROR_DIRECTORIO_NO_VACIO -9

// Constantes.
#define TAMFILA 100
#define TAMBUFFER (TAMFILA * 1000)

struct entrada
{
    char nombre[60];
    unsigned int ninodo;
};

struct UltimaEntrada
{
    char camino[512];
    int p_inodo;
};

int extraer_camino(const char *camino, char *inicial, char *final, char *tipo);
int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir,
                   unsigned int *p_inodo, unsigned int *p_entrada,
                   char reservar, unsigned char permisos);
int mi_creat(const char *camino, unsigned char permisos);
int mi_dir(const char *camino, char *buffer);
int mi_chmod(const char *camino, unsigned char permisos);
int mi_stat(const char *camino, struct STAT *p_stat);
int mi_write(const char *camino, const void *buf, unsigned int offset,
             unsigned int nbytes);
int mi_read(const char *camino, void *buf, unsigned int offset,
             unsigned int nbytes);
int mi_link(const char *camino1, const char *camino2);
int mi_unlink(const char *camino);
int mi_unlink_r(const char *camino);
void mostrar_error_directorios(int error);
