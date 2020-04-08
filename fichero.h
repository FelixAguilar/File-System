// Autores: Felix Aguilar, Adrian Bennasar, Alvaro Bueno
#include <stdio.h>           // printf(), fprintf(), stderr, stdout, stdin.
#include <stdlib.h>          // exit(), EXIT_SUCCESS, EXIT_FAILURE, atoi().
#include <unistd.h>          // read(), write(), open(), close(), lseek().
#include <errno.h>           // errno.
#include <string.h>          // strerror().
#include <limits.h>          // límite numérico.
#include <time.h>            // para el guardado de tiempos.
#include "ficheros_basico.h" // enlace a la libreria ficheros_basico.h.

struct STAT
{
    char tipo;     // Tipo ('l':libre, 'd':directorio o 'f':fichero).
    char permisos; // Permisos (lectura y/o escritura y/o ejecución).

    time_t atime; // Fecha y hora del último acceso a datos: atime.
    time_t mtime; // Fecha y hora de la última modificación de datos: mtime.
    time_t ctime; // Fecha y hora de la última modificación del inodo: ctime.

    unsigned int nlinks;        // Numero de enlaces de entradas en directorio.
    unsigned int tamEnBytesLog; // Tamaño en bytes lógicos.
    unsigned int numBloquesOcupados;
};

int mi_write_f(unsigned int ninodo, const void *buf_original,
               unsigned int offset, unsigned int nbytes);
int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset,
              unsigned int nbytes);
int mi_stat_f(unsigned int ninodo, struct STAT *p_stat);
int mi_chmod_f(unsigned int ninodo, unsigned char permisos);
int mi_truncar_f(unsigned int ninodo, unsigned int nbytes);