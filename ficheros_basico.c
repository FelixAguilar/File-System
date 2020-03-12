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
* Esta función inicializa las variables contenidas en el struct que forma el 
* superbloque.
* 
* nbloques: número de bloques del sistema.
* ninodos: número de inodos del sistema.
*
* returns: EXIT_SUCCESS si se ha podido escribir el superbloque en posSB o 
* EXIT_FAILURE si ha habido algún error con la función bwrite().
*/
int initSB(unsigned int nbloques, unsigned int ninodos)
{

    // Creamos una variable de tipo struct superbloque.

    struct superbloque SB;

    // Posición del primer bloque del mapa de bits.
    SB.posPrimerBloqueMB = SBPOS + SBSIZE;

    // Posición del último bloque del mapa de bits.
    SB.posUltimoBloqueMB = SB.posPrimerBloqueMB + tamMB(nbloques) - 1;

    // Posición del primer bloque del array de inodos.
    SB.posPrimerBloqueAI = SB.posUltimoBloqueMB + 1;

    // Posición del último bloque del array de inodos.
    SB.posUltimoBloqueAI = SB.posPrimerBloqueAI + tamAI(ninodos) - 1;

    // Posición del primer bloque de datos.
    SB.posPrimerBloqueDatos = SB.posUltimoBloqueAI + 1;

    // Posición del último bloque de datos.
    SB.posUltimoBloqueDatos = nbloques - 1;

    // Posición del inodo del directorio raíz en el array de inodos.
    SB.posInodoRaiz = 0;

    // Posición del primer inodo libre en el array de inodos.
    SB.posPrimerInodoLibre = 0;

    // Cantidad de bloques libres en el SF (-1 por el superbloque).
    SB.cantBloquesLibres = nbloques - 1;

    // Cantidad de inodos libres en el array de inodos.
    SB.cantInodosLibres = ninodos;

    // Cantidad total de bloques.
    SB.totBloques = nbloques;

    // Cantidad total de inodos.
    SB.totInodos = ninodos;

    // Finalmente escribimos la estructura en el bloque posSB con bwrite().
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
    // Lee el superbloque del disco.
    struct superbloque SB;
    if (bread(SBPOS, &SB) == -1)
    {
        perror("Error");
        return EXIT_FAILURE;
    }

    // Reserva un espacio de memoria para el buffer de tamaño bloque.
    unsigned char *buffer = malloc(sizeof(char) * BLOCKSIZE);
    if (!buffer)
    {
        perror("Error");
        return EXIT_FAILURE;
    }

    // Pone todas las posiciones del bufer a cero.
    memset(buffer, 0, BLOCKSIZE);

    // Itera tantas veces como bloques haya en el mapa de bits del disco.
    for (int ind = SB.posPrimerBloqueMB; ind <= SB.posUltimoBloqueMB; ind++)
    {
        // Escribe el buffer en disco dejando el bloque a cero.
        if (bwrite(ind, buffer) == -1)
        {
            perror("Error");
            free(buffer);
            return EXIT_FAILURE;
        }
    }

    // Libera el buffer.
    free(buffer);

    // Marca como ocupado los bloques de metadatos indicados en el superbloque.
    for (unsigned int i = SBPOS; i <= SB.posUltimoBloqueAI; i++)
    {
        if (escribir_bit(i, 1))
        {
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
        if (bwrite(i, &inodos) == -1)
        {
            perror("Error");
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

/* Funcion: escribirbit:
* ----------------------
* Esta funcion permite modificar el valor de un bit del mapa de bits del disco.
*
*  nbloque: bloque que queremos indicar si esta libre u ocupado.
*  bit: valor del bit que queremos escribir.
*
* return: EXIT_SUCCESS si se ha podido escribir el bit sino EXIT_FAILURE.
*/
int escribir_bit(unsigned int nbloque, unsigned int bit)
{

    // Lee el superbloque del disco.
    struct superbloque SB;
    if (bread(SBPOS, &SB) == -1)
    {
        perror("Error");
        return EXIT_FAILURE;
    }

    // Declara e inicializa las variables a utilizar.
    unsigned int posbyte = nbloque / 8;
    unsigned int posbit = nbloque % 8;
    unsigned int nbloqueMB = posbyte / BLOCKSIZE;
    unsigned int nbloqueabs = SB.posPrimerBloqueMB + nbloqueMB;

    // Reserva espacio de memoria para el buffer del bloque.
    unsigned char *bufferMB = malloc(sizeof(char) * BLOCKSIZE);
    if (!bufferMB)
    {
        perror("Error");
        return EXIT_FAILURE;
    }

    // Leer del disco el bloque donde esta el bit.
    if (bread(nbloqueabs, bufferMB) == -1)
    {
        free(bufferMB);
        perror("Error");
        return EXIT_FAILURE;
    }

    // Calcula la posicion del byte dentro del bloque.
    posbyte = posbyte % BLOCKSIZE;

    //Genera la mascara para modificar el bit indicado por parametro.
    unsigned char mascara = 128;
    mascara >>= posbit;

    //Disponemos a cambiar el bit en el buffer del bloque.
    if (bit)
    {
        bufferMB[posbyte] |= mascara; // OR.
    }
    else
    {
        bufferMB[posbyte] &= ~mascara; // AND y NOT.
    }

    //Escribir el buffer en el disco.
    if (bwrite(nbloqueabs, bufferMB) == -1)
    {
        free(bufferMB);
        perror("Error");
        return EXIT_FAILURE;
    }

    free(bufferMB);
    return EXIT_SUCCESS;
}

/* Function: leer_bit:
* --------------------
* Esta funcion permite leer el valor de un bit del mapa de bits del disco.
*
*  nbloque: bloque que queremos saber si esta libre u ocupado.
*
* return: Valor del bit leido o bien -1 si se ha producido un error.
*/
unsigned char leer_bit(unsigned int nbloque)
{
    // Lee el superbloque del disco.
    struct superbloque SB;
    if (bread(SBPOS, &SB) == -1)
    {
        perror("Error");
        return EXIT_FAILURE;
    }

    // Declara e inicializa las variables a utilizar.
    unsigned int posbyte = nbloque / 8;
    unsigned int posbit = nbloque % 8;
    unsigned int nbloqueMB = posbyte / BLOCKSIZE;
    unsigned int nbloqueabs = SB.posPrimerBloqueMB + nbloqueMB;

    // Reserva espacio de memoria para el buffer del bloque.
    unsigned char *bufferMB = malloc(sizeof(char) * BLOCKSIZE);
    if (!bufferMB)
    {
        perror("Error");
        return EXIT_FAILURE;
    }

    // Leer del disco el bloque donde esta el bit.
    if (bread(nbloqueabs, bufferMB) == -1)
    {
        free(bufferMB);
        perror("Error");
        return EXIT_FAILURE;
    }

    // Calcula la posicion del byte dentro del bloque.
    posbyte = posbyte % BLOCKSIZE;

    //Genera la mascara para modificar el bit indicado por parametro.
    unsigned char mascara = 128;
    mascara >>= posbit;

    // Obtiene el bit del bloque que se ha indicado por parametro.
    mascara &= bufferMB[posbyte];
    mascara >>= (7 - posbit);

    free(bufferMB);
    return mascara;
}

/* Funcion: reservar_bloque:
* --------------------------
* Esta funcion reserva un bloque para ser usado si encuentra uno libre en el 
* mapa de bit.
*
* returns: Devuelve el numero de bloque si se ha podido reservar, si no -1.
*/
int reservar_bloque()
{
    // Lee el superbloque del disco.
    struct superbloque SB;
    if (bread(SBPOS, &SB) == -1)
    {
        perror("Error");
        return EXIT_FAILURE;
    }

    // Comprueba que haya bloques libres en el disco.
    if (!SB.cantBloquesLibres)
    {
        perror("Error");
        return -1;
    }

    // Reserva espacio de memoria para el buffer del bloque.
    unsigned char *bufferMB = malloc(sizeof(char) * BLOCKSIZE);
    if (!bufferMB)
    {
        perror("Error");
        return EXIT_FAILURE;
    }

    // Reserva espacio de memoria para la mascara del bloque
    unsigned char *bufferAux = malloc(sizeof(char) * BLOCKSIZE);
    if (!bufferAux)
    {
        perror("Error");
        free(bufferMB);
        return EXIT_FAILURE;
    }

    // Se inicia la mascara con todos los bits a 1.
    memset(bufferAux, 255, BLOCKSIZE);

    // Busca el primer bloque con algun bit a 0 en el mapa de bits.
    unsigned int posBloqueMB = 0;

    // Lee el primer bloque del mapa de bits.
    if (bread(SB.posPrimerBloqueMB + posBloqueMB, bufferMB) == -1)
    {
        free(bufferMB);
        free(bufferAux);
        perror("Error");
        return -1;
    }

    // Itera por todos los bloques de MB hasta encontrar uno con algun bit a 0.
    while (!memcmp(bufferMB, bufferAux, BLOCKSIZE) && posBloqueMB < SB.posUltimoBloqueMB)
    {
        posBloqueMB++;
        if (bread(SB.posPrimerBloqueMB + posBloqueMB, bufferMB) == -1)
        {
            free(bufferMB);
            free(bufferAux);
            perror("Error");
            return -1;
        }
    }

    // Continuar.
}