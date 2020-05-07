// Autores: Félix Aguilar, Adrián Bennasar, Álvaro Bueno.
#include "directorios.h"

int main(int argc, char const *argv[])
{
    // Comprueba que la sintaxis sea correcta.
    if (argc != 3)
    {
        fprintf(stderr,
                "Error de sintaxis: ./mi_ls <disco></ruta_directorio>\n");
        return EXIT_FAILURE;
    }

    // Monta el disco en el sistema.
    if (bmount(argv[1]) == -1)
    {
        fprintf(stderr, "Error de montaje de disco.\n");
        return EXIT_FAILURE;
    }

    char buffer[TAMBUFFER];
    memset(buffer, 0 , TAMBUFFER);
    int total = mi_dir(argv[2], buffer);

    if (total > 0)
    {
        printf("Total: %d\n", total);
        printf("Tipo\tPermisos\tmTime\t\t\tTamaño\tNombre\n");
        printf("----------------------------------------------------------"
               "----------------------------------\n");
        char *loc = strchr(buffer, '|');
        while(loc)
        {
            *(loc) = '\n';
            loc = strchr(loc + 1, '|');
        }
        printf("%s\n", buffer);
        
    }

    bumount();
}