// Autores: Félix Aguilar, Adrián Bennasar, Álvaro Bueno
#include <stdlib.h>
#include "fichero.h"

/* Fichero: permitir.c:
* ---------------------
* Cambia los permisos del inodo indicado por parámetro por los permisos también
* indicados por parámetro.
*
*  argc: número de parámetros introducidos en la ejecución.
*  argv: array de strings con los parámetros indicados por consola.
*
* return Exit_Success o si se ha producido un error Exit_Failure.
*/
int main(int argc, char const *argv[])
{
    // Revisa que la sintaxis sea la correcta.
    if (argc < 3)
    {
        fprintf(stderr, "Sintaxis: permitir <nombre_dispositivo><numero_inodo>"
                        "<permisos>\n");
        return EXIT_FAILURE;
    }

    // Obtiene los valores de los parámetros.
    int ninodo = atoi(argv[2]);
    char permisos = (*argv[3]);

    // Monta el dispositivo en el sistema.
    if (bmount(argv[1]) == -1)
    {
        fprintf(stderr, "Error montando el dispositivo en el sistema.\n");
        return EXIT_FAILURE;
    }

    // Obtiene el superbloque del dispositivo.
    struct superbloque SB;
    if (bread(0, &SB) == -1)
    {
        fprintf(stderr, "Error con la lectura del superbloque.\n");
        return EXIT_FAILURE;
    }

    // Cambia los permisos del inodo introducido por parámetro.
    if (mi_chmod_f(ninodo, permisos))
    {
        fprintf(stderr, "Error con la modificacion de los permisos.\n");
        return EXIT_FAILURE;
    }

    // Desmonta el dispositivo del sistema.
    bumount();
    return EXIT_SUCCESS;
}