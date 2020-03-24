#include "fichero.h";

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
    bytes_escritos = aux - desp1;

    // Caso en que el archivo vaya a ocupar mas de un bloque.
    if (bloqueLI != bloqueLF)
    {
        // Tratamiento bloques intermedios.
        int i = bloqueLI++;
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
        bytes_escritos = bytes_escritos + aux - desp2;
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

int mi_read_f(unsigned int ninodo, void *buf_original, unsigned int offset,
              unsigned int nbytes)
{
    // Lee el inodo indicado por parametro.
    struct inodo inodo;
    if (leer_inodo(ninodo, &inodo))
    {
        perror("Error");
        return -1;
    }
    // Comprueba si el inodo apunta a un archivo con permiso de lectura.
    if ((inodo.permisos & 4) != 4)
    {
        perror("Error");
        return -1;
    }

    if (offset >= inodo.tamEnBytesLog)
    {
        return 0;
    }
    if (offset + nbytes >= inodo.tamEnBytesLog)
    {
        nbytes = inodo.tamEnBytesLog - offset;
    }

    // Calcula el primer y ultimo bloque de la lectura.
    int bloqueLI = offset / BLOCKSIZE;
    int bloqueLF = (offset + nbytes - 1) / BLOCKSIZE;

    // Obtiene el bloque fisico del inodo.
    int bloquef = traducir_bloque_inodo(ninodo, bloqueLI, 0);
    if (bloquef == -1)
    {
        perror("Error");
        return -1;
    }
    // Lee el bloque del disco.
    char buf_bloque[BLOCKSIZE];
    int bytes_leidos = bread(bloquef, buf_bloque);
    if (bytes_leidos == -1)
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
        memcpy(buf_original, buf_bloque + desp1, nbytes);
    }
    else
    {
        // Copiamos el contenido de buf_original en el buf_bloque.
        memcpy(buf_original, buf_bloque + desp1, BLOCKSIZE - desp1);
    }

    // Caso en que el archivo vaya a ocupar mas de un bloque.
    if (bloqueLI != bloqueLF)
    {
        // Tratamiento bloques intermedios.
        int i = bloqueLI++;
        while (i < bloqueLF)
        {
            // Obtiene el bloque fisico intermedio en el arcivo.
            bloquef = traducir_bloque_inodo(ninodo, i, 0);
            if (bloquef == -1)
            {
                perror("Error");
                return -1;
            }
            // Escribe el bloque intermiedio en el disco virtual.
            int aux = bread(bloquef, buf_original + (BLOCKSIZE - desp1) +
                                         (i - bloqueLI - 1) * BLOCKSIZE);
            if (aux == -1)
            {
                perror("Error");
                return -1;
            }
            bytes_leidos = bytes_leidos + aux;
            i++;
        }
        // Obtiene el bloque fisico final.
        bloquef = traducir_bloque_inodo(ninodo, bloqueLF, 0);
        if (bloquef == -1)
        {
            perror("Error");
            return -1;
        }
        // Lee el bloque del disco.
        int aux = bread(bloquef, buf_bloque);
        if (aux == -1)
        {
            perror("Error");
            return -1;
        }
        // Calcula el desplazamiento final.
        int desp2 = (offset + nbytes - 1) % BLOCKSIZE;

        // Escribe el buffer en el bloque fisico.
        aux = bwrite(bloquef, buf_bloque);
        if (aux == -1)
        {
            perror("Error");
            return -1;
        }
        bytes_leidos = bytes_leidos + aux - desp2;

        // Copia los bytes pertinentes en el buffer del bloque.
        memcpy(buf_original + (nbytes - desp2 - 1), buf_bloque, desp2 + 1);
    }
    // Lee el inodo despues de la operacion de escritura del archivo.
    if (leer_inodo(ninodo, &inodo))
    {
        perror("Error");
        return -1;
    }

    inodo.atime = time(NULL);

    // Escribe el inodo en el disco virtual.
    if (escribir_inodo(ninodo, inodo))
    {
        perror("Error");
        return -1;
    }
    return bytes_leidos;
}