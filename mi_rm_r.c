// Autores: Félix Aguilar, Adrián Bennasar, Álvaro Bueno
#include "directorios.h"

/*
* Fichero: mi_rm_r.c:
* --------------------
* Elimina recursivamente el contenido de un directorio.
*
*  argc: número de argumentos introducidos.
*  argv: char array de los argumentos introducidos.
*
*  returns: Exit_Success o si se ha producido un error Exit_Failure.
*/
int main(int argc, char **argv)
{
    // Comprueba que la sintaxis sea la correcta.
    if (argc != 3)
    {
        fprintf(stderr, "Sintaxis: ./mi_rm_r <disco> </ruta_directorio>\n");
        return EXIT_FAILURE;
    }
    // Comprueba que es un directorio y no es el directorio raiz.
    if ((argv[2][strlen(argv[2]) - 1] != '/'))
    {
        fprintf(stderr, "Error: La ruta proporcionada no es un directorio.\n");
        return EXIT_FAILURE;
    }
    // No permite eliminar el directorio raíz.
    if (strlen(argv[2]) == 1)
    {
        fprintf(stderr, "Error: La ruta proporcionada es el directorio raiz.\n");
        return EXIT_FAILURE;
    }
    // Monta el dispositivo virtual en el sistema.
    if (bmount(argv[1]) == -1)
    {
        fprintf(stderr, "Error: no se pudo montar el dispositivo.\n");
        return EXIT_FAILURE;
    }
    // Procede a borrar recursivamente los dispositivos.
    int error;
    if ((error = mi_unlink_r(argv[2])) < 0)
    {
        mostrar_error_directorios(error);
        return EXIT_FAILURE;
    }
    // Desmonta el dispositivo virtual.
    bumount();
    return EXIT_SUCCESS;
}