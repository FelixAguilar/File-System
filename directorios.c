// Autores: Félix Aguilar, Adrián Bennasar, Álvaro Bueno
#include "directorios.h"

// Variable global para el último fichero escrito.
struct UltimaEntrada UltimaEntradaEscritura;
struct UltimaEntrada UltimaEntradaLectura;

/* Función: extraer_camino:
* -------------------------
* Esta función descompone el camino indicado por parametro en dos, inicial y 
* final, los cuales son el siguiente paso y los proximos pasos en el camino 
* respectivamente.
* 
*  camino: dirección del fichero del dispositivo virtual.
*  inicial: directorio o fichero mas proximo al directorio actual.
*  final: camino restante despues de inicial.
*  tipo: indica si inicial es un fichero o un directorio.
*
* returns: Exit_Success o bien Exit_Failure si se produce un error
*/
int extraer_camino(const char *camino, char *inicial, char *final, char *tipo)
{
    // Si el camino no comienza con '/' entonces error.
    if (*(camino) != '/')
    {
        return EXIT_FAILURE;
    }

    // Localiza la primera barra despues de la inicial.
    char *f = strchr((camino + 1), '/');
    *(tipo) = 'f';

    // Dependiendo de si existe el caracter '/' se realiza lo siguiente.
    if (f)
    {
        // Si existe, recoje el elemento inicial y el resto lo guarda en final.
        strncpy(inicial, (camino + 1), (strlen(camino) - strlen(f) - 1));
        strcpy(final, f);

        // Si el primer caracter de final es '/', inicial es un directorio.
        if (final[0] == '/')
        {
            *(tipo) = 'd';
        }
    }
    else
    {
        // Si no existe, copia camino en inicial y final lo indica como vacio.
        strcpy(inicial, (camino + 1));
        final[0] = '\0';
    }

    return EXIT_SUCCESS;
}

/* Función: buscar_entrada:
* -------------------------
* Esta funcion buscara una determinada entrada en el sistema de archivos, la 
* cual dependiendo del valor de reservar, reservara o no un inodo para el 
* elemento. Ademas de esto devuelve el identificador de su inodo.
*
*  camino_parcial: camino a recorrer para llegar al destino.
*  p_inodo_dir: identificador del inodo del directorio padre.
*  p_inodo: identificador del inodo del destino a obtener.
*  p_entrada: numero de entrada dentro del directorio padre.
*  reservar: indicador si se ha de crear un nuevo elemento o no (1 o 0).
*  permisos: permisos a tener por el nuevo elemento (si reservar = 1).
*
* return: Devuelve un codigo de error para ser procesado por la funcion 
*         mostrar_error_directorios si es menor que 0. si ha sido correcta la
*         ejecucion devolvera 0 o 1.
*/
int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir,
                   unsigned int *p_inodo, unsigned int *p_entrada,
                   char reservar, unsigned char permisos)
{
    // Si el camino percial es la raiz.
    if (!strcmp(camino_parcial, "/"))
    {
        // Lee el superbloque.
        struct superbloque SB;
        if (bread(SBPOS, &SB) == -1)
        {
            return ERROR_ACCESO_DISCO;
        }
        // Guarda el pinodo del directorio raiz y finaliza la ejecucion.
        *(p_inodo) = SB.posInodoRaiz;
        *(p_entrada) = 0;
        return EXIT_SUCCESS;
    }

    // Lee el inodo del directorio padre.
    struct inodo inodo;
    if (leer_inodo(*(p_inodo_dir), &inodo))
    {
        return ERROR_ACCESO_DISCO;
    }

    // Comprueba que este tenga permiso de lectura.
    if ((inodo.permisos & 4) != 4)
    {
        return ERROR_PERMISO_LECTURA;
    }

    // Instancia las variables a utilizar.
    struct entrada entrada;
    char inicial[sizeof(entrada.nombre)];
    char final[strlen(camino_parcial)];
    char tipo;

    // Inicializa los buffers como arrays vacios.
    memset(inicial, 0, sizeof(entrada.nombre));
    memset(final, 0, strlen(camino_parcial));
    memset(entrada.nombre, 0, sizeof(entrada.nombre));

    // Obtiene el elemento inicial y su tipo ademas del final del camino.
    if (extraer_camino(camino_parcial, inicial, final, &tipo))
    {
        return ERROR_CAMINO_INCORRECTO;
    }

#if DEBUG

    printf("[buscar_entrada()->inicial: %s, final: %s, reservar: %d]\n", inicial,
           final, reservar);

#endif

    // Obtiene el numero total de entrada en el inodo y inicializa el indice.
    int cantEntradasInodo = inodo.tamEnBytesLog / sizeof(struct entrada);
    int numEntradaInodo = 0;

    //Si hay entradas en el inodo.
    if (cantEntradasInodo > 0)
    {
        // Inicializa un array de entradas que caben en un bloque.
        struct entrada entradas[BLOCKSIZE / sizeof(struct entrada)];

        /* Se itera mientras haya entradas sin procesar o bien no se haya 
           encontrado el elemento incial. */
        while ((numEntradaInodo < cantEntradasInodo) && strcmp(entrada.nombre,
                                                               inicial))
        {
            // Obtiene la direccion fisica del bloque de entradas a procesar.
            int bloquef = traducir_bloque_inodo(
                *(p_inodo_dir),
                numEntradaInodo / (BLOCKSIZE / sizeof(struct entrada)), 0);
            if (bloquef == -1)
            {
                return ERROR_ACCESO_DISCO;
            }
            // Lee el contenido del bloque de entradas.
            memset(entradas, 0, BLOCKSIZE);
            int bytes;
            if ((bytes = bread(bloquef, entradas)) == -1)
            {
                return ERROR_ACCESO_DISCO;
            }
            /* Itera por el contenido del bloque de entradas mientras no se 
               hayan procesado todas las entradas de este o bien encontrado el 
               elemento inicial.*/
            int numEntradaArray = 0;
            //printf("bytes: %d\n", bytes);
            while (numEntradaArray < ((bytes / sizeof(struct entrada)) - 1) &&
                   strcmp(entradas[numEntradaArray].nombre, inicial) &&
                   (numEntradaInodo < cantEntradasInodo))
            {
                // Avanza dentro de las entradas del inodo.
                numEntradaArray++;
                numEntradaInodo++;
                //printf("%d y %s\n", numEntradaArray , entradas[numEntradaArray].nombre);
            }

            //printf ("strcmp (%s, %s)\n", entradas[numEntradaArray].nombre, inicial);

            //Si el elemento se ha encontrado, copia sus datos en entrada.
            if (cantEntradasInodo != numEntradaInodo && !strcmp(entradas[numEntradaArray].nombre, inicial))
            {
                strcpy(entrada.nombre, entradas[numEntradaArray].nombre);
                entrada.ninodo = entradas[numEntradaArray].ninodo;
            }
        }
    }

    //printf("%d == %d && strcmp(%s, %s)\n", cantEntradasInodo, numEntradaInodo , entrada.nombre, inicial);

    // Si inicial no se ha encontrado y se han procesado todas las entradas.
    if (cantEntradasInodo == numEntradaInodo && strcmp(entrada.nombre, inicial))
    {
        /* Dependiendo de estado introducido en reservar, se realiza diferentes
           acciones. */
        switch (reservar)
        {
        case 0:
            return ERROR_NO_EXISTE_ENTRADA_CONSULTA;
            break;

        case 1:

            /* Comprueba que el inodo sea diferente de fichero y tenga permisos
               de escritura. */
            if (inodo.tipo == 'f')
            {
                return ERROR_NO_ES_UN_DIRECTORIO;
            }
            if ((inodo.permisos & 2) != 2)
            {
                return ERROR_PERMISO_ESCRITURA;
            }
            else
            {
                /* copia inicial en entrada y revisa que el tipo de inicial sea
                   directorio. */
                strcpy(entrada.nombre, inicial);
                if (tipo == 'd')
                {
                    // Revisa que el final sea '/'.
                    if (!strcmp(final, "/"))
                    {
                        // Reserva un inodo para el nuevo directorio.
                        entrada.ninodo = reservar_inodo(tipo, permisos);

#if DEBUG

                        printf("[buscar()->reservado inodo %d tipo %c con perm"
                               "isos %d para %s]\n",
                               entrada.ninodo, tipo, permisos, entrada.nombre);

#endif

                        if (entrada.ninodo == -1)
                        {
                            return ERROR_ACCESO_DISCO;
                        }
                    }
                    else
                    {
                        return ERROR_NO_EXISTE_DIRECTORIO_INTERMEDIO;
                    }
                }
                else
                {
                    // Reserva un inodo para el nuevo fichero.
                    entrada.ninodo = reservar_inodo(tipo, permisos);

#if DEBUG

                    printf("[buscar()-> reservado inodo %d tipo %c con permisos"
                           " %d para %s]\n",
                           entrada.ninodo, tipo, permisos, entrada.nombre);

#endif

                    if (entrada.ninodo == -1)
                    {
                        return ERROR_ACCESO_DISCO;
                    }
                }
                // Guarda la nueva entrada en el directorio pertinente.
                if (mi_write_f(*(p_inodo_dir), &entrada, inodo.tamEnBytesLog,
                               sizeof(struct entrada)) == -1)
                {
                    liberar_inodo(entrada.ninodo);
                    return EXIT_FAILURE;
                }
#if DEBUG

                printf("[buscar_entrada()-> creada entrada: %s, %d]\n",
                       entrada.nombre, entrada.ninodo);

#endif
            }
            break;
        }
    }
    // Si se ha llegado al final del procesado entonces.
    if (!strcmp(final, "") || !strcmp(final, "/"))
    {
        // Si se ha encontrado el elemento y esta en modo escritura entonces.
        if ((numEntradaInodo < cantEntradasInodo) && (reservar == 1))
        {
            return ERROR_ENTRADA_YA_EXISTENTE;
        }
        // Si no devuelve la entrada pertinente.
        *(p_inodo) = entrada.ninodo;
        *(p_entrada) = numEntradaInodo;
        return EXIT_SUCCESS;
    }
    else
    {
        /* Si no se ha llegado al final se pasa a procesar el siguiente 
           elemento del camino, llamada recursiva. */
        *(p_inodo_dir) = entrada.ninodo;
        return buscar_entrada(final, p_inodo_dir, p_inodo, p_entrada, reservar,
                              permisos);
    }
}

/* Funcion: mi_creat:
* -------------------
* Utilizada para crear un fichero o un directorio y su entrada en el directorio
* padre.
*
*  camino: direccion completa con el camino donde se creara el elemento.
*  permisos: permisos que tendra el elemento.
*
* returns: Exit_Success o codigo de error si se ha producido un error.
*/
int mi_creat(const char *camino, unsigned char permisos)
{
    // Inicializacion de variables.
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    int error;

    // Ejecucion de la accion.
    if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 1,
                                permisos)) < 0)
    {
        return error;
    }
    return EXIT_SUCCESS;
}

/* Funcion: mi_dir:
* -----------------
* Esta funcion devuelve el contenido del directorio en el buffer. La 
* informacion de cada elemento del directorio es tipo, permisos, fecha de 
* modificacion, tamaño y nombre y estan divididos por el caracter '|'.
*
*  camino: direccion del direcctorio 
*  buffer: buffer donde almacenara el contenido del directorio para imprimir.
*
* returns: el total de entradas procesadas o bien el codigo de error para ser 
*          procesado por la funcion mostrar_error_directorios.
*/
int mi_dir(const char *camino, char *buffer)
{
    // Comprueba que el camino esta bien escrito.
    if (camino[0] != '/')
    {
        return ERROR_CAMINO_INCORRECTO;
    }
    // Inicializa las variables necesarias.
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    int error;

    // Realiza la busqueda del directorio en el sistema de archivos.
    if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0,
                                0)) < 0)
    {
        return error;
    }
    // Obtiene el inodo del sistema de archivos.
    struct inodo inodo;
    if (leer_inodo(p_inodo, &inodo))
    {
        return ERROR_ACCESO_DISCO;
    }
    // Comprueba que el inodo pertenezca a un directorio.
    if (inodo.tipo != 'd')
    {
        struct entrada entrada;
        if (mi_read_f(p_inodo_dir, &entrada, p_entrada * sizeof(struct entrada), sizeof(struct entrada)) < 0)
        {
            return ERROR_ACCESO_DISCO;
        }
        if ((error = formato_ls(entrada, buffer)) < 0)
        {
            return error;
        }
        return 1;
    }
    // Comprueba que el directorio tenga permisos de lectura.
    if ((inodo.permisos & 4) != 4)
    {
        return ERROR_PERMISO_LECTURA;
    }
    /* Variables utilizadas para obtener los bytes a leer y el total de 
       entradas procesadas. */
    int offset = 0;
    int totalEntradas = 0;

    // Inicializa y limpia el buffer de entradas.
    struct entrada entradas[BLOCKSIZE / sizeof(struct entrada)];
    memset(&entradas, 0, sizeof(struct entrada));

    // Lee el primer bloque de entradas del directorio.
    int bytes = mi_read_f(p_inodo, &entradas, offset, BLOCKSIZE);

    // Itera mientras haya entradas que leer.
    while (bytes > 0)
    {
        // Numero de entradas que se han leido del disco.
        int entradasBloque = bytes / sizeof(struct entrada);
        totalEntradas = totalEntradas + entradasBloque;

        // Itera por todas las entradas leidas del disco.
        int idx = 0;
        while (idx < entradasBloque)
        {
            // Leer informacion del inodo.
            if ((error = formato_ls(entradas[idx], buffer)) < 0)
            {
                return error;
            }
            idx++;
        }
        // Leer siguiente bloque de entradas del disco.
        offset = bytes + offset;
        memset(&entradas, 0, sizeof(struct entrada));
        bytes = mi_read_f(p_inodo, &entradas, offset, BLOCKSIZE);
    }
    return totalEntradas;
}

int formato_ls(struct entrada entrada, char *buffer)
{
    struct STAT stat;
    if (mi_stat_f(entrada.ninodo, &stat))
    {
        return ERROR_ACCESO_DISCO;
    }
    // Escribir entrada en el buffer con el formato adecuado.
    char array[11];
    sprintf(array, "%c\t", stat.tipo);
    strcat(buffer, array);
    if (stat.permisos & 4)
    {
        strcat(buffer, "r");
    }
    else
    {
        strcat(buffer, "-");
    }
    if (stat.permisos & 2)
    {
        strcat(buffer, "w");
    }
    else
    {
        strcat(buffer, "-");
    }
    if (stat.permisos & 1)
    {
        strcat(buffer, "x\t\t");
    }
    else
    {
        strcat(buffer, "-\t\t");
    }
    struct tm *tm;
    char tmp[100];
    tm = localtime(&stat.mtime);
    sprintf(tmp, "%d-%02d-%02d %02d:%02d:%02d\t", tm->tm_year + 1900,
            tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min,
            tm->tm_sec);
    strcat(buffer, tmp);
    sprintf(array, "%d\t", stat.tamEnBytesLog);
    strcat(buffer, array);
    strcat(buffer, entrada.nombre);
    strcat(buffer, "|");
    return EXIT_SUCCESS;
}

/* Funcion: mi_chmod:
* -------------------
* Esta funcion permite cambiar los permisos de un elemento.
*
*  camino: direccion del elemento del cual se quiere cambiar los permisos.
*  permisos: nuevos permisos que tendra el elemento.
*
* return: Devuelve Exit_Success si ha ido correctamente o bien un codigo de 
*         error para la funcion mostrar_error_directorios.
*/
int mi_chmod(const char *camino, unsigned char permisos)
{
    // Inicializacion de las variables a utilizar.
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    int error;

    // Obtiene el pinodo del elemento en el sistema de archivos.
    if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0,
                                permisos)) < 0)
    {
        return error;
    }

    //Modifica los permisos del elemento.
    if (mi_chmod_f(p_inodo, permisos))
    {
        return ERROR_ACCESO_DISCO;
    }
    return EXIT_SUCCESS;
}

/* Funcion: mi_stat:
* ------------------
* Esta funcion obtiene la metainformacion del elemento indicado por el camino.
*
*  camino: direccion del elemento a obtener la informacion.
*  p_stat: metainformacion del elemento.
*
* returns: el numero del inodo o bien un codigo de error si se ha prducido un 
*          error al buscar el elemento.
*/
int mi_stat(const char *camino, struct STAT *p_stat)
{
    // Inicializacion de las variables.
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    int error;

    // Obtencion del inodo del elemento en el sistema de archivos.
    if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0,
                                0)) < 0)
    {
        return error;
    }
    // Obtencion de la metainformacion.
    if (mi_stat_f(p_inodo, p_stat))
    {
        return ERROR_ACCESO_DISCO;
    }
    return p_inodo;
}

/* Funcion: mi_write:
* ------------------
* Esta función permite la escritura del contenido de un buffer en un archivo.
*
*  camino: direccion del archivo donde se va a escribir.
*  buf: buffer con el contenido a escribir.
*  offset: desplazamiento de bytes dentro del archivo.
*  nbytes: tamaño en bytes de buffer.
*
* returns: número de bytes escritos o bien el error generado.         
*/
int mi_write(const char *camino, const void *buf, unsigned int offset,
             unsigned int nbytes)
{
    // Inicializacion de las variables.
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    int error;
    int bytes = 0;

    // Revisa si este ha sido el último archivo accedido.
    if (strcmp(camino, UltimaEntradaEscritura.camino))
    {
        // Obtencion del inodo del elemento en el sistema de archivos.
        if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada,
                                    0, 0)) < 0)
        {
            return error;
        }
        // Actualiza la variable global del último fichero leído.
        strcpy(UltimaEntradaEscritura.camino, camino);
        UltimaEntradaEscritura.p_inodo = p_inodo;

// Si DEBUG es 1 entonces se muestran los mensajes de depuración
#if DEBUG

        printf("\n[mi_write() -> Actualizamos la caché de escritura]\n");

#endif
    }
    else
    {
        // Utiliza el p_inodo de memoria.
        p_inodo = UltimaEntradaEscritura.p_inodo;

// Si DEBUG es 1 entonces se muestran los mensajes de depuración
#if DEBUG

        printf("\n[mi_write() -> Utilizamos la caché de escritura en vez de llam"
               "ar a buscar_entrada()]\n");

#endif
    }

    // Realiza la escritura del buffer.
    bytes = mi_write_f(p_inodo, buf, offset, nbytes);
    if (bytes == -1)
    {
        return ERROR_PERMISO_ESCRITURA;
    }
    return bytes;
}

/* Funcion: mi_read:
* ------------------
* Esta función permite la lectura del contenido de un archivo en un buffer.
*
*  camino: direccion del archivo donde se va a escribir.
*  buf: buffer con el contenido a escribir.
*  offset: desplazamiento de bytes dentro del archivo.
*  nbytes: tamaño en bytes de buffer.
*
* returns: número de bytes leídos o bien el error generado.         
*/
int mi_read(const char *camino, void *buf, unsigned int offset,
            unsigned int nbytes)
{
    // Inicializacion de las variables.
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    int error;
    int bytes = 0;

    // Revisa si este ha sido el último archivo accedido.
    if (strcmp(camino, UltimaEntradaLectura.camino))
    {
        // Obtencion del inodo del elemento en el sistema de archivos.
        if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada,
                                    0, 0)) < 0)
        {
            return error;
        }
        // Actualiza la variable global del último fichero leído.
        strcpy(UltimaEntradaLectura.camino, camino);
        UltimaEntradaLectura.p_inodo = p_inodo;

// Si DEBUG es 1 entonces se muestran los mensajes de depuración
#if DEBUG

        printf("\n[mi_read() -> Actualizamos la caché de lectura]\n");

#endif
    }
    else
    {
        // Utiliza el p_inodo de memoria.
        p_inodo = UltimaEntradaLectura.p_inodo;

// Si DEBUG es 1 entonces se muestran los mensajes de depuración
#if DEBUG

        printf("\n[mi_read() -> Utilizamos la caché de lectura en vez de llamar"
               "a buscar_entrada()]\n");

#endif
    }

    // Realiza la lectura del archivo.
    bytes = mi_read_f(p_inodo, buf, offset, nbytes);
    if (bytes == -1)
    {
        return ERROR_PERMISO_LECTURA;
    }
    return bytes;
}

/* Funcion: mi_link:
* ------------------
* Esta funcion crea un link al contenido de un fichero en una nueva 
* localizacion.
* 
*  camino1: direccion del archivo a linkear.
*  camino2: direccion donde se crea el link al archivo.
*
* return: Exit_Success o codigo de error a procesar por 
*         mostrar_error_directorio.
*/
int mi_link(const char *camino1, const char *camino2)
{
    // Inicializacion de las variables.
    unsigned int p_inodo_dir1 = 0;
    unsigned int p_inodo1 = 0;
    unsigned int p_entrada1 = 0;
    int error;

    // Busca el archivo a linkear en el disco.
    if ((error = buscar_entrada(camino1, &p_inodo_dir1, &p_inodo1,
                                &p_entrada1, 0, 0)) < 0)
    {
        return error;
    }

    // Lee el inodo relacionado con el archivo a linkear.
    struct inodo inodo1;
    if (leer_inodo(p_inodo1, &inodo1))
    {
        return ERROR_ACCESO_DISCO;
    }
    // Comprueba que el archivo tenga permisos de lectura.
    if ((inodo1.permisos & 4) != 4)
    {
        return ERROR_PERMISO_LECTURA;
    }
    // Inicializacion de las variables.
    unsigned int p_inodo_dir2 = 0;
    unsigned int p_inodo2 = 0;
    unsigned int p_entrada2 = 0;

    // Crea la entrada del link en el directorio correspondiente.
    if ((error = buscar_entrada(camino2, &p_inodo_dir2, &p_inodo2,
                                &p_entrada2, 1, 6)) < 0)
    {
        return error;
    }

    // Lee la entrada del link en el directorio.
    struct entrada entrada2;
    if (mi_read_f(p_inodo_dir2, &entrada2,
                  sizeof(struct entrada) * (p_entrada2),
                  sizeof(struct entrada)) < 0)
    {
        return ERROR_ACCESO_DISCO;
    }

    // Actualiza el inodo enlazado al camino.
    entrada2.ninodo = p_inodo1;

    // Escribe la entrada del link en el directorio.
    if (mi_write_f(p_inodo_dir2, &entrada2,
                   sizeof(struct entrada) * (p_entrada2),
                   sizeof(struct entrada)) < 0)
    {
        return ERROR_ACCESO_DISCO;
    }

    // Libera el inodo creado con el buscar_entrada del link.
    if (liberar_inodo(p_inodo2) < 0)
    {
        return ERROR_ACCESO_DISCO;
    }

    // Actualiza los metadatos el inodo del archivo 1 y lo guarda.
    inodo1.nlinks = inodo1.nlinks + 1;
    inodo1.ctime = time(NULL);
    if (escribir_inodo(p_inodo1, inodo1))
    {
        return ERROR_ACCESO_DISCO;
    }
    return EXIT_SUCCESS;
}

/* Funcion: mi_unlink:
* --------------------
* Borra la entrada de directorio especificada, esta funcion sirve tanto para
* borrar un enlace a un fichero como el contenido de este si no quedan enlaces.
*
*  camino: direccion del archivo a borrar.
*
* returns: Exit_Success o bien un codigo de error para ser tratado por 
*          mostrar_error_directorios.
*/
int mi_unlink(const char *camino)
{
    // Inicializacion de las variables.
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    int error;

    // Busca el archivo a linkear en el disco.
    if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo,
                                &p_entrada, 0, 0)) < 0)
    {
        return error;
    }
    // Lee el inodo del archivo a borrar.
    struct inodo inodo;
    if (leer_inodo(p_inodo, &inodo))
    {
        return ERROR_ACCESO_DISCO;
    }
    // Comprueba si es un directorio y esta vacio.
    if (inodo.tipo == 'd' && inodo.tamEnBytesLog > 0)
    {
        return ERROR_DIRECTORIO_NO_VACIO;
    }
    // Lee el inodo del directorio.
    struct inodo inodo_dir;
    if (leer_inodo(p_inodo_dir, &inodo_dir))
    {
        return ERROR_ACCESO_DISCO;
    }
    // Obtiene el numero de entradas que contiene el directorio.
    int num_entradas = inodo_dir.tamEnBytesLog / sizeof(struct entrada);

    // Comprueba si la entrada a eliminar es la ultima.
    if (p_entrada == num_entradas - 1)
    {
        // Elimina la ultima entrada.
        if (mi_truncar_f(p_inodo_dir, inodo_dir.tamEnBytesLog - sizeof(struct entrada)) < 0)
        {
            return ERROR_ACCESO_DISCO;
        }
    }
    else
    {
        // Lee la ultima entrada del directorio.
        struct entrada entrada;
        if (mi_read_f(p_inodo_dir, &entrada,
                      sizeof(struct entrada) * (num_entradas - 1),
                      sizeof(struct entrada)) < 0)
        {
            return ERROR_ACCESO_DISCO;
        }
        // Escribe la ultima entrada en la posicion de la entrada a borrar.
        if (mi_write_f(p_inodo_dir, &entrada,
                       sizeof(struct entrada) * p_entrada,
                       sizeof(struct entrada)) < 0)
        {
            return ERROR_ACCESO_DISCO;
        }
        // Elimina la ultima entrada.
        if (mi_truncar_f(p_inodo_dir, inodo_dir.tamEnBytesLog - sizeof(struct entrada)) < 0)
        {
            return ERROR_ACCESO_DISCO;
        }
    }
    // Decrementa el numero de enlaces al inodo.
    inodo.nlinks = inodo.nlinks - 1;

    // Si no quedan enlaces al inodo entonces se elimina.
    if (!inodo.nlinks)
    {
        if (liberar_inodo(p_inodo) < 0)
        {
            return ERROR_ACCESO_DISCO;
        }
    }
    else
    {
        // Actualiza el ctime y guarda el inodo.
        inodo.ctime = time(NULL);
        if (escribir_inodo(p_inodo, inodo))
        {
            return ERROR_ACCESO_DISCO;
        }
    }
    return EXIT_SUCCESS;
}

/* Funcion: mi_unlink_r:
* ----------------------
* Esta funcion borra de forma recursiva todos los elementos dentro de la 
* estructura de directorios.
*
*  camino: ruta del directorio donde comienza la eliminacion de elementos.
*
* returns: Exit_Success o bien un codigo de error a ser tratado por 
*          mostrar_error_directorios. 
*/
int mi_unlink_r(const char *camino)
{
    // Inicializacion de las variables.
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    int error;

    // Busca el p_inodo del directorio.
    if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo,
                                &p_entrada, 0, 0)) < 0)
    {
        printf("%s\n", camino);
        return error;
    }
    // Variables para la lectura del contenido del inodo.
    struct entrada entrada[BLOCKSIZE / sizeof(struct entrada)];
    int offset = 0;
    int leidos;

    // Lee el contenido del inodo.
    if ((leidos = mi_read_f(p_inodo, &entrada, offset, BLOCKSIZE)) < 0)
    {
        return ERROR_ACCESO_DISCO;
    }
    // Recorre todas las entradas del directorio.
    while (leidos > 0)
    {
        // Actualiza el offset de lectura e indice.
        int offset = offset + leidos;
        int entradas = 0;

        // Recorre todas las entradas leidas.
        while (entradas < (leidos / sizeof(struct entrada)))
        {
            // Lee el inodo de la entrada.
            struct inodo inodo;
            if (leer_inodo(entrada[entradas].ninodo, &inodo))
            {
                return ERROR_ACCESO_DISCO;
            }

            printf("tipo : %c \n", inodo.tipo);

            // Comprueba si es un directorio.
            if (inodo.tipo == 'd')
            {
                char subcamino[60];
                memset(subcamino, 0, sizeof(char) * 60);
                strcat(subcamino, camino);
                strcat(subcamino, entrada[entradas].nombre);
                strcat(subcamino, "/");
                printf("Camino : %s\n", subcamino);
                // Si es un directorio, entonces llama a la funcion otra vez.
                if ((error = mi_unlink_r(subcamino)) < 0)
                {
                    return error;
                }
            }
            else
            {
                char subcamino[60];
                memset(subcamino, 0, sizeof(char) * 60);
                strcat(subcamino, camino);
                strcat(subcamino, entrada[entradas].nombre);
                // Si es un fichero, borra este del sistema.
                if ((error = mi_unlink(subcamino)) < 0)
                {
                    return error;
                }
            }
            // Procesa siquiente entrada.
            entradas++;
        }
        // Lee siguiente conjunto de entradas.
        if ((leidos = mi_read_f(p_inodo, &entrada, offset, BLOCKSIZE)) < 0)
        {
            return ERROR_ACCESO_DISCO;
        }
    }
    // Elimina el directorio indicado por parametro.
    if ((error = mi_unlink(camino)) < 0)
    {
        return error;
    }
    return EXIT_SUCCESS;
}

/* Funcion: mostrar_error_directorios:
* ------------------------------------
* Esta funcion trata los error producidos por las funciones de la libreria 
* directorios, mostrando la excepcion por pantalla.
*
*  error: codigo de error.
*/
void mostrar_error_directorios(int error)
{
    switch (error)
    {
    case -1:
        fprintf(stderr, "Error: Camino incorrecto.\n");
        break;
    case -2:
        fprintf(stderr, "Error: Permiso denegado de lectura.\n");
        break;
    case -3:
        fprintf(stderr, "Error: No existe el archivo o el directorio.\n");
        break;
    case -4:
        fprintf(stderr, "Error: No existe algún directorio intermedio.\n");
        break;
    case -5:
        fprintf(stderr, "Error: Permiso denegado de escritura.\n");
        break;
    case -6:
        fprintf(stderr, "Error: El archivo ya existe.\n");
        break;
    case -7:
        fprintf(stderr, "Error: No es un directorio.\n");
        break;
    case -8:
        fprintf(stderr, "Error: No se ha podico acceder a disco.\n");
        break;
    case -9:
        fprintf(stderr, "Error: El directorio no esta vacio.\n");
    }
}
