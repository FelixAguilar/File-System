#include "bloques.h"

static int descriptor; // Descriptor del archivo usado como disco virtual.


/* Funcion: bmount
* ---------------
* Esta función obtiene el descriptor del fichero pasado por parámetro con la 
* llamada al sistema open() y lo devuelve si no ha habido error. En caso de que
* se produzca un error sale con EXIT_FAILURE. Si no existe el fichero lo crea
* y si ya existe lo abre en modo lectura/escritura.
* 
* descriptor: descriptor del fichero apuntado por *camino
*
* returns: descriptor.
*/

int bmount (const char *camino){
    descriptor = open(camino, O_RDWR|O_CREAT, 0666);
    if(descriptor == -1){
        return EXIT_FAILURE;
    } else {
        return descriptor;
    }
}

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

/* Funcion: bwrite
* ---------------

* Esta función permite escribir el contenido de buf en un fichero, concretamen
* te en el bloque especificado por el parámetro nbloque. En el caso de que no
* haya error, la función devuelve el número de bytes que se han podido escribir.
* Si hay error el programa sale con EXIT_FAILURE.
* 
* 
*
* returns: writtenBytes
*/

int bwrite(unsigned int nbloque, const void *buf){
    size_t writtenBytes;
    // posicionamos el cursor en el bloque donde queremos escribir.
    lseek(descriptor, nbloque * BLOCKSIZE, SEEK_SET);
    writtenBytes = write(descriptor, buf, BLOCKSIZE);
    if(writtenBytes < 0){
        return EXIT_FAILURE;
    }else{
        return writtenBytes;
    }
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
    if (close(descriptor) != -1)
    {
        return EXIT_SUCCESS;
    }
    else
    {
        // Si no, devuelve el error que ha ocurrido.
        fprintf(stderr, "%s", strerror(errno));
    }
    return EXIT_FAILURE;
}
