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
    int bloques = ibits/BLOCKSIZE;


    if (ibits % BLOCKSIZE)
    {
        return bloques + 1;
    }
    else
    {
        return bloques;
    }
}
