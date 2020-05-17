// Autores: Félix Aguilar, Adrián Bennasar, Álvaro Bueno.
#include "directorios.h"

/* Fichero: mi_touch.c:
* --------------------
* Este fichero permite crear un archivo.
*
*  argc: número de argumentos introducidos.
*  argv: char array de los argumentos introducidos.
*
*  returns: Exit_Success o bien Exit_Failure si se produce un error.
*/
int main(int argc, char const *argv[])
{
    // Comprueba que la sintaxis sea correcta.
    if (argc != 4)
    {
        fprintf(stderr,
                "Error de sintaxis: ./mi_mkdir <disco> <permisos> </ruta>\n");
        return EXIT_FAILURE;
    }
    if (atoi(argv[2]) > 7)
    {
        fprintf(stderr, "Error: modo inválido: <<%s>>\n", argv[2]);
        return EXIT_FAILURE;
    }
    unsigned char permisos = atoi(argv[2]);
    if ((argv[3][strlen(argv[3]) - 1] != '/'))
    {
        // Monta el disco en el sistema.
        if (bmount(argv[1]) == -1)
        {
            fprintf(stderr, "Error de montaje de disco.\n");
            return EXIT_FAILURE;
        }
        int error;
        if ((error = mi_creat(argv[3], permisos)) < 0)
        {
            mostrar_error_directorios(error);
            return EXIT_FAILURE;
        }
        bumount();
    }
    return EXIT_SUCCESS;
}