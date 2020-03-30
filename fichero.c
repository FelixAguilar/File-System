#include "fichero.h"

/* Funcion: mi_write_f:
* ---------------------
* Esta funcion permite escribir el contenido del buffer en la unidad virutal.
*
*  ninodo: numero de nodo en el array de inodos.
*  buf_original: buffer con el contenido del archivo.
*  offset: numero de byte a partir del cual se empiezan a escribir los datos.
*  nbytes: numero de bytes que contiene el buf_original.
*
* return: numero de bytes que se ha podido escribir si no -1.
*/
int mi_write_f(unsigned int ninodo, const void *buf_original,
               unsigned int offset, unsigned int nbytes)
{
    // Lee el inodo indicado por parametro.
    struct inodo inodo;
    if (leer_inodo(ninodo, &inodo))
    {
        perror("Error");
        return -1;
    }
    // Comprueba si el inodo apunta a un archivo con permiso de escritura.
    if ((inodo.permisos & 2) != 2)
    {
        perror("Error");
        return -1;
    }
    // Calcula el primer y ultimo bloque de la escritura.
    int bloqueLI = offset / BLOCKSIZE;
    int bloqueLF = (offset + nbytes - 1) / BLOCKSIZE;

    // Obtiene el bloque fisico del inodo.
    int bloquef = traducir_bloque_inodo(ninodo, bloqueLI, 1);
    if (bloquef == -1)
    {
        perror("Error");
        return -1;
    }
    // Lee el bloque del disco.
    char buf_bloque[BLOCKSIZE];
    if (bread(bloquef, buf_bloque) == -1)
    {
        perror("Error");
        return -1;
    }

    // Calcula el desplazamiento en el bloque.
    int desp1 = offset % BLOCKSIZE;

    // Si el archivo es menor que el tamaño de un bloque.
    if (bloqueLI == bloqueLF)
    {
        // Copiamos el contenido de buf_original en el buf_bloque.
        memcpy(buf_bloque + desp1, buf_original, nbytes);
    }
    else
    {
        // Copiamos el contenido de buf_original en el buf_bloque.
        memcpy(buf_bloque + desp1, buf_original, BLOCKSIZE - desp1);
    }
    // Escribe el bloque en el disco virtual.
    int bytes_escritos = 0;
    int aux = bwrite(bloquef, buf_bloque);
    if (aux == -1)
    {
        perror("Error");
        return -1;
    }
    if (bloqueLI == bloqueLF)
    {
        bytes_escritos = (aux - desp1) - (BLOCKSIZE - desp1 - nbytes);
    }
    else
    {
        bytes_escritos = aux - desp1;
    }

    // Caso en que el archivo vaya a ocupar mas de un bloque.
    if (bloqueLI != bloqueLF)
    {
        // Tratamiento bloques intermedios.
        int i = bloqueLI + 1;

        while (i < bloqueLF)
        {
            // Obtiene el bloque fisico intermedio en el arcivo.
            bloquef = traducir_bloque_inodo(ninodo, i, 1);
            if (bloquef == -1)
            {
                perror("Error");
                return -1;
            }
            // Escribe el bloque intermiedio en el disco virtual.
            aux = bwrite(bloquef, buf_original + (BLOCKSIZE - desp1) +
                                      (i - bloqueLI - 1) * BLOCKSIZE);
            if (aux == -1)
            {
                perror("Error");
                return -1;
            }
            bytes_escritos = bytes_escritos + aux;
            i++;
        }
        // Obtiene el bloque fisico final.
        bloquef = traducir_bloque_inodo(ninodo, bloqueLF, 1);
        if (bloquef == -1)
        {
            perror("Error");
            return -1;
        }
        // Lee el bloque del disco.
        if (bread(bloquef, buf_bloque) == -1)
        {
            perror("Error");
            return -1;
        }
        // Calcula el desplazamiento final.
        int desp2 = (offset + nbytes - 1) % BLOCKSIZE;

        // Copia los bytes pertinentes en el buffer del bloque.
        memcpy(buf_bloque, buf_original + (nbytes - desp2 - 1), desp2 + 1);

        // Escribe el buffer en el bloque fisico.
        aux = bwrite(bloquef, buf_bloque);
        if (aux == -1)
        {
            perror("Error");
            return -1;
        }
        bytes_escritos = bytes_escritos + desp2 + 1;
    }
    // Lee el inodo despues de la operacion de escritura del archivo.
    if (leer_inodo(ninodo, &inodo))
    {
        perror("Error");
        return -1;
    }
    // Actualiza el tamaño logico si es mayor que el archivo en el inodo.
    if ((offset + nbytes) > inodo.tamEnBytesLog)
    {
        inodo.tamEnBytesLog = offset + nbytes;
        inodo.ctime = time(NULL);
    }
    // Actualiza el tiempo de modificacion en la zona de datos.
    inodo.mtime = time(NULL);

    // Escribe el inodo en el disco virtual.
    if (escribir_inodo(ninodo, inodo))
    {
        perror("Error");
        return -1;
    }
    return bytes_escritos;
}

int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset, unsigned int nbytes)
{
    int desp1; //deplazamiento
    int desp2;
    char buf_bloque[BLOCKSIZE];
    int bloque; //bloque fisico
    int leidos;
    int primerBLogico; //inicio donde escribimos
    int ultimoBLogico; //fin donde escribimos
    struct inodo inodo;
    leer_inodo(ninodo, &inodo);
    if ((inodo.permisos & 4) == 4)
    {
        if(offset >= inodo.tamEnBytesLog)
        {
            leidos = 0; // No podemos leer nada
            return leidos;
        }    
        if(offset + nbytes >= inodo.tamEnBytesLog)
        {
            nbytes = inodo.tamEnBytesLog - offset; // leemos sólo los bytes desde el offset hasta EOF 
        }
        primerBLogico = offset / BLOCKSIZE;
        ultimoBLogico = (offset + nbytes - 1) / BLOCKSIZE;
        desp1 = offset % BLOCKSIZE;
        desp2 = (offset + nbytes - 1) % BLOCKSIZE;
        bloque = traducir_bloque_inodo(ninodo, primerBLogico,0);
        if(primerBLogico == ultimoBLogico)
        {
            if (bloque!=-1)
            {
                bread(bloque, buf_bloque);
                memcpy(buf_original,buf_bloque + desp1, nbytes);
            }
            leidos = nbytes;
        }
        else
        {
            //Primer Bloque
            if (bloque!=-1)
            {
                bread(bloque, buf_bloque);
                memcpy(buf_original,buf_bloque + desp1, BLOCKSIZE-desp1);
            }
            leidos = BLOCKSIZE-desp1;

            //Bloque intermedios
            for (int i = primerBLogico+1; i < ultimoBLogico; i++)
            {
                bloque = traducir_bloque_inodo(ninodo, i,0);
                if (bloque != -1)
                {
                    bread(bloque, buf_original + (BLOCKSIZE - desp1) + (i - primerBLogico - 1) * BLOCKSIZE);
                }
                leidos += BLOCKSIZE;
            }
            
            //Ultimo bloque
            bloque = traducir_bloque_inodo(ninodo, ultimoBLogico,0);
            if (bloque != -1)
            {
                bread(bloque, buf_bloque);
                memcpy (buf_original + (nbytes - desp2 - 1), buf_bloque, desp2 + 1);
            }
            leidos += desp2+1;  
        }
        leer_inodo(ninodo, &inodo);
        localtime(&inodo.atime);
        escribir_inodo(ninodo, inodo);
        return leidos;
    }
    else
    {
        printf("No tienes permisos de lectura");
        return -1;
    }
    
}

/* Funcion: mi_stat_f:
* ---------------------
* Esta funcion devuelve la metainformación de un fichero/directorio.
*
*  ninodo: numero de nodo en el array de inodos.
*  p_stat: tipo estructurado que contiene los mismos campos que un inodo excepto los punteros.
*  
* return: EXIT_FAILURE si se ha producido un error leyendo el inodo o EXIT_SUCCESS en caso contrario.
*/
int mi_stat_f(unsigned int ninodo, struct STAT *p_stat)
{
    // Lee el inodo indicado por parametro.
    struct inodo inodo;
    if (leer_inodo(ninodo, &inodo))
    {
        perror("Error");
        return EXIT_FAILURE;
    }

    p_stat->tipo = inodo.tipo;
    p_stat->permisos = inodo.permisos;
    p_stat->nlinks = inodo.nlinks;
    p_stat->tamEnBytesLog = inodo.tamEnBytesLog;
    p_stat->atime = inodo.atime;
    p_stat->ctime = inodo.ctime;
    p_stat->mtime = inodo.mtime;
    p_stat->numBloquesOcupados = inodo.numBloquesOcupados;

    return EXIT_SUCCESS;
}

/* Funcion: mi_chmod_f:
* ---------------------
* Esta funcion cambia los permisos de un fichero/directorio.
*
*  ninodo: numero de nodo en el array de inodos.
*  permisos: tipo de permiso que se quiere establecer.
*  
* return: EXIT_FAILURE si se ha producido un error leyendo el inodo o EXIT_SUCCESS en caso contrario.
*/
int mi_chmod_f(unsigned int ninodo, unsigned char permisos)
{
    // Lee el inodo indicado por parametro.
    struct inodo inodo;
    if (leer_inodo(ninodo, &inodo))
    {
        perror("Error");
        return EXIT_FAILURE;
    }

    inodo.permisos = permisos;

    inodo.ctime = time(NULL);

    if (escribir_inodo(ninodo, inodo))
    {
        perror("Error");
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
