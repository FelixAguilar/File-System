#include <stdio.h>   // printf(), fprintf(), stderr, stdout, stdin
#include <stdlib.h>  // exit(), EXIT_SUCCESS, EXIT_FAILURE, atoi()
#include <unistd.h>  // SEEK_SET, read(), write(), open(), close(), lseek()
#include <errno.h>   // errno
#include <string.h>  // strerror()
#include <limits.h>  // límite numérico
#include <time.h>    // para el guardado de tiempos
#include "bloques.h" // enlaze a la libreria bloques.h

#define SBPOS 0       // posicion superbloque
#define SBSIZE 1      // tamaño superbloque
#define INODOSIZE 128 // bytes

struct superbloque
{
    // Posición del primer bloque del mapa de bits en el SF
    unsigned int posPrimerBloqueMB;
    // Posición del último bloque del mapa de bits en el SF
    unsigned int posUltimoBloqueMB;
    // Posición del primer bloque del array de inodos en el SF
    unsigned int posPrimerBloqueAI;
    // Posición del último bloque del array de inodos en el SF
    unsigned int posUltimoBloqueAI;
    // Posición del primer bloque de datos en el SF
    unsigned int posPrimerBloqueDatos;
    // Posición del último bloque de datos en el SF
    unsigned int posUltimoBloqueDatos;
    // Posición del inodo del directorio raíz en el AI
    unsigned int posInodoRaiz;
    // Posición del primer inodo libre en el AI
    unsigned int posPrimerInodoLibre;
    // Cantidad de bloques libres del SF
    unsigned int cantBloquesLibres;
    // Cantidad de inodos libres del SF
    unsigned int cantInodosLibres;
    // Cantidad total de bloques del SF
    unsigned int totBloques;
    // Cantidad total de inodos del SF
    unsigned int totInodos;
    // Relleno para que ocupe 1 bloque
    char padding[BLOCKSIZE - 12 * sizeof(unsigned int)];
};

typedef union _inodo {
    struct inodo
    {
        char tipo;     // Tipo ('l':libre, 'd':directorio o 'f':fichero)
        char permisos; // Permisos (lectura y/o escritura y/o ejecución)

        /* 
        Por cuestiones internas de alineación de estructuras, si se está 
        utilizando un tamaño de palabra de 4 bytes (microprocesadores de 32 
        bits): unsigned char reservado_alineacion1 [2]; en caso de que la 
        palabra utilizada sea del tamaño de 8 bytes (microprocesadores de 64 
        bits): unsigned char reservado_alineacion1 [6]; 
        */

        char reservado_alineacion1[6];
        time_t atime; // Fecha y hora del último acceso a datos: atime
        time_t mtime; // Fecha y hora de la última modificación de datos: mtime
        time_t ctime; // Fecha y hora de la última modificación del inodo: ctim

        unsigned int nlinks; // Cantidad de enlaces de entradas en directorio
        unsigned int tamEnBytesLog; // Tamaño en bytes lógicos.
        unsigned int numBloquesOcupados; // Bloques ocupados zona de datos

        unsigned int punterosDirectos[12];  // 12 punteros a bloques directos
        unsigned int punterosIndirectos[3]; /* 3 punteros a bloques indirectos:
        time_t 1 indirecto simple, 1 indirecto doble, 1 indirecto triple */
    };
    char padding[INODOSIZE];
} inodo_t;

int tamMB(unsigned int nbloques);
int tamAI(unsigned int nbloques);
int initSB(unsigned int nbloques, unsigned int ninodos);
int initMB();
int initAI();