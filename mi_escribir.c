// Autores: Félix Aguilar, Adrián Bennasar, Álvaro Bueno
#include "directorios.h"

/* Fichero: mi_escribir.c:
* ------------------------
* Escribe el texto pasado por parámetro dentro del archivo indicado por la ruta
* dentro del disco.
*
*  argc: número de argumentos del comando.
*  argv: array de strings con los argumentos introducidos junto al comando.
*
* returns: Exit_Success, o Exit_Failure si se ha producido un error.
*/
int main(int argc, char const *argv[])
{
    // Comprueba la sintaxis, si es errónea muestra el formato aceptado.
    if (argc != 5)
    {
        fprintf(stderr, "Sintaxis: mi_escribir <disco> </ruta_fichero> <texto>"
                        " <offset>\n");
        return EXIT_FAILURE;
    }
    // Monta el disco virtual en el sistema.
    if (bmount(argv[1]) == -1)
    {
        fprintf(stderr, "Error al montar el disco virtual.\n");
        return EXIT_FAILURE;
    }
    printf("Longitud texto: %ld\n", strlen(argv[3]));

    // Escribe el buffer en el archivo.
    int aux = atoi(argv[4]);
    int bytes_escritos = mi_write(argv[2], argv[3], aux, strlen(argv[3]));
    if (bytes_escritos < 0)
    {
        // Muestra si ha habido un error.
        mostrar_error_directorios(bytes_escritos);
        return EXIT_FAILURE;
    }
    else
    {
        printf("Bytes escritos: %d\n", bytes_escritos);
    }
    // Desmonta el disco virtual del sistema.
    bumount();
    return EXIT_SUCCESS;
}