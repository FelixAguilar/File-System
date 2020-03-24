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
    int bytes_escritos = 0;

    // Lee el inodo indicado por parametro.
    struct inodo inodo;
    if (leer_inodo(ninodo, &inodo))
    {
        perror("Error");
        return bytes_escritos;
    }
    // Comprueba si el inodo apunta a un archivo con permiso de escritura.
    if ((inodo.permisos & 2) != 2)
    {
        perror("Error");
        return bytes_escritos;
    }
    // Calcula el primer y ultimo bloque de la escritura.
    int bloqueLI = offset / BLOCKSIZE;
    int bloqueLF = (offset + nbytes - 1) / BLOCKSIZE;

    // Obtiene el bloque fisico del inodo.
    int bloquef = traducir_bloque_inodo(ninodo, bloqueLI, 1);
    if (bloquef == -1)
    {
        perror("Error");
        return bytes_escritos;
    }
    // Lee el bloque del disco.
    char buf_bloque[BLOCKSIZE];
    if (bread(bloquef, buf_bloque) == -1)
    {
        perror("Error");
        return bytes_escritos;
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
    if (bwrite(bloquef, buf_bloque) == -1)
    {
        perror("Error");
        return bytes_escritos;
    }
    bytes_escritos = BLOCKSIZE - desp1;

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
                return bytes_escritos;
            }
            // Escribe el bloque intermiedio en el disco virtual.
            if (bwrite(bloquef, buf_original + (BLOCKSIZE - desp1) +
                                    (i - bloqueLI - 1) * BLOCKSIZE) == -1)
            {
                perror("Error");
                return bytes_escritos;
            }
            bytes_escritos = bytes_escritos + BLOCKSIZE;
            i++;
        }
        // Obtiene el bloque fisico final.
        bloquef = traducir_bloque_inodo(ninodo, bloqueLF, 1);
        if (bloquef == -1)
        {
            perror("Error");
            return bytes_escritos;
        }
        // Lee el bloque del disco.
        if (bread(bloquef, buf_bloque) == -1)
        {
            perror("Error");
            return bytes_escritos;
        }
        // Calcula el desplazamiento final.
        int desp2 = (offset + nbytes - 1) % BLOCKSIZE;

        // Copia los bytes pertinentes en el buffer del bloque.
        memcpy(buf_bloque, buf_original + (nbytes - desp2 - 1), desp2 + 1);

        // Escribe el buffer en el bloque fisico.
        if (bwrite(bloquef, buf_bloque) == -1)
        {
            perror("Error");
            return bytes_escritos;
        }
        bytes_escritos = bytes_escritos + desp2 + 1;
    }
    // Lee el inodo despues de la operacion de escritura del archivo.
    if (leer_inodo(ninodo, &inodo))
    {
        perror("Error");
        return bytes_escritos;
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
        return bytes_escritos;
    }
    return bytes_escritos;
}