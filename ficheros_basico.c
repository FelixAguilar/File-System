#include "ficheros_basico.h"

/* Funcion: tamMB
* --------------
* Devuelve el tamaño del espacio de disco ocupado por el mapa de bits en 
* bloques segun el numero de bloques en el disco.
*
*   nbloques: numero de bloques en el disco.
*
*   returns: tamaño del mapa en bloques.
*/
int tamMB(unsigned int nbloques)
{
    int bits = nbloques / 8;
    int bloques = bits / BLOCKSIZE;
    if (bits % BLOCKSIZE)
    {
        return bloques + 1;
    }
    else
    {
        return bloques;
    }
}

/* Funcion: tamAI
* ---------------
* Esta función determina el tamaño del array de inodos en bloques.
* 
* ninodos: número de inodos del sistema.
*
* returns: bloques.
*/
int tamAI(unsigned int ninodos)
{
    int ibits = ninodos * INODOSIZE;
    int bloques = ibits / BLOCKSIZE;

    if (ibits % BLOCKSIZE)
    {
        return bloques + 1;
    }
    else
    {
        return bloques;
    }
}

/* Funcion: initMB
* ----------------
* Esta funcion inicia el mapa de bits del disco, actualizando los bloques del
* que pertenecen al mapa con el valor 0.
*
* returns: Exit_failure si ha habido algun problema o bien Exit_success si ha 
* sido correcto. 
*/
int initMB()
{
    // Reserva un espacio de memoria para el buffer de tamaño bloque.
    unsigned char *buffer = malloc(sizeof(char) * BLOCKSIZE);
    if (!buffer)
    {
        // Si no se ha podido reservar devueve error.
        perror("Error");
        return EXIT_FAILURE;
    }
    // Pone todas las posiciones del bufer a cero.
    memset(buffer, 0, BLOCKSIZE);

    // Obtiene el superbloque del disco
    struct superbloque SB;
    if (bread(SBPOS, &SB))
    {
        // Si no se ha podido leer devueve error.
        perror("Error");
        return EXIT_FAILURE;
    }

    // Itera tantas veces como bloques haya en el disco de mapa de bits.
    int ind = SB.posPrimerBloqueMB;
    while (ind <= SB.posUltimoBloqueMB)
    {
        // Escribe el buffer en disco dejando el bloque a cero.
        if (bwrite(ind, buffer))
        {
            // Si no se ha podido escribir devueve error.
            perror("Error");
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}