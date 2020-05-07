// Autores: Félix Aguilar, Adrián Bennasar, Álvaro Bueno
#include "directorios.h"

/* Función: extraer_camino:
* -----------------
* Esta función obtiene el descriptor del fichero pasado por parámetro con la 
* llamada al sistema open(). Si no existe el fichero lo crea y si ya existe lo 
* abre en modo lectura/escritura.
* 
*  camino: dirección del fichero del dispositivo virtual.
*
* returns: descriptor o -1 si se produce un error al intentar abrir el fichero.
*/

int extraer_camino(const char *camino, char *inicial, char *final, char *tipo)
{
    if (*(camino) != '/')
    {
        return EXIT_FAILURE;
    }

    char *f = strchr((camino + 1), '/');

    *(tipo) = 'f';

    if (f)
    {
        strncpy(inicial, (camino + 1), (strlen(camino) - strlen(f) - 1));

        strcpy(final, f);

        if (final[0] == '/')
        {
            *(tipo) = 'd';
        }
    }
    else
    {
        strcpy(inicial, (camino + 1));
        final[0] = '\0';
    }

    return EXIT_SUCCESS;
}

int buscar_entrada(const char *camino_parcial, unsigned int *p_inodo_dir,
                   unsigned int *p_inodo, unsigned int *p_entrada,
                   char reservar, unsigned char permisos)
{

    // Variables

    struct superbloque SB;
    struct entrada entrada;
    struct inodo inodo;
    char inicial[sizeof(entrada.nombre)];
    char final[strlen(camino_parcial)];
    char tipo;
    // unsigned int punteros[BLOCKSIZE / sizeof(unsigned int)];
    memset(entrada.nombre, 0, sizeof(entrada.nombre));
    memset(inicial, 0, sizeof(entrada.nombre));
    memset(final, 0, strlen(camino_parcial));

    if (!strcmp(camino_parcial, "/"))
    {
        if (bread(SBPOS, &SB) == -1)
        {
            return -1;
        }
        *(p_inodo) = SB.posInodoRaiz;
        *(p_entrada) = 0;
        return 0;
    }

    if (extraer_camino(camino_parcial, inicial, final, &tipo))
    {
        return ERROR_CAMINO_INCORRECTO;
    }
    printf("[buscar_entrada()->inicial:%s,final:%s,reservar:%d]\n", inicial,
           final, reservar);
    if (leer_inodo(*(p_inodo_dir), &inodo))
    {
        return -1;
    }
    if ((inodo.permisos & 4) != 4)
    {
        printf("[buscar_entrada()->El inodo %d no tiene permisos de lectura\n", *(p_inodo_dir));
        return ERROR_PERMISO_LECTURA;
    }

    int cantEntradasInodo = inodo.tamEnBytesLog / sizeof(struct entrada);
    int numEntradaInodo = 0;
    if (cantEntradasInodo > 0)
    {
        struct entrada entradas[BLOCKSIZE / sizeof(struct entrada)];
        while ((numEntradaInodo < cantEntradasInodo) && strcmp(entrada.nombre,
                                                               inicial))
        {
            int bloquef = traducir_bloque_inodo(*(p_inodo_dir),
                                                numEntradaInodo / sizeof(struct entrada), 0);
            if (bloquef == -1)
            {
                return -1;
            }
            memset(entradas, 0, BLOCKSIZE);
            if (bread(bloquef, entradas) == -1)
            {
                return -1;
            }
            int numEntradaArray = 0;
            while (numEntradaArray < (BLOCKSIZE / sizeof(struct entrada)) &&
                   strcmp(entradas[numEntradaArray].nombre, inicial) && (numEntradaInodo < cantEntradasInodo))
            {
                numEntradaArray++;
                numEntradaInodo++;
            }
            if (!strcmp(entradas[numEntradaArray].nombre, inicial))
            {
                // entrada = entradas[numEntradaArray];
                strcpy(entrada.nombre, entradas[numEntradaArray].nombre);
                entrada.ninodo = entradas[numEntradaArray].ninodo;
            }
        }
    }
    if (cantEntradasInodo == numEntradaInodo && strcmp(entrada.nombre, inicial))
    {
        switch (reservar)
        {
        case 0:
            return ERROR_NO_EXISTE_ENTRADA_CONSULTA;
            break;

        case 1:
            if (inodo.tipo == 'f')
            {
                return ERROR_NO_SE_PUEDE_CREAR_ENTRADA_EN_UN_FICHERO;
            }
            if ((inodo.permisos & 2) != 2)
            {
                return ERROR_PERMISO_ESCRITURA;
            }
            else
            {
                strcpy(entrada.nombre, inicial);
                if (tipo == 'd')
                {
                    if (!strcmp(final, "/"))
                    {
                        entrada.ninodo = reservar_inodo(tipo, permisos);
                        printf("[buscar()->reservado inodo %d tipo %c con perm"
                               "isos %d para %s]\n",
                               entrada.ninodo, tipo, permisos, entrada.nombre);
                        if (entrada.ninodo == -1)
                        {
                            return -1;
                        }
                    }
                    else
                    {
                        return ERROR_NO_EXISTE_DIRECTORIO_INTERMEDIO;
                    }
                }
                else
                {
                    entrada.ninodo = reservar_inodo(tipo, permisos);
                    printf("[buscar()->reservado inodo %d tipo %c con perm"
                           "isos %d para %s]\n",
                           entrada.ninodo, tipo, permisos, entrada.nombre);
                    if (entrada.ninodo == -1)
                    {
                        return -1;
                    }
                }
                if (mi_write_f(*(p_inodo_dir), &entrada, inodo.tamEnBytesLog,
                               sizeof(struct entrada)) == -1)
                {
                    liberar_inodo(entrada.ninodo);
                }
                printf("[buscar_entrada()->creada entrada: %s, %d]\n",
                       entrada.nombre, entrada.ninodo);
                return EXIT_FAILURE;
            }

            break;

        default:
            break;
        }
    }
    if (!strcmp(final, "") || !strcmp(final, "/"))
    {
        if ((numEntradaInodo < cantEntradasInodo) && (reservar == 1))
        {
            return ERROR_ENTRADA_YA_EXISTENTE;
        }
        *(p_inodo) = entrada.ninodo;
        *(p_entrada) = numEntradaInodo;
        return EXIT_SUCCESS;
    }
    else
    {
        *(p_inodo_dir) = entrada.ninodo;
        return buscar_entrada(final, p_inodo_dir, p_inodo, p_entrada, reservar,
                              permisos);
    }
}

void mostrar_error_buscar_entrada(int error)
{
    // fprintf(stderr, "Error: %d\n", error);
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
    }
}

int mi_creat(const char *camino, unsigned char permisos)
{
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;

    int error;

    if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 1, permisos)) < 0)
    {
        mostrar_error_buscar_entrada(error);
    }

    return EXIT_SUCCESS;
}

int mi_dir(const char *camino, char *buffer)
{

    if (camino[strlen(camino) - 1] != '/')
    {
        // error
    }

    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;
    int error;
    if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 0)) < 0)
    {
        mostrar_error_buscar_entrada(error);
        return -1;
    }

    struct inodo inodo;
    if (leer_inodo(p_inodo, &inodo))
    {
        // error
    }

    if ((inodo.permisos & 4) != 4)
    {
        //error
    }

    if (inodo.tipo != 'd')
    {
        // 
    }

    struct entrada entrada[BLOCKSIZE / sizeof(struct entrada)];
    struct STAT stat;
    int offset = 0;
    int totalEntradas = 0;
    memset(&entrada, 0, sizeof(struct entrada));
    int bytes = mi_read_f(p_inodo, &entrada, offset, BLOCKSIZE);
    while (bytes > 0)
    {
        int entradasBloque = bytes / sizeof(struct entrada);
        totalEntradas = totalEntradas + entradasBloque;
        int idx = 0;
        while (idx < entradasBloque)
        {
            
            // Leer informacion del inodo.
            if (mi_stat_f(entrada[idx].ninodo, &stat))
            {
                // Error
            }

            // Escribir entrada en el buffer
            char array[10];
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
                    tm->tm_mon + 1, tm->tm_mday, tm->tm_hour, tm->tm_min, tm->tm_sec);
            strcat(buffer, tmp);
            sprintf(array, "%d\t", stat.tamEnBytesLog);
            strcat(buffer, array);
            strcat(buffer, entrada[idx].nombre);
            strcat(buffer, "|");
            idx++;
        }

        // Leer siguiente entrada.
        offset = bytes + offset;
        memset(&entrada, 0, sizeof(struct entrada));
        bytes = mi_read_f(p_inodo, &entrada, offset, BLOCKSIZE);
    }
    return totalEntradas;
}

int mi_chmod(const char *camino, unsigned char permisos)
{
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;

    int error;

    if ((error = buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, permisos)) < 0)
    {
        mostrar_error_buscar_entrada(error);
    }

    if (!p_inodo)
    {
        // error
    }

    mi_chmod_f(p_inodo, permisos);
    return EXIT_SUCCESS;
}

int mi_stat(const char *camino, struct STAT *p_stat)
{
    unsigned int p_inodo_dir = 0;
    unsigned int p_inodo = 0;
    unsigned int p_entrada = 0;

    if (!(buscar_entrada(camino, &p_inodo_dir, &p_inodo, &p_entrada, 0, 0)))
    {
        // error
    }

    if (!p_inodo)
    {
        // error
    }

    mi_stat_f(p_inodo, p_stat);
    return EXIT_SUCCESS;
}
