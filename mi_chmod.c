// Autores: Félix Aguilar, Adrián Bennasar, Álvaro Bueno.
#include "directorios.h"

/* Fichero: mi_chmod.c:
* ---------------------
* Este archivo permite cambiar los permisos de un fichero o directorio.
*
*  argc: número de argumentos introducidos.
*  argv: char array de los argumentos introducidos.
*
*  returns: Exit_Success o bien Exit_Failure si se ha producido un error.
*/
int main(int argc, char const *argv[])
{
    // Comprueba que la sintaxis sea correcta.
    if (argc != 4)
    {
        fprintf(stderr,
                "Error de sintaxis: ./mi_chmod <disco> <permisos> </ruta>\n");
        return EXIT_FAILURE;
    }
    // Comprueba que los permisos a utilizar estén en el rango adecuado.
    unsigned char permisos = atoi(argv[2]);
    if (permisos > 7)
    {
        fprintf(stderr, "Error: permisos fuera de rango.");
        return EXIT_FAILURE;
    }
    // Monta el disco en el sistema.
    if (bmount(argv[1]) == -1)
    {
        fprintf(stderr, "Error: no se pudo montar el disco.\n");
        return EXIT_FAILURE;
    }
    // Realiza el cambio de permisos.
    int error;
    if ((error = mi_chmod(argv[3], permisos)) < 0)
    {
        mostrar_error_directorios(error);
    }
    bumount();
    return EXIT_SUCCESS;
}