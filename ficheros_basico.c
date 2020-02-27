#include "ficheros_basico.h"

/* Funcion: tamMB
* --------------
* Devuelve el tamaño del espacio de disco ocupado por el mapa de bits en 
* bloques según el número de bloques en el disco.
*
*   nbloques: número de bloques en el disco.
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

/* Funcion: initSB
* ---------------
* Esta función inicializa las variables contenidas en el struct que forma el superbloque.
* 
* nbloques: número de bloques del sistema.
* ninodos: número de inodos del sistema.
*
* returns: EXIT_SUCCESS si se ha podido escribir el superbloque en posSB o EXIT_FAILURE
* si ha habido algún error con la función bwrite().
*/
int initSB(unsigned int nbloques, unsigned int ninodos)
{

   // Creamos una variable de tipo struct superbloque

   struct superbloque SB;

   // Posición del primer bloque del mapa de bits
   SB.posPrimerBloqueMB = SBPOS + SBTAM;

   // Posición del último bloque del mapa de bits
   SB.posUltimoBloqueMB = SB.posPrimerBloqueMB + tamMB(nbloques) - 1;

   // Posición del primer bloque del array de inodos
   SB.posPrimerBloqueAI = SB.posUltimoBloqueMB + 1;

   // Posición del último bloque del array de inodos
   SB.posUltimoBloqueAI = SB.posPrimerBloqueAI + tamAI(ninodos) - 1;

   // Posición del primer bloque de datos
   SB.posPrimerBloqueDatos = SB.posUltimoBloqueAI + 1;

   // Posición del último bloque de datos
   SB.posUltimoBloqueDatos = nbloques - 1;

   // Posición del inodo del directorio raíz en el array de inodos
   SB.posInodoRaiz = 0;

   // Posición del primer inodo libre en el array de inodos
   SB.posPrimerInodoLibre = 0;

   // Cantidad de bloques libres en el SF
   SB.cantBloquesLibres = nbloques;

   // Cantidad de inodos libres en el array de inodos
   SB.cantInodosLibres = ninodos;

   // Cantidad total de bloques
   SB.totBloques = nbloques;

   // Cantidad total de inodos
   SB.totInodos = ninodos;

   // Finalmente escribimos la estructura en el bloque posSB con bwrite()
   if (bwrite(SBPOS, &SB) == -1)
   {
      return EXIT_FAILURE;
   }
   return EXIT_SUCCESS;
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
    if (bread(SBPOS, &SB) == -1)
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
        if (bwrite(ind, buffer) == -1)
        {
            // Si no se ha podido escribir devueve error.
            perror("Error");
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

/* Funcion: intAI
* ---------------
* Esta función inicializa la lista de inodos libres.
*
* returns: EXIT_SUCCESS si ha ido bien y EXIT_FAILURE si ha habido algún error.
*/
int initAI()
{
    struct superbloque SB;
    if (bread(SBPOS, &SB) == -1)
    {
        perror("Error");
        return EXIT_FAILURE;
    }
    struct inodo inodos[BLOCKSIZE / INODOSIZE];

    int continodos = SB.posPrimerInodoLibre + 1;

    for (int i = SB.posPrimerBloqueAI; i <= SB.posUltimoBloqueAI; i++)
    {
        for (int j = 0; j < BLOCKSIZE / INODOSIZE; j++)
        {
            inodos[j].tipo = 'l';
            if (continodos < SB.totInodos)
            {
                inodos[j].punterosDirectos[0] = continodos;
                continodos++;
            }
            else
            {
                inodos[j].punterosDirectos[0] = UINT_MAX;
            }
        }
        if (bwrite(SBPOS, &inodos) == -1)
        {
            perror("Error");
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }
}
