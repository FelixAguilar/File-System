// Autores: Félix Aguilar, Adrián Bennasar, Álvaro Bueno
#include "directorios.h"
/*
* Fichero: mi_rm.c:
* -----------------
* Elimina el fichero indicado por parametro.
*
* returns: Exit_Success o si se ha producido un error Exit_Failure.
*/
int main(int argc, const char *argv[])
{
    // Comprueba que la sintaxis sea la correcta.
    if (argc != 3)
    {
        fprintf(stderr, "Sintaxis: ./mi_rm <disco> </ruta_fichero>\n");
        return EXIT_FAILURE;
    }
    // Comprueba que es un fichero.
    if ((argv[2][strlen(argv[2]) - 1] == '/'))
    {
        fprintf(stderr, "Error: La ruta prop222orcionada no es un fichero.\n");
        return EXIT_FAILURE;
    }
    // Monta el dispositivo virtual en el sistema.
    if (bmount(argv[1]) == -1)
    {
        fprintf(stderr, "Error: no se pudo montar el dispositivo.\n");
        return EXIT_FAILURE;
    }
    // ELimina el fichero del sistema.
    int error;
    if ((error = mi_unlink(argv[2])) < 0)
    {
        mostrar_error_directorios(error);
        return EXIT_FAILURE;
    }

    // Desmonta el dispositivo virtual.
    bumount();
    return EXIT_SUCCESS;
}