// Autores: Félix Aguilar, Adrián Bennasar, Álvaro Bueno
#include "ficheros_basico.h"

/* Función: tamMB:
* ----------------
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

/* Función: tamAI:
* ----------------
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

/* Función: initSB:
* -----------------
* Esta función inicializa las variables contenidas en el struct que forma el 
* superbloque.
* 
* nbloques: número de bloques del sistema.
* ninodos: número de inodos del sistema.
*
* returns: EXIT_SUCCESS si se ha podido escribir el superbloque en posSB o 
*          EXIT_FAILURE si ha habido algún error con la función bwrite().
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

/* Función: initMB:
* -----------------
* Esta función inicia el mapa de bits del disco, actualizando los bloques del
* que pertenecen al mapa con el valor 0.
*
* returns: Exit_failure si ha habido algún problema o bien Exit_success si ha 
*          sido correcto. 
*/
int initMB()
{
    // Lee el superbloque del disco.
    struct superbloque SB;
    if (bread(SBPOS, &SB) == -1)
    {
        return EXIT_FAILURE;
    }

    // Reserva un espacio de memoria para el buffer de tamaño bloque.
    unsigned char *buffer = malloc(sizeof(char) * BLOCKSIZE);
    if (!buffer)
    {
        return EXIT_FAILURE;
    }

    // Pone todas las posiciones del buffer a cero.
    memset(buffer, 0, BLOCKSIZE);

    // Itera tantas veces como bloques haya en el mapa de bits del disco.
    for (int ind = SB.posPrimerBloqueMB; ind <= SB.posUltimoBloqueMB; ind++)
    {
        // Escribe el buffer en disco dejando el bloque a cero.
        if (bwrite(ind, buffer) == -1)
        {
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
            return EXIT_FAILURE;
        }
    }

    // Actualiza la cantidad de bloques libres en el superbloque.
    SB.cantBloquesLibres = SB.cantBloquesLibres - ((SB.posUltimoBloqueMB + 1) -
                                                   SB.posPrimerBloqueMB);
    if (bwrite(SBPOS, &SB) == -1)
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/* Función: intAI:
* ----------------
* Esta función inicializa la lista de inodos libres.
*
* returns: EXIT_SUCCESS si ha ido bien y EXIT_FAILURE si ha habido algún error.
*/
int initAI()
{
    // Lee el superbloque.
    struct superbloque SB;
    if (bread(SBPOS, &SB) == -1)
    {
        return EXIT_FAILURE;
    }

    // Inicializa estructura de inodos.
    struct inodo inodos[BLOCKSIZE / INODOSIZE];
    int continodos = SB.posPrimerInodoLibre + 1;

    // Itera dentro de todos los bloques del array de inodos.
    for (int i = SB.posPrimerBloqueAI; i <= SB.posUltimoBloqueAI; i++)
    {
        // Itera dentro de cada estructura de inodos.
        for (int j = 0; j < BLOCKSIZE / INODOSIZE; j++)
        {
            // Iniciliza el contenido del inodo.
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

        // Escribe el inodo en el dispositivo.
        if (bwrite(i, &inodos) == -1)
        {
            return EXIT_FAILURE;
        }
    }

    // Actualiza la cantidad de bloques libres en el superbloque.
    SB.cantBloquesLibres = SB.cantBloquesLibres - ((SB.posUltimoBloqueAI + 1) -
                                                   SB.posPrimerBloqueAI);
    // Escribe el superbloque en el dispositivo.
    if (bwrite(SBPOS, &SB) == -1)
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/* Función: escribir_bit:
* -----------------------
* Esta función permite modificar el valor de un bit del mapa de bits del disco.
*
*  nbloque: bloque que queremos indicar si está libre u ocupado.
*  bit: valor del bit que queremos escribir.
*
* return: EXIT_SUCCESS si se ha podido escribir el bit, si no EXIT_FAILURE.
*/
int escribir_bit(unsigned int nbloque, unsigned int bit)
{

    // Lee el superbloque del disco.
    struct superbloque SB;
    if (bread(SBPOS, &SB) == -1)
    {
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
        return EXIT_FAILURE;
    }

    // Leer del disco el bloque donde está el bit.
    if (bread(nbloqueabs, bufferMB) == -1)
    {
        free(bufferMB);
        return EXIT_FAILURE;
    }

    // Calcula la posición del byte dentro del bloque.
    posbyte = posbyte % BLOCKSIZE;

    //Genera la máscara para modificar el bit indicado por parámetro.
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
        return EXIT_FAILURE;
    }

    free(bufferMB);
    return EXIT_SUCCESS;
}

/* Function: leer_bit:
* --------------------
* Esta función permite leer el valor de un bit del mapa de bits del disco.
*
*  nbloque: bloque que queremos saber si está libre u ocupado.
*
* return: Valor del bit leído o bien -1 si se ha producido un error.
*/
unsigned char leer_bit(unsigned int nbloque)
{
    // Lee el superbloque del disco.
    struct superbloque SB;
    if (bread(SBPOS, &SB) == -1)
    {
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
        return EXIT_FAILURE;
    }

    // Lee del disco el bloque donde está el bit.
    if (bread(nbloqueabs, bufferMB) == -1)
    {
        free(bufferMB);
        return EXIT_FAILURE;
    }

    // Calcula la posición del byte dentro del bloque.
    posbyte = posbyte % BLOCKSIZE;

    // Genera la máscara para modificar el bit indicado por parámetro.
    unsigned char mascara = 128;
    mascara >>= posbit;

    // Obtiene el bit del bloque que se ha indicado por parámetro.
    mascara &= bufferMB[posbyte];
    mascara >>= (7 - posbit);

    free(bufferMB);
    return mascara;
}

/* Función: reservar_bloque:
* --------------------------
* Esta función reserva un bloque para ser usado si encuentra uno libre en el 
* mapa de bit.
*
* returns: Devuelve el número de bloque si se ha podido reservar, si no -1.
*/
int reservar_bloque()
{
    // Lee el superbloque del disco.
    struct superbloque SB;
    if (bread(SBPOS, &SB) == -1)
    {
        return -1;
    }

    // Comprueba que haya bloques libres en el disco.
    if (!SB.cantBloquesLibres)
    {
        return -1;
    }

    // Reserva espacio de memoria para el buffer del bloque.
    unsigned char *bufferMB = malloc(sizeof(char) * BLOCKSIZE);
    if (!bufferMB)
    {
        return -1;
    }

    // Reserva espacio de memoria para la máscara del bloque
    unsigned char *bufferAux = malloc(sizeof(char) * BLOCKSIZE);
    if (!bufferAux)
    {
        free(bufferMB);
        return -1;
    }

    // Se inicia la máscara con todos los bits a 1.
    memset(bufferAux, 255, BLOCKSIZE);

    // Declarar las variables y las inicializa.
    unsigned int posBloqueMB = SB.posPrimerBloqueMB;
    unsigned int posbyte = 0;
    unsigned int posbit = 0;
    unsigned char mascara = 128;
    unsigned int nbloque;

    // Lee el primer bloque del mapa de bits.
    if (bread((posBloqueMB), bufferMB) == -1)
    {
        free(bufferMB);
        free(bufferAux);
        return -1;
    }

    // Itera por todos los bloques de MB hasta encontrar uno con algún bit a 0.
    while (!memcmp(bufferMB, bufferAux, BLOCKSIZE) && posBloqueMB <
                                                          SB.posUltimoBloqueMB)
    {
        posBloqueMB++;
        if (bread((posBloqueMB), bufferMB) == -1)
        {
            free(bufferMB);
            free(bufferAux);
            return -1;
        }
    }

    free(bufferAux);

    // Localizar el byte en el que se encuentra el bloque vacío.
    while (posbyte <= BLOCKSIZE && bufferMB[posbyte] == 255)
    {
        posbyte++;
    }

    // Localizar el bit del primer bloque libre.
    while (bufferMB[posbyte] & mascara)
    {
        posbit++;
        bufferMB[posbyte] <<= 1;
    }

    // Determina la posición del bloque libre y liberamos el bufferMB.
    nbloque = ((posBloqueMB - SB.posPrimerBloqueMB) * BLOCKSIZE + posbyte) *
                  8 +
              posbit;
    free(bufferMB);

    // Reserva el bloque en el mapa de bits.
    if (escribir_bit(nbloque, 1))
    {
        return -1;
    }

    // Decrementa la cantidad de bloques libres.
    SB.cantBloquesLibres--;

    // Escribe el superbloque en el disco.
    if (bwrite(SBPOS, &SB) == -1)
    {
        return -1;
    }

    // Reserva espacio de memoria para la máscara del bloque
    bufferAux = malloc(sizeof(char) * BLOCKSIZE);
    if (!bufferAux)
    {
        return -1;
    }

    // Se inicia la máscara con todos los bits a 0.
    memset(bufferAux, 0, BLOCKSIZE);

    // Pone a 0s el bloque reservado en el disco.
    if (bwrite(nbloque, bufferAux) == -1)
    {
        free(bufferAux);
        return -1;
    }

    free(bufferAux);
    return nbloque;
}

/* Función: liberar_bloque:
* -------------------------
* Esta función libera el bloque indicado por parámetro.
*
*  nbloque: bloque a liberar.
*
* return: número de bloque liberado sino -1.
*/
int liberar_bloque(unsigned int nbloque)
{
    // Lee el superbloque del disco.
    struct superbloque SB;
    if (bread(SBPOS, &SB) == -1)
    {
        return -1;
    }

    // Revisa que el bloque este ocupado.
    if (!leer_bit(nbloque))
    {
        return -1;
    }

    // Pone a 0 el bit correspondiente al bloque en el mapa de bits.
    if (escribir_bit(nbloque, 0))
    {
        return -1;
    }

    // Incrementa la cantidad de bloques en el superbloque.
    SB.cantBloquesLibres++;

    // Escribe el superbloque en el disco.
    if (bwrite(SBPOS, &SB) == -1)
    {
        return -1;
    }

    return nbloque;
}

/* Función: escribir_inodo:
* -------------------------
* Esta función permite escribir un inodo en el disco en la posición indicada 
* por parámetro.
*
*  ninodo: posición del inodo en el array de inodos.
*  inodo: inodo que se quiere escribir en el disco.
*
* return: EXIT_SUCCESS si ha podido realizarlo, si no EXIT_FAILURE.
*/
int escribir_inodo(unsigned int ninodo, struct inodo inodo)
{
    // Lee el superbloque del disco.
    struct superbloque SB;
    if (bread(SBPOS, &SB) == -1)
    {
        return EXIT_FAILURE;
    }

    // Buffer de inodos.
    struct inodo inodos[BLOCKSIZE / INODOSIZE];

    // Lee el bloque que contiene el inodo.
    if (bread((SB.posPrimerBloqueAI + (ninodo / (BLOCKSIZE / INODOSIZE))),
              inodos) == -1)
    {
        return EXIT_FAILURE;
    }

    // Modifica el buffer con el inodo en el lugar correspondiente.
    inodos[(ninodo % (BLOCKSIZE / INODOSIZE))] = inodo;

    // Escribe el buffer en el disco.
    if (bwrite((SB.posPrimerBloqueAI + (ninodo / (BLOCKSIZE / INODOSIZE))),
               inodos) == -1)
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/* Función: leer_inodo:
* ---------------------
* Esta función permite leer un inodo en el disco en la posición indicada por 
* parámetro.
* 
*  ninodo: posición del inodo en el array de inodos.
*  inodo: puntero al buffer del inodo.
*
* return: EXIT_SUCCESS si ha podido realizarlo, si no EXIT_FAILURE.
*/
int leer_inodo(unsigned int ninodo, struct inodo *inodo)
{
    // Lee el superbloque del disco.
    struct superbloque SB;
    if (bread(SBPOS, &SB) == -1)
    {
        return EXIT_FAILURE;
    }

    // Buffer de inodos.
    struct inodo inodos[BLOCKSIZE / INODOSIZE];

    // Lee el bloque que contiene el inodo.
    if (bread((SB.posPrimerBloqueAI + (ninodo / (BLOCKSIZE / INODOSIZE))),
              inodos) == -1)
    {

        return EXIT_FAILURE;
    }

    // Modifica el buffer con el inodo en el lugar correspondiente.
    *inodo = inodos[(ninodo % (BLOCKSIZE / INODOSIZE))];
    return EXIT_SUCCESS;
}

/* Función: reservar_inodo:
* -------------------------
* Esta función permite reservar un inodo que está libre para su uso.
*
*  tipo: tipo de archivo a apuntar por este.
*  permisos: tipo de permisos que tendra este archivo.
*
* return: la posición del inodo o -1 si se ha producido algún error.
*/
int reservar_inodo(unsigned char tipo, unsigned char permisos)
{
    // Lee el superbloque del disco.
    struct superbloque SB;
    if (bread(SBPOS, &SB) == -1)
    {
        return -1;
    }

    // Comprobar si hay inodos libres.
    if (!SB.cantInodosLibres)
    {
        return -1;
    }

    // Obtener el primer inodo libre.
    unsigned int posInodoReservado = SB.posPrimerInodoLibre;

    // Buffer de un inodo.
    struct inodo inodo;

    // Actualiza la lista enlazada de inodos libres.
    if (leer_inodo(posInodoReservado, &inodo))
    {
        return -1;
    }
    SB.posPrimerInodoLibre = inodo.punterosDirectos[0];

    // Inicializa todos los campos del inodo.
    inodo.tipo = tipo;
    inodo.permisos = permisos;
    inodo.nlinks = 1;
    inodo.tamEnBytesLog = 0;
    inodo.atime = time(NULL);
    inodo.ctime = time(NULL);
    inodo.mtime = time(NULL);
    inodo.numBloquesOcupados = 0;
    for (int i = 0; i < (sizeof(inodo.punterosDirectos) / sizeof(unsigned int));
         i++)
    {
        inodo.punterosDirectos[i] = 0;
    }
    for (int i = 0; i < (sizeof(inodo.punterosIndirectos) /
                         sizeof(unsigned int));
         i++)
    {
        inodo.punterosIndirectos[i] = 0;
    }

    // Escribe el inodo en el array de inodos del disco.
    if (escribir_inodo(posInodoReservado, inodo))
    {
        return -1;
    }

    // Actualizar el superbloque y escribirlo en el disco.
    SB.cantInodosLibres--;
    if (bwrite(SBPOS, &SB) == -1)
    {
        return -1;
    }

    return posInodoReservado;
}

/* Función: obtener_nrangoBL:
* ---------------------------
* Esta función permite obtener el rango de punteros en el que se situa el 
* bloque lógico indicado por parámetro. Ademas obtiene la direccion almacenada 
* en el puntero correspondiente del inodo.
*
*  inodo: inodo que contiene los punteros del nivel actual.
*  nblogico: la posición lógica del bloque en el inodo.
*  ptr: puntero al siguiente nivel de la estructura del archivo.
*
* return: Devuelve el nivel en el que se encuentra el bloque o bien -1.
*/
int obtener_nrangoBL(struct inodo inodo, unsigned int nblogico, int *ptr)
{
    if (nblogico < DIRECTOS)
    {
        *ptr = inodo.punterosDirectos[nblogico];
        return 0;
    }
    if (nblogico < INDIRECTOS0)
    {
        *ptr = inodo.punterosIndirectos[0];
        return 1;
    }
    if (nblogico < INDIRECTOS1)
    {
        *ptr = inodo.punterosIndirectos[1];
        return 2;
    }
    if (nblogico < INDIRECTOS2)
    {
        *ptr = inodo.punterosIndirectos[2];
        return 3;
    }

    // Si se encuentra fuera de rango error.
    *ptr = 0;
    return -1;
}

/* Función: obtener_indice:
* -------------------------
* Esta función permite generalizar la obtención de los índices de los 
* bloques de punteros.
* 
*  nblogicos: la posición lógica del bloque en el inodo.
*  nivel_punteros: nivel en el inodo que se encuentra la función.
*
* return: devuelve el índice del bloque del nivel inferior o bien -1 si error.
*/
int obtener_indice(unsigned int nblogico, unsigned int nivel_punteros)
{
    // Si el bloque lógico se encuentra en los directos, devuelve su posición.
    if (nblogico < DIRECTOS)
    {
        return nblogico;
    }

    // Si se encuentra en los indirectos 0, se le restan los directos.
    if (nblogico < INDIRECTOS0)
    {
        return nblogico - DIRECTOS;
    }

    /* Si se encuentra en los indirectos 1, dependiendo del nivel de punteros,
       devolverá el bloque intermedio de punteros en el que se encuentra, o 
       bien la posición en el bloque final de punteros. */
    if (nblogico < INDIRECTOS1)
    {
        if (nivel_punteros == 2)
        {
            return (nblogico - INDIRECTOS0) / NPUNTEROS;
        }
        if (nivel_punteros == 1)
        {
            return (nblogico - INDIRECTOS0) % NPUNTEROS;
        }
    }

    /* Si se encuentra en los indirectos 2, dependiendo del nivel de punteros, 
       devolverá el bloque inicial de punteros, el bloque intermedio de 
       punteros, o bien la posición en el bloque final de punteros.*/
    if (nblogico < INDIRECTOS2)
    {
        if (nivel_punteros == 3)
        {
            return (nblogico - INDIRECTOS1) / (NPUNTEROS * NPUNTEROS);
        }
        if (nivel_punteros == 2)
        {
            return ((nblogico - INDIRECTOS1) % (NPUNTEROS * NPUNTEROS)) /
                   NPUNTEROS;
        }
        if (nivel_punteros == 1)
        {
            return ((nblogico - INDIRECTOS1) % (NPUNTEROS * NPUNTEROS)) %
                   NPUNTEROS;
        }
    }

    // Si se produce un error, devuelve -1.
    return -1;
}

/* Función: traducir_bloque_inodo:
* --------------------------------
* Esta función se encarga de obtener el número de bloque físico correspondiente
* a un bloque lógico determinado del inodo indicado.
*
*  ninodo: es el número de inodo en el array de inodos.
*  nblogico: es la posición logica del bloque en el inodo.
*  reservar: indica si hay que reservar y leer un bloque (1) o solo leerlo (0).
*
* return: Devuelve la posición fisica del bloque leido o bien leido y resevado.
*/
int traducir_bloque_inodo(unsigned int ninodo, unsigned int nblogico,
                          char reservar)
{

    // Lee el inodo.
    struct inodo inodo;
    if (leer_inodo(ninodo, &inodo))
    {
        return -1;
    }

    // Declaración variables para la búsqueda de la dirección física.
    int ptr = 0;
    int ptr_ant = 0;
    int salvar_inodo = 0;
    int indice;
    int buffer[NPUNTEROS];

    // Obtiene en qué nivel del inodo está el bloque lógico.
    int nRangoBL = obtener_nrangoBL(inodo, nblogico, &ptr);
    if (nRangoBL == -1)
    {
        return -1;
    }
    int nivel_punteros = nRangoBL;

    // Itera por los niveles de punteros del inodo.
    while (nivel_punteros)
    {

        // Si ptr es diferente de 0, salta.
        if (!ptr)
        {
            // Si reservar es 1, devuelve -1.
            if (!reservar)
            {
                return -1;
            }

            // Reserva el primer bloque y guarda su dirección en ptr.
            salvar_inodo = 1;
            ptr = reservar_bloque();
            if (ptr == -1)
            {
                return -1;
            }

            // Actualiza la información del inodo.
            inodo.numBloquesOcupados++;
            inodo.ctime = time(NULL);

            /* Si estamos en el nivel_punteros más alto, guardamos la dirección
               física del bloque de punteros en el inodo. */
            if (nivel_punteros == nRangoBL)
            {
                inodo.punterosIndirectos[nRangoBL - 1] = ptr;
                printf("[Traducir_bloque_inodo()-> inodo.punterosIndirectos[%d"
                       "] = %d (reservado BF %d para punteros_nivel%d)]\n",
                       nRangoBL - 1, ptr, ptr, nivel_punteros);
            }
            else
            {
                /* Si no, guardamos la dirección física del bloque de punteros 
                   en el nivel superior */
                buffer[indice] = ptr;
                printf("[Traducir_bloque_inodo()-> punteros_nivel%d [%d] = %d"
                       "(reservado BF %d para punteros_nivel%d)]\n",
                       nivel_punteros + 1, indice, ptr, ptr, nivel_punteros);

                // Actualiza el bloque físico de punteros.
                if (bwrite(ptr_ant, buffer) == -1)
                {
                    return -1;
                }
            }
        }

        // Lee el último bloque de punteros.
        if (bread(ptr, buffer) == -1)
        {
            return -1;
        }

        // Obtiene el índice de bloque lógico dentro del nivel.
        indice = obtener_indice(nblogico, nivel_punteros);
        if (indice == -1)
        {
            return -1;
        }

        // Avanza dentro de los niveles de punteros.
        ptr_ant = ptr;
        ptr = buffer[indice];
        nivel_punteros--;
    }

    // Si no existe un bloque de datos.
    if (!ptr)
    {

        // Si está traduciendo el bloque para leer, devuelve error.
        if (!reservar)
        {
            return -1;
        }
        else
        {
            salvar_inodo = 1;

            // Se reserva un bloque físico.
            ptr = reservar_bloque();
            if (ptr == -1)
            {
                return -1;
            }

            // Actualiza el inodo.
            inodo.numBloquesOcupados++;
            inodo.ctime = time(NULL);

            /* Si el rango del bloque lógico está en los directos, lo guarda 
               directamente en el inodo. */
            if (!nRangoBL)
            {
                inodo.punterosDirectos[nblogico] = ptr;
                printf("[Traducir_bloque_inodo()-> inodo.punterosDirectos[%d] "
                       "= %d (reservado BF %d para BL %d)]\n",
                       nblogico, ptr, ptr, nblogico);
            }
            else
            {

                /* Si se encuentra en alguno de los indirectos, lo guarda en el 
                   buffer de bloque de punteros adecuado. */
                buffer[indice] = ptr;
                printf("[Traducir_bloque_inodo()-> punteros_nivel%d [%d] = %d "
                       "(reservado BF %d para BL %d)]\n",
                       nivel_punteros + 1, indice, ptr, ptr, nblogico);
                if (bwrite(ptr_ant, buffer) == -1)
                {
                    return -1;
                }
            }
        }
    }

    // Si se ha de guardar el inodo, realiza la escritura en el dispositivo.
    if (salvar_inodo == 1)
    {
        if (escribir_inodo(ninodo, inodo))
        {
            return -1;
        }
    }
    return ptr;
}

/* Función: liberar_inodo:
* --------------------------------
* Esta función se encarga de liberar el inodo y todos los bloques de datos 
* unidos a él.
*
*  ninodo: es el número de inodo en el array de inodos.
*
* return: Devuelve el número de inodo que se ha liberado.
*/
int liberar_inodo(unsigned int ninodo)
{
    // Lee el inodo.
    struct inodo inodo;
    if (leer_inodo(ninodo, &inodo))
    {

        return -1;
    }

    // Ejecuta el algoritmo de liberación de los bloques a partir del offset 0.
    int bloquesLiberados = liberar_bloques_inodo(0, &inodo);

    /* Actualiza los bloques ocupados del inodo y comprueba que se hayan 
       borrado todos. */
    inodo.numBloquesOcupados = bloquesLiberados - inodo.numBloquesOcupados;
    if ((inodo.numBloquesOcupados))
    {

        return -1;
    }

    // Actualiza el inodo para enlazarlo con los inodos libres.
    inodo.tipo = 'l';
    inodo.tamEnBytesLog = 0;

    // Lee el superbloque.
    struct superbloque SB;
    if (bread(SBPOS, &SB) == -1)
    {

        return -1;
    }

    // Enlaza el inodo con la lista de inodos libres.
    inodo.punterosDirectos[0] = SB.posPrimerInodoLibre;
    SB.posPrimerInodoLibre = ninodo;
    SB.cantInodosLibres++;

    // Escribe el inodo en el dispositivo.
    if (escribir_inodo(ninodo, inodo))
    {

        return -1;
    }

    // Escribe el superbloque en el dispositivo.
    if (bwrite(SBPOS, &SB) == -1)
    {

        return -1;
    }

    return ninodo;
}

/* Función: liberar_bloques_inodo:
* --------------------------------
* Esta función se encarga de liberar los bloques de datos unidos al inodo.
*
*  primerBL: primer bloque lógico donde se empieza a borrar.
*  inodo: el inodo donde se quiere borrar.
*
* return: devuelve el número de bloques que se han liberado.
*/
int liberar_bloques_inodo(unsigned int primerBL, struct inodo *inodo)
{

    unsigned int nRangoBL, nivel_punteros, indice, ultimoBL;
    int ptr = 0;
    int nBL;
    int bloques_punteros[3][NPUNTEROS];

    /* Guardamos historial de índices y punteros que fuimos recorriendo para 
    poder volver atrás (después de llegar al final) y marcarlos como libres. */
    int ptr_nivel[3];
    int indices[3];
    int liberados = 0;

    // Buffer bloque de ceros.
    unsigned char buffer_null[BLOCKSIZE];
    memset(buffer_null, 0, BLOCKSIZE);

    // Obtiene el último bloque lógico.
    if (!(inodo->tamEnBytesLog))
    {
        return liberados;
    }
    if (!(inodo->tamEnBytesLog % BLOCKSIZE))
    {
        ultimoBL = inodo->tamEnBytesLog / BLOCKSIZE - 1;
    }
    else
    {
        ultimoBL = inodo->tamEnBytesLog / BLOCKSIZE;
    }

    printf("[liberar_bloques_inodo()-> primerBL: %d, ultimoBL: %d]\n", primerBL,
           ultimoBL);

    // Itera por todos los bloques lógicos del inodo.
    for (nBL = primerBL; nBL <= ultimoBL; nBL++)
    {

        // Obtiene el nivel en que se encuentra el bloque lógico.
        nRangoBL = obtener_nrangoBL(*inodo, nBL, &ptr);
        if (nRangoBL < 0)
        {
            return -1;
        }
        nivel_punteros = nRangoBL;

        // Itera por los bloques de punteros para obtenerlos.
        while ((ptr > 0) && (nivel_punteros > 0))
        {

            // Obtiene el índice del bloque lógico en el bloque de punteros.
            indice = obtener_indice(nBL, nivel_punteros);

            /* El bloque de punteros no se leerá de disco a no ser que índice 
               = 0 o bien que nBL sea el primer bloque. */
            if ((!indice) || (nBL == primerBL))
            {
                if (bread(ptr, bloques_punteros[nivel_punteros - 1]) == -1)
                {
                    return -1;
                }
            }

            /* Guarda y actualiza la dirección física del puntero y el índice 
               del bloque lógico. */
            ptr_nivel[nivel_punteros - 1] = ptr;
            indices[nivel_punteros - 1] = indice;
            ptr = bloques_punteros[nivel_punteros - 1][indice];
            nivel_punteros--;
        }

        // Si existe el bloque de datos, procedemos a borrarlo.
        if (ptr > 0)
        {

            // Se libera el bloque físico y actualiza la variable liberados.
            liberar_bloque(ptr);
            liberados++;
            printf("[liberar_bloques_inodo()-> liberado BF %d de datos par"
                   "a BL %d]\n",
                   ptr, nBL);

            /* Si el bloque pertenece a los punteros directos, se elimina 
               directamente del inodo. */
            if (!nRangoBL)
            {
                inodo->punterosDirectos[nBL] = 0;
            }
            else
            {

                /* Si el bloque pertenece a un puntero indirecto, se itera todo 
                   el árbol .*/
                while (nivel_punteros < nRangoBL)
                {

                    /* Actualiza las variables utilizadas para eliminación de 
                       bloques. */
                    indice = indices[nivel_punteros];
                    bloques_punteros[nivel_punteros][indice] = 0;
                    ptr = ptr_nivel[nivel_punteros];

                    // Comprueba si el bloque está vacío.
                    if (!memcmp(bloques_punteros[nivel_punteros], buffer_null,
                                BLOCKSIZE))
                    {

                        // Libera el bloque de punteros.
                        liberar_bloque(ptr);
                        
                        /* Dependiendo del nivel de punteros donde se encuentre 
                           el algoritmo, saltará una cantidad determinada de 
                           bloques lógicos . */
                        switch (nivel_punteros)
                        {
                        case 0:
                            nBL += NPUNTEROS - indices[nivel_punteros] - 1;
                            break;
                        case 1:
                            nBL += NPUNTEROS * (NPUNTEROS -
                                                indices[nivel_punteros]) -
                                   1;
                            break;
                        case 2:
                            nBL += (NPUNTEROS * NPUNTEROS) *
                                       (NPUNTEROS - indices[nivel_punteros]) -
                                   1;
                            break;
                        default:
                            break;
                        }   

                        /* Actualiza el nivel de punteros y los bloques 
                           liberados. */
                        liberados++;
                        nivel_punteros++;

                        printf("[liberar_bloques_inodo()-> liberado BF %d de p"
                               "unteros_nivel%d correspondiente al BL %d]\n",
                               ptr, nivel_punteros, nBL);

                        /* Si el nivel de punteros coincide con el rango, 
                           modifica directamente el inodo. */
                        if (nivel_punteros == nRangoBL)
                        {
                            inodo->punterosIndirectos[nRangoBL - 1] = 0;
                        }
                    }
                    else
                    {

                        /* Si no está vacío el bloque de punteros, se actualiza 
                           en el disco. */
                        if (bwrite(ptr, bloques_punteros[nivel_punteros]) == -1)
                        {
                            return -1;
                        }
                        nivel_punteros = nRangoBL;
                    }
                }
            }
        }
    }
    printf("[liberar_bloques_inodo()-> total bloques liberados: %d]\n",
           liberados);
    return liberados;
}
