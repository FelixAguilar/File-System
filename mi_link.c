// Félix Aguilar, Adrián Bennasar, Álvaro Bueno
#include "directorios.h"

/* Fichero: mi_link.c:
* --------------------
* Permite la creacion de enlaces a archivos desde otras ubicaciones del 
* dispositivo.
*
* returns: Exit_Success o si se produce un error Exit_Failure.
*/
int main(int argc, char const *argv[])
{
    // Comprobación de la sintaxis de los argumentos.
    if (argc != 4)
    {
        fprintf(stderr, "Sintaxis: mi_link <disco> </ruta_fichero_original> </"
                        "ruta_enlace>\n");
        return EXIT_FAILURE;
    }
    //Comprueba que los caminos proporcionados son fichero.
    if (argv[2][strlen(argv[2]) - 1] == '/' || argv[3][strlen(argv[3]) - 1] ==
                                                   '/')
    {
        fprintf(stderr, "Error: Al menos una de las rutas no es un fichero.\n");
        return EXIT_FAILURE;
    }
    // Monta el dispositivo en el sistema.
    if (bmount(argv[1]) == -1)
    {
        fprintf(stderr, "Error: no se pudo montar el dispositivo.\n");
        return EXIT_FAILURE;
    }
    // Crea el link del archivo en el nuevo camino.
    int error;
    if ((error = mi_link(argv[2], argv[3])) < 0)
    {
        mostrar_error_directorios(error);
        return EXIT_FAILURE;
    }
    // Desmonta el dispositivo del sistema y finaliza.
    bumount();
    return EXIT_SUCCESS;
}