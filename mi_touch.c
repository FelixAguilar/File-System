#include "directorios.h"

int main(int argc, char const *argv[])
{
    // Comprueba que la sintaxis sea correcta.
    if (argc != 4)
    {
        fprintf(stderr,
                "Error de sintaxis: ./mi_mkdir <disco><permisos></ruta>\n");
        return EXIT_FAILURE;
    }

    if (atoi(argv[2]) > 7)
    {
        // error
    }

    if ((strcmp(argv[3][strlen(argv[3] - 1)], "/")))
    {
        // Monta el disco en el sistema.
        if (bmount(argv[1]) == -1)
        {
            fprintf(stderr, "Error de montaje de disco.\n");
            return EXIT_FAILURE;
        }
        mi_creat(argv[3], argv[2]);
        bumount();
    }
}