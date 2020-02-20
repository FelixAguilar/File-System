#include "bloques.h"

static int descriptor; // Descriptor del archivo usado como disco virtual.

/* Funcion: bread
 * --------------
 * Lee del dispositivo virtual el bloque especificado por nbloque y copia su 
 * contenido en el buffer apuntado por *buf.
 * 
 *  nbloque: posicion del bloque que se quiere leer.
 *  buf: direccion de memoria del buffer para guardar el valor.
 * 
 * returns: numero de bytes leidos o bien EXIT_FAILURE si se produce un error.
*/

int bread(unsigned int nbloque, void *buf)
{
    // Revisa que el descriptor exista y prueba de reubicar el puntero en este.
    if (descriptor)
    {
        if (lseek(descriptor, nbloque * BLOCKSIZE, SEEK_SET) != -1)
        {
            // Si ha sido posible, lee el contenido a partir de este bloque
            size_t bytes = read(descriptor, buf, BLOCKSIZE);
            if (bytes != -1)
            {
                // Si no hubo errores, devuelve el numero de bytes leidos.
                return bytes;
            }
            else
            {
                // Si no, devuelve el error que ha ocurrido.
                fprintf(stderr, "%s", strerror(errno));
            }
        }
        else
        {
            fprintf(stderr, "%s", strerror(errno));
        }
    }
    return EXIT_FAILURE;
}

/*
* Function:  bumount
* --------------------
* Libera el descriptor de ficheros con la función close() 
*
*  returns: Devuelve EXIT_SUCCESS si se ha cerrado el fichero correctamente y 
*           EXIT_FAILURE si ha habido algún error.
*/
int bumount()
{
    if (close(descriptor) < 0)
    {
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
