// Autores: Félix Aguilar, Adrián Bennasar, Álvaro Bueno
#include "directorios.h"
/*
* Fichero: mi_mkfs.c:
* -------------------
* Crea e inicializa el dispositivo virtual de almacenamiento con el nombre y 
* tamaño indicado por parámetro.
*
*  argc: número de argumentos introducidos.
*  argv: char array de los argumentos introducidos.
*
* returns: Exit_Success o si se ha producido un error Exit_Failure.
*/
int main(int argc, char **argv)
{
    // Comprueba que la sintaxis sea la correcta.
    if (argc != 3)
    {
        fprintf(stderr, "Error de sintaxis: ./mi_mkfs <nombre_dispositivo>"
                        "<tamaño_en_bloques>\n");
        return EXIT_FAILURE;
    }
    // Obtiene los valores de los parámetros.
    char *nombre = argv[1];
    int nBloques = atoi(argv[2]);
    int ninodos = nBloques / 4;

    // Generación de buffer de ceros tamaño bloque.
    unsigned char buf[BLOCKSIZE];
    memset(buf, 0, BLOCKSIZE);

    // Monta el dispositivo virtual en el sistema.
    if (bmount(nombre) == -1)
    {
        fprintf(stderr, "Error de montaje de dispositivo virtual.\n");
        return EXIT_FAILURE;
    }
    // Limpia el espacio ocupado por el dispositivo virtual con el buffer.
    for (int i = 0; i < nBloques; i++)
    {
        if (bwrite(i, buf) == -1)
        {
            fprintf(stderr, "Error de escritura en el dispositivo.\n");
            return EXIT_FAILURE;
        }
    }
    // Inicialización de metadatos.
    if (initSB(nBloques, ninodos))
    {
        fprintf(stderr, "Error generando el superbloque en el dispositivo\n");
        return EXIT_FAILURE;
    }
    if (initMB())
    {
        fprintf(stderr, "Error en la generación del mapa de bits del "
                        "dispositivo.\n");
        return EXIT_FAILURE;
    }
    if (initAI())
    {
        fprintf(stderr, "Error en la generación el array de inodos del "
                        "dispositivo.\n");
        return EXIT_FAILURE;
    }
    // Reserva el inodo para el directorio raíz.
    if (reservar_inodo('d', 7) == -1)
    {
        fprintf(stderr, "Error en la reserva del inodo raíz.\n");
        return EXIT_FAILURE;
    }
    // Desmonta el dispositivo virtual.
    bumount();
    return EXIT_SUCCESS;
}
