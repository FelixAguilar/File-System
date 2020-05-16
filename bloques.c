// Autores: Félix Aguilar, Adrián Bennasar, Álvaro Bueno
#include "bloques.h"

static int descriptor; // Descriptor del archivo utilizado como disco virtual.

/* Función: bmount:
* -----------------
* Esta función obtiene el descriptor del fichero pasado por parámetro con la 
* llamada al sistema open(). Si no existe el fichero lo crea y si ya existe lo 
* abre en modo lectura/escritura.
* 
*  camino: dirección del fichero del dispositivo virtual.
*
* returns: descriptor o -1 si se produce un error al intentar abrir el fichero.
*/
int bmount(const char *camino)
{
    descriptor = open(camino, O_RDWR | O_CREAT, 0666);
    return descriptor;
}

/* Función: bread:
 * ---------------
 * Lee del dispositivo virtual el bloque especificado por nbloque y copia su 
 * contenido en el buffer apuntado por *buf.
 * 
 *  nbloque: posición del bloque que se quiere leer.
 *  buf: dirección de memoria del buffer para guardar el valor.
 * 
 * returns: número de bytes leidos o bien -1 si se produce un error.
*/
int bread(unsigned int nbloque, void *buf)
{
    // Revisa que el descriptor exista y prueba de reubicar el puntero en este.
    if (descriptor)
    {
        if (lseek(descriptor, nbloque * BLOCKSIZE, SEEK_SET) != -1)
        {

            // Si ha sido posible, lee el contenido a partir de este bloque.
            size_t bytes = read(descriptor, buf, BLOCKSIZE);
            if (bytes != -1)
            {

                // Si no hubo errores, devuelve el número de bytes leídos.
                return bytes;
            }
        }    
    }
    return -1;
}

/* Función: bwrite:
* -----------------
* Esta función permite escribir el contenido de buf en un fichero, en concreto
* en el bloque especificado por el parámetro nbloque.
* 
*  nboque: bloque lógico en el que se quiere escribir.
*  buf: puntero al contenido que se quiere escribir en nbloque.
*
* returns: bytes o -1 si ha ocurrido un error.
*/
int bwrite(unsigned int nbloque, const void *buf)
{
    size_t bytes;

    // Posicionamos el cursor en el bloque donde queremos escribir.
    if (lseek(descriptor, nbloque * BLOCKSIZE, SEEK_SET) != -1)
    {
        bytes = write(descriptor, buf, BLOCKSIZE);
        if (bytes < 0)
        {
            return -1;
        }
        else
        {
            return bytes;
        }
    }
    else
    {
        return -1;
    }
}

/*
* Function: bumount:
* ------------------
* Libera el descriptor de ficheros con la función close() 
*
*  returns: devuelve EXIT_SUCCESS si se ha cerrado el fichero correctamente y 
*           EXIT_FAILURE si ha habido algún error.
*/
int bumount()
{
    if (close(descriptor) != -1)
    {
        return EXIT_SUCCESS;
    }

    // Si no, devuelve el error que ha ocurrido.
    return EXIT_FAILURE;
}