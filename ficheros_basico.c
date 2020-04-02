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

    // Actualiza la cantidad de bloques libres en el superbloque.
    SB.cantBloquesLibres = SB.cantBloquesLibres - ((SB.posUltimoBloqueMB + 1) - SB.posPrimerBloqueMB);
    if (bwrite(SBPOS, &SB) == -1)
    {
        perror("Error");
        return EXIT_FAILURE;
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

    // Actualiza la cantidad de bloques libres en el superbloque.
    SB.cantBloquesLibres = SB.cantBloquesLibres - ((SB.posUltimoBloqueAI + 1) - SB.posPrimerBloqueAI);
    if (bwrite(SBPOS, &SB) == -1)
    {
        perror("Error");
        return EXIT_FAILURE;
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
        return -1;
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
        return -1;
    }

    // Reserva espacio de memoria para la mascara del bloque
    unsigned char *bufferAux = malloc(sizeof(char) * BLOCKSIZE);
    if (!bufferAux)
    {
        perror("Error");
        free(bufferMB);
        return -1;
    }

    // Se inicia la mascara con todos los bits a 1.
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
        perror("Error");
        return -1;
    }

    // Itera por todos los bloques de MB hasta encontrar uno con algun bit a 0.
    while (!memcmp(bufferMB, bufferAux, BLOCKSIZE) && posBloqueMB < SB.posUltimoBloqueMB)
    {
        posBloqueMB++;
        if (bread((posBloqueMB), bufferMB) == -1)
        {
            free(bufferMB);
            free(bufferAux);
            perror("Error");
            return -1;
        }
    }

    free(bufferAux);

    // Localizar el byte en el que se encuentra el bloque vacio.
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

    // Determina la posicion del bloque libre y liberamos el bufferMB.
    nbloque = ((posBloqueMB - SB.posPrimerBloqueMB) * BLOCKSIZE + posbyte) * 8 + posbit;
    free(bufferMB);

    // Reserva el bloque en el mapa de bits.
    if (escribir_bit(nbloque, 1))
    {
        perror("Error");
        return -1;
    }

    // Decrementa la cantidad de bloques libres.
    SB.cantBloquesLibres--;

    // Escribe el superbloque en el disco.
    if (bwrite(SBPOS, &SB) == -1)
    {
        perror("Error");
        return -1;
    }

    // Reserva espacio de memoria para la mascara del bloque
    bufferAux = malloc(sizeof(char) * BLOCKSIZE);
    if (!bufferAux)
    {
        perror("Error");
        return -1;
    }

    // Se inicia la mascara con todos los bits a 0.
    memset(bufferAux, 0, BLOCKSIZE);

    // Pone a 0s el bloque reservado en el disco.
    if (bwrite(nbloque, bufferAux) == -1)
    {
        perror("Error");
        free(bufferAux);
        return -1;
    }

    free(bufferAux);
    return nbloque;
}

/* Funcion: liberar_bloque:
* -------------------------
* Esta funcion libera el bloque indicado por parametro.
*
*  nbloque: bloque a liberar.
*
* return: numero de bloque liberado sino -1.
*/
int liberar_bloque(unsigned int nbloque)
{
    // Lee el superbloque del disco.
    struct superbloque SB;
    if (bread(SBPOS, &SB) == -1)
    {
        perror("Error");
        return -1;
    }

    // Revisa que el bloque este ocupado.
    if (!leer_bit(nbloque))
    {
        // Escribir error.
        return -1;
    }

    // Pone a 0 el bit correspondiente al bloque en el mapa de bits.
    if (escribir_bit(nbloque, 0))
    {
        perror("Error");
        return -1;
    }

    // Incrementa la cantidad de bloques en el superbloque.
    SB.cantBloquesLibres++;

    // Escribe el superbloque en el disco.
    if (bwrite(SBPOS, &SB) == -1)
    {
        perror("Error");
        return -1;
    }

    return nbloque;
}

/* Funcion: escribir_inodo:
* -------------------------
* Esta funcion permite escribir un inodo en el disco en la posicion indicada 
* por parametro.
*
*  ninodo: posicion del inodo en el array de inodos.
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
        perror("Error");
        return EXIT_FAILURE;
    }

    // Buffer de inodos.
    struct inodo inodos[BLOCKSIZE / INODOSIZE];

    // Lee el bloque que contiene el inodo.
    if (bread((SB.posPrimerBloqueAI + (ninodo / (BLOCKSIZE / INODOSIZE))), inodos) == -1)
    {
        perror("Error");
        return EXIT_FAILURE;
    }

    // Modifica el buffer con el inodo en el lugar correspondiente.
    inodos[(ninodo % (BLOCKSIZE / INODOSIZE))] = inodo;

    // Escribe el buffer en el disco.
    if (bwrite((SB.posPrimerBloqueAI + (ninodo / (BLOCKSIZE / INODOSIZE))), inodos) == -1)
    {
        perror("Error");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

/* Funcion: leer_inodo:
* ---------------------
* Esta funcion permite leer un inodo en el disco en la posicion indicada por 
* parametro.
* 
*  ninodo: posicion del inodo en el array de inodos.
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
        perror("Error");
        return EXIT_FAILURE;
    }

    // Buffer de inodos.
    struct inodo inodos[BLOCKSIZE / INODOSIZE];

    // Lee el bloque que contiene el inodo.
    if (bread((SB.posPrimerBloqueAI + (ninodo / (BLOCKSIZE / INODOSIZE))), inodos) == -1)
    {
        perror("Error");
        return EXIT_FAILURE;
    }

    // Modifica el buffer con el inodo en el lugar correspondiente.
    *inodo = inodos[(ninodo % (BLOCKSIZE / INODOSIZE))];
    return EXIT_SUCCESS;
}

/* Funcion: reservar_inodo:
* -------------------------
* Esta funcion permite reservar un inodo que este libre para su uso.
*
*  tipo: tipo de archivo a apuntar por este.
*  permisos: tipo de permisos que tendra este archivo.
*
* return: la posicion del inodo o -1 si se ha producido algun error.
*/
int reservar_inodo(unsigned char tipo, unsigned char permisos)
{
    // Lee el superbloque del disco.
    struct superbloque SB;
    if (bread(SBPOS, &SB) == -1)
    {
        perror("Error");
        return -1;
    }

    // Comprobar si hay inodos libres.
    if (!SB.cantInodosLibres)
    {
        perror("Error");
        return -1;
    }

    // Obtener el primer inodo libre.
    unsigned int posInodoReservado = SB.posPrimerInodoLibre;

    // Buffer de un inodo.
    struct inodo inodo;

    // Actualiza la lista enlazada de inodos libres.
    if (leer_inodo(posInodoReservado, &inodo))
    {
        perror("Error");
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
    for (int i = 0; i < (sizeof(inodo.punterosDirectos) / sizeof(unsigned int)); i++)
    {
        inodo.punterosDirectos[i] = 0;
    }
    for (int i = 0; i < (sizeof(inodo.punterosIndirectos) / sizeof(unsigned int)); i++)
    {
        inodo.punterosIndirectos[i] = 0;
    }

    // Escribe el inodo en el array de inodos del disco.
    if (escribir_inodo(posInodoReservado, inodo))
    {
        perror("Error");
        return -1;
    }

    // Actualizar el superbloque y escribirlo en el disco.
    SB.cantInodosLibres--;
    if (bwrite(SBPOS, &SB) == -1)
    {
        perror("Error");
        return -1;
    }

    return posInodoReservado;
}

/* Funcion: obtener_nrangoBL:
* ---------------------------
* Esta funcion permite obtener el rango de punteros en el que se situa el 
* bloque logico indicado por parametro. Ademas obtiene la direccion almacenada 
* en el puntero correspondiente del inodo.
*
*  inodo: inodo que contiene los punteros del nivel actual.
*  nblogico: la posicion logica del bloque en el inodo.
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
    perror("Bloque lógico fuera de rango");
    return -1;
}

/* Funcion: obtener_indice:
* -------------------------
* Esta funcion permite generalizar la obtencion de los indices de los 
* bloques de punteros.
* 
*  nblogicos: la posicion logica del bloque en el inodo.
*  nivel_punteros: nivel en el inodo que se encuentra la funcion.
*
* return: devuelve el indice del bloque del nivel inferior o bien -1 si error.
*/
int obtener_indice(unsigned int nblogico, unsigned int nivel_punteros)
{
    if (nblogico < DIRECTOS)
    {
        return nblogico;
    }
    if (nblogico < INDIRECTOS0)
    {
        return nblogico - DIRECTOS;
    }
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
    if (nblogico < INDIRECTOS2)
    {
        if (nivel_punteros == 3)
        {
            return (nblogico - INDIRECTOS1) / (NPUNTEROS * NPUNTEROS);
        }
        if (nivel_punteros == 2)
        {
            return ((nblogico - INDIRECTOS1) % (NPUNTEROS * NPUNTEROS)) / NPUNTEROS;
        }
        if (nivel_punteros == 1)
        {
            return ((nblogico - INDIRECTOS1) % (NPUNTEROS * NPUNTEROS)) % NPUNTEROS;
        }
    }
    return -1;
}

/* Funcion: traducir_bloque_inodo:
* --------------------------------
* Esta funcion se encarga de obtener el numero de bloque fisico correspondiente
* a un bloque logico determinado del inodo indicado.
*
*  ninodo: es el numero de inodo en el array de inodos.
*  nblogico: es la posicion logioca del bloque en el inodo.
*  reservar: indica si hay que reservar y leer un bloque (1) o solo leerlo (0).
*
* return: Devuelve la posicion fisica del bloque leido o bien leido y resevado.
*/
int traducir_bloque_inodo(unsigned int ninodo, unsigned int nblogico, char reservar)
{
    // Declaracion variables.
    struct inodo inodo;
    int ptr = 0, ptr_ant = 0, salvar_inodo = 0, nRangoBL, nivel_punteros, indice;
    int buffer[NPUNTEROS];
    if (leer_inodo(ninodo, &inodo))
    {
        perror("Error");
        return -1;
    }
    nRangoBL = obtener_nrangoBL(inodo, nblogico, &ptr);
    if (nRangoBL == -1)
    {
        perror("Error");
        return -1;
    }
    nivel_punteros = nRangoBL;
    while (nivel_punteros)
    {
        if (!ptr)
        {
            if (!reservar)
            {
                return -1;
            }
            salvar_inodo = 1;
            ptr = reservar_bloque();
            if (ptr == -1)
            {
                perror("Error");
                return -1;
            }
            inodo.numBloquesOcupados++;
            inodo.ctime = time(NULL);
            if (nivel_punteros == nRangoBL)
            {
                inodo.punterosIndirectos[nRangoBL - 1] = ptr;
                // IMPRIMIMOS PARA TEST
                printf("%d\n", ptr);
            }
            else
            {
                buffer[indice] = ptr;
                // IMPRIMIMOS PARA TEST
                printf("%d\n", ptr);
                if (bwrite(ptr_ant, buffer) == -1)
                {
                    perror("Error");
                    return -1;
                }
            }
        }
        if (bread(ptr, buffer) == -1)
        {
            perror("Error");
            return -1;
        }
        indice = obtener_indice(nblogico, nivel_punteros);
        if (indice == -1)
        {
            perror("Error");
            return -1;
        }
        ptr_ant = ptr;
        ptr = buffer[indice];
        nivel_punteros--;
    }
    if (!ptr)
    {
        if (!reservar)
        {
            return -1;
        }
        else
        {
            salvar_inodo = 1;
            ptr = reservar_bloque();
            if (ptr == -1)
            {
                perror("Error");
                return -1;
            }
            inodo.numBloquesOcupados++;
            inodo.ctime = time(NULL);
            if (!nRangoBL)
            {
                inodo.punterosDirectos[nblogico] = ptr;
                // IMPRIMIMOS PARA TEST
                printf("%d\n", ptr);
            }
            else
            {
                buffer[indice] = ptr;
                // IMPRIMIMOS PARA TEST
                printf("%d\n", ptr);
                if (bwrite(ptr_ant, buffer) == -1)
                {
                    perror("Error");
                    return -1;
                }
            }
        }
    }
    if (salvar_inodo == 1)
    {
        if (escribir_inodo(ninodo, inodo))
        {
            perror("Error");
            return -1;
        }
    }
    return ptr;
}

int liberar_inodo(unsigned int ninodo)
{
    struct inodo inodo;

    if (leer_inodo(ninodo, &inodo))
    {
        perror("Error");
        return -1;
    }

    int bloquesLiberados = liberar_bloques_inodo(0, &inodo);
    printf("Se han borrado los bloques\n");
    inodo.numBloquesOcupados = bloquesLiberados - inodo.numBloquesOcupados;
    if ((inodo.numBloquesOcupados))
    {
        perror("Error");
        return -1;
    }

    inodo.tipo = 'l';
    inodo.tamEnBytesLog = 0;

    struct superbloque SB;
    if (bread(SBPOS, &SB) == -1)
    {
        perror("Error");
        return -1;
    }

    inodo.punterosDirectos[0] = SB.posPrimerInodoLibre;
    SB.posPrimerInodoLibre = ninodo;
    SB.cantInodosLibres++;

    if (escribir_inodo(ninodo, inodo))
    {
        perror("Error");
        return -1;
    }

    if (bwrite(SBPOS, &SB) == -1)
    {
        perror("Error");
        return -1;
    }

    return ninodo;
}

int liberar_bloques_inodo(unsigned int primerBL, struct inodo *inodo)
{

    unsigned int nRangoBL, nivel_punteros, indice, ultimoBL;
    int ptr = 0;
    int nBL;
    int bloques_punteros[3][NPUNTEROS];
    // guardamos historial de indices y punteros que fuimos recorriendo para poder
    // volver atras (después de llegar al final) y marcarlos como libres.
    int ptr_nivel[3];
    int indices[3];
    int liberados = 0;

    // buffer bloque de ceros.
    unsigned char buffer_null[BLOCKSIZE];
    memset(buffer_null, 0, BLOCKSIZE);

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

    for (nBL = 0; nBL <= ultimoBL; nBL++)
    {

        nRangoBL = obtener_nrangoBL(*inodo, nBL, &ptr);
        if (nRangoBL < 0)
        {
            perror("Error");
            return -1;
        }
        nivel_punteros = nRangoBL;

        while ((ptr > 0) && (nivel_punteros > 0))
        {

            indice = obtener_indice(nBL, nivel_punteros);
            if ((!indice) || (nBL == primerBL))
            {
                if (bread(ptr, bloques_punteros[nivel_punteros - 1]) == -1)
                {
                    perror("Error");
                    return -1;
                }
            }
            ptr_nivel[nivel_punteros - 1] = ptr;
            indices[nivel_punteros - 1] = indice;
            ptr = bloques_punteros[nivel_punteros - 1][indice];
            nivel_punteros--;
        }

        if (ptr > 0)
        {
            liberar_bloque(ptr);
            liberados++;
            if (!nRangoBL)
            {
                inodo->punterosDirectos[nBL] = 0;
            }
            else
            {
                while (nivel_punteros < nRangoBL)
                {

                    indice = indices[nivel_punteros];
                    bloques_punteros[nivel_punteros][indice] = 0;
                    ptr = ptr_nivel[nivel_punteros];
                    if (!memcmp(bloques_punteros[nivel_punteros], buffer_null, BLOCKSIZE))
                    {
                        liberar_bloque(ptr);
                        printf("nBL: %d\n", nBL);
                        //
                        switch (nivel_punteros){
                            case 0: nblog += NPUNTEROS - indices[nivel_punteros]-1; break;
                            case 1: nblog += NPUNTEROS * (NPUNTEROS - indices[nivel_punteros])-1; break;
                            case 2: nblog += (NPUNTEROS * NPUNTEROS) * (NPUNTEROS - indices[nivel_punteros])-1; break;
                            default: break;
                        }
                        // printf("nBL: %d\n", nBL);
                        liberados++;
                        nivel_punteros++;
                        if (nivel_punteros == nRangoBL)
                        {
                            inodo->punterosIndirectos[nRangoBL - 1] = 0;
                        }
                    }
                    else
                    {
                        if (bwrite(ptr, bloques_punteros[nivel_punteros]) == -1)
                        {
                            perror("Error");
                            return -1;
                        }
                        nivel_punteros = nRangoBL;
                    }
                }
            }
        }
    }
    return liberados;
}
