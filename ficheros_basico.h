// Autores: Félix Aguilar, Adrián Bennasar, Álvaro Bueno
#include <stdio.h>   // printf(), fprintf(), stderr, stdout, stdin.
#include <stdlib.h>  // exit(), EXIT_SUCCESS, EXIT_FAILURE, atoi().
#include <unistd.h>  // SEEK_SET, read(), write(), open(), close(), lseek().
#include <errno.h>   // errno.
#include <string.h>  // strerror().
#include <limits.h>  // límite numérico.
#include <time.h>    // para el guardado de tiempos.
#include "bloques.h" // enlace a la libreria bloques.h.

#define SBPOS 0                                       // posición superbloque.
#define SBSIZE 1                                      // tamaño superbloque.
#define INODOSIZE 128                                 // bytes de un inodo.
#define NPUNTEROS (BLOCKSIZE / sizeof(unsigned int))  // 256 punteros.
#define DIRECTOS 12                                   // punteros.
#define INDIRECTOS0 (NPUNTEROS + DIRECTOS)            // 268 punteros.
#define INDIRECTOS1 (NPUNTEROS * NPUNTEROS + INDIRECTOS0) // 65.804 punteros.
#define INDIRECTOS2 (NPUNTEROS * NPUNTEROS * NPUNTEROS + INDIRECTOS1) // 16.843.020 punteros.

struct superbloque
{
    // Posición del primer bloque del mapa de bits en el SF.
    unsigned int posPrimerBloqueMB;
    // Posición del último bloque del mapa de bits en el SF.
    unsigned int posUltimoBloqueMB;
    // Posición del primer bloque del array de inodos en el SF.
    unsigned int posPrimerBloqueAI;
    // Posición del último bloque del array de inodos en el SF.
    unsigned int posUltimoBloqueAI;
    // Posición del primer bloque de datos en el SF.
    unsigned int posPrimerBloqueDatos;
    // Posición del último bloque de datos en el SF.
    unsigned int posUltimoBloqueDatos;
    // Posición del inodo del directorio raíz en el AI.
    unsigned int posInodoRaiz;
    // Posición del primer inodo libre en el AI.
    unsigned int posPrimerInodoLibre;
    // Cantidad de bloques libres del SF.
    unsigned int cantBloquesLibres;
    // Cantidad de inodos libres del SF.
    unsigned int cantInodosLibres;
    // Cantidad total de bloques del SF.
    unsigned int totBloques;
    // Cantidad total de inodos del SF.
    unsigned int totInodos;
    // Relleno para que ocupe 1 bloque.
    char padding[BLOCKSIZE - 12 * sizeof(unsigned int)];
};

struct inodo
{
    char tipo;     // Tipo ('l':libre, 'd':directorio o 'f':fichero).
    char permisos; // Permisos (lectura y/o escritura y/o ejecución).

    char reservado_alineacion1[6];
    time_t atime; // Fecha y hora del último acceso a datos.
    time_t mtime; // Fecha y hora de la última modificación de datos.
    time_t ctime; // Fecha y hora de la última modificación del inodo.

    unsigned int nlinks;             /* Cantidad de enlaces de entradas en 
                                        directorio.*/
    unsigned int tamEnBytesLog;      // Tamaño en bytes lógicos.
    unsigned int numBloquesOcupados; /* Cantidad de bloques ocupados en la zona 
                                        de datos.*/

    unsigned int punterosDirectos[12];  // 12 punteros a bloques directos.
    unsigned int punterosIndirectos[3]; /* 3 punteros a bloques indirectos: 
                                        1 indirecto simple, 1 indirecto doble,
                                        1 indirecto triple */

    // Variable de alineacion.
    char padding[INODOSIZE - 2 * sizeof(unsigned char) - 3 * sizeof(time_t) -
                 18 * sizeof(unsigned int) - 6 * sizeof(unsigned char)];
};

int tamMB(unsigned int nbloques);
int tamAI(unsigned int nbloques);
int initSB(unsigned int nbloques, unsigned int ninodos);
int initMB();
int initAI();
int escribir_bit(unsigned int nbloque, unsigned int bit);
unsigned char leer_bit(unsigned int nbloque);
int reservar_bloque();
int liberar_bloque(unsigned int nbloque);
int escribir_inodo(unsigned int ninodo, struct inodo inodo);
int leer_inodo(unsigned int ninodo, struct inodo *inodo);
int reservar_inodo(unsigned char tipo, unsigned char permisos);
int obtener_nrangoBL(struct inodo inodo, unsigned int nblogico, int *ptr);
int obtener_indice(unsigned int nblogico, unsigned int nivel_punteros);
int traducir_bloque_inodo(unsigned int ninodo, unsigned int nblogico,
                          char reservar);
int liberar_inodo(unsigned int ninodo);
int liberar_bloques_inodo(unsigned int primerBL, struct inodo *inodo);
int mi_truncar_f(unsigned int ninodo, unsigned int nbytes);
