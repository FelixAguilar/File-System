// Autores: Félix Aguilar, Adrián Bennasar, Álvaro Bueno.
#include "directorios.h"

int main(int argc, char const *argv[])
{
    // Comprueba que la sintaxis sea correcta.
    if (argc != 4)
    {
        fprintf(stderr,
                "Error de sintaxis: ./mi_chmod <disco><permisos></ruta>\n");
        return EXIT_FAILURE;
    }

    if (atoi(argv[2]) > 7)
    {
        // error
    }

    unsigned char permisos = atoi(argv[2]);

    // Monta el disco en el sistema.
    if (bmount(argv[1]) == -1)
    {
        fprintf(stderr, "Error de montaje de disco.\n");
        return EXIT_FAILURE;
    }


    mi_chmod(argv[3], permisos);

    bumount();

}