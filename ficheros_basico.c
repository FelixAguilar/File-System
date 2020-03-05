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
    SB.posPrimerBloqueMB = SBPOS + SBSIZE;

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
        // Si no se ha podido reservar devuelve error.
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
    for (int ind = SB.posPrimerBloqueMB; ind <= SB.posUltimoBloqueMB; ind++)
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
        if (bwrite(i, &inodos) == -1)
        {
            perror("Error");
            return EXIT_FAILURE;
        }
    }
    return EXIT_SUCCESS;
}

/* Funcion: leer_bit
* ---------------
* Esta función lee un determinado bit del MB.
* 
* nbloque: bloque físico que contiene el bit.
*
* returns: valor del bit leído.
*/
unsigned char leer_bit(unsigned int nbloque)
{

    // Reserva un espacio de memoria para el buffer de tamaño bloque.
    unsigned char *bufferMB = malloc(sizeof(char) * BLOCKSIZE);
    if (!bufferMB)
    {
        // Si no se ha podido reservar devuelve error.
        perror("Error");
        return EXIT_FAILURE;
    }

    // Lee el superbloque.
    struct superbloque SB;
    if (bread(SBPOS, &SB) == -1)
    {
        perror("Error");
        return EXIT_FAILURE;
    }

    // Definimos los valores.
    unsigned int posbyte = nbloque / 8;
    unsigned int posbit = nbloque % 8;
    unsigned int nbloqueMB = posbyte / BLOCKSIZE;
    unsigned int nbloqueabs = SB.posPrimerBloqueMB + nbloque;

    // Definimos la máscara.
    unsigned char mascara = 128;

    // Desplazamiento de bits a la derecha.
    mascara >>= posbit;

    posbyte = posbyte % BLOCKSIZE;
    // Operador AND para bits.
    mascara &= bufferMB[posbyte];

    // Desplazamiento de bits a la derecha.
    mascara >>= (7 - posbit);

    return mascara;
}

/* Funcion: liberar_bloque
* ---------------
* Esta función libera un bloque determinado.
* 
* nbloque: bloque físico a liberar.
*
* returns: número de bloque liberado
*/
int liberar_bloque(unsigned int nbloque)
{
    // Lee el superbloque.
    struct superbloque SB;
    if (bread(SBPOS, &SB) == -1)
    {
        perror("Error");
        return EXIT_FAILURE;
    }

    // Ponemos a 0 el bit correspondiente al nbloque.
    escribir_bit(nbloque, 0);

    // Aumentamos la cantidad de bloques libres.
    SB.cantBloquesLibres++;

    // Escribimos el nuevo superbloque.
    if (bwrite(SBPOS, &SB) == -1)
    {
        perror("Error");
        return EXIT_FAILURE;
    }

    return nbloque;
}

/* Funcion: escribir_bit
* ---------------
* Esta función permite modificar el valor de un bit del sistema de ficheros.
* 
* nbloque: bloque que queremos indicar si está libre u ocupado.
* bit: valor del bit a escribir.
*
* returns: EXIT_SUCCESS si se ha podido escribir el bit o -1 en caso de que se haya producido un error.
* 
*/
int escribir_bit(unsigned int nbloque, unsigned int bit)
{
    /* 
        Declaramos una variable para la posición del byte en el MB que contiene el bit que representa
        el nbloque y una variable para la posición del bit dentro de ese byte.
    */

    unsigned int posbyte;
    unsigned int posbit;

    /*
        Declaramos una variable para la posición en el MB del bloque donde se halla el bit que representa
        el nbloque para entonces leerlo y una variable para la posición absoluta del dispositivo virtual
        donde se encuentra ese bloque.
    */

    unsigned int nbloqueMB;
    unsigned int nbloqueabs;

    // Leemos el superbloque para obtener la localización del MB.
    struct superbloque SB;
    if (bread(SBPOS, &SB) == -1)
    {
        perror("Error");
        return -1;
    }

    posbyte = nbloque / 8;
    posbit = nbloque % 8;

    nbloqueMB = posbyte / BLOCKSIZE;
    nbloqueabs = SB.posPrimerBloqueMB + nbloqueMB;

    // Reserva un espacio de memoria para el buffer de tamaño bloque.
    unsigned char *bufferMB = malloc(sizeof(char) * BLOCKSIZE);
    if (!bufferMB)
    {
        // Si no se ha podido reservar devueve error.
        perror("Error");
        return -1;
    }
    // Pone todas las posiciones del bufer a cero.
    memset(bufferMB, 0, BLOCKSIZE);

    /*
        Ahora que ya tenemos ubicado el bit en el dispositivo, leemos el bloque que lo contiene y 
        cargamos el contenido en un buffer, bufferMB, en el que tendremos que modificar el bit deseado, 
        pero preservando el valor de los demás bits del bloque
    */

    if (bread(nbloqueabs, bufferMB) == -1)
    {
        fprintf(stderr, "%s", strerror(errno));
        return -1;
    }
    posbyte = posbyte % BLOCKSIZE;

    // Ahora que ya tenemos en memoria el byte, bufferMB[posbyte], podemos poner a 1 o a 0 el bit correspondiente.
    unsigned char mascara = 128;
    mascara >>= posbit; // desplazamiento de bits a la derecha

    if (bit == 1)
    {
        bufferMB[posbyte] |= mascara;
    }
    else if (bit == 0)
    {
        bufferMB[posbyte] &= ~mascara;
    }

    /*
     escribimos ese buffer del MB en el dispositivo virtual con bwrite() en la posición que habíamos calculado 
     anteriormente, nbloqueabs.
    */

    if (bwrite(nbloqueabs, bufferMB) == -1)
    {
        fprintf(stderr, "%s", strerror(errno));
        return -1;
    }
    return EXIT_SUCCESS;
}

/* Funcion: reservar_bloque()
* ---------------
* Esta función reserva un bloque para ser usado, si hay alguno libre.
* 
*
* returns: nbloque si se ha podido reservar un bloque o -1 si se ha producido un error.
* 
*/
int reservar_bloque()
{

    // Leemos el contenido del superbloque.
    struct superbloque SB;
    if (bread(SBPOS, &SB) == -1)
    {
        perror("Error");
        return -1;
    }

    // Declaramos una variable para recorrer los bloques del Mapa de Bits(MB).
    unsigned int posBloqueMB;

    // Comprobamos la variable del superbloque que nos indica si quedan bloques libres.
    if (SB.cantBloquesLibres > 0)
    {
        // Reserva un espacio de memoria para el buffer de tamaño bloque.
        unsigned char *bufferMB = malloc(sizeof(char) * BLOCKSIZE);
        if (!bufferMB)
        {
            // Si no se ha podido reservar devueve error.
            perror("Error");
            return -1;
        }
        // Pone todas las posiciones del bufer a cero.
        memset(bufferMB, 0, BLOCKSIZE);

        // Reserva un espacio de memoria para el buffer auxiliar.
        unsigned char *bufferAux = malloc(sizeof(char) * BLOCKSIZE);
        if (!bufferAux)
        {
            // Si no se ha podido reservar devueve error.
            perror("Error");
            return -1;
        }
        // Pone todas las posiciones del bufer a uno.
        memset(bufferAux, 255, BLOCKSIZE);

        // Variable para el resultado de memcomp

        int ret = 0;

        // Inicializamos la variable que se va a usar para recorrer los bloques del MB.
        posBloqueMB = SB.posPrimerBloqueMB;
        for (posBloqueMB; (posBloqueMB < SB.posUltimoBloqueMB) && (ret == 0); posBloqueMB++)
        {
            bread(posBloqueMB, bufferMB);
            ret = memcmp(bufferMB, bufferAux, BLOCKSIZE);
        }

        unsigned int posbyte = 0;
        for (posbyte = 0; (posbyte < BLOCKSIZE) && (bufferMB[posbyte] = 255); posbyte++)
        {
            posbyte++;
        }

        unsigned int posbit = 0;
        unsigned char mascara = 128;
        while (bufferMB[posbyte] & mascara)
        {
            posbit++;
            bufferMB[posbyte] <<= 1; // desplaz. de bits a la izqda
        }

        unsigned int nbloque = ((posBloqueMB - SB.posPrimerBloqueMB) * BLOCKSIZE + posbyte) * 8 + posbit;
        if (escribir_bit(nbloque, 1) == -1)
        {
            perror("Error");
        }

        SB.cantBloquesLibres--;

        // Reserva un espacio de memoria para el buffer de tamaño bloque.
        unsigned char *bufferCeros = malloc(sizeof(char) * BLOCKSIZE);
        if (!bufferMB)
        {
            // Si no se ha podido reservar devueve error.
            perror("Error");
            return -1;
        }
        // Pone todas las posiciones del bufer a cero.
        memset(bufferCeros, 0, BLOCKSIZE);

        if (bwrite(nbloque, bufferCeros) == -1)
        {
            fprintf(stderr, "%s", strerror(errno));
            return -1;
        }

        return nbloque;
    }
    else
    {
        printf("ERROR: No quedan bloques libres.");
        return -1;
    }
}
