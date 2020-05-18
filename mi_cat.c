// Félix Aguilar, Adrián Bennasar, Álvaro Bueno
#include "directorios.h"

/* Fichero: mi_cat.c
* ------------------
* Lee el contenido de un fichero y lo vuelca en un archivo o en pantalla. 
*
*  argc: número de argumentos introducidos.
*  argv: char array de los argumentos introducidos.
*
*  returns: Exit_Success o si se produce un error Exit_Failure.
*/
int main(int argc, char const *argv[])
{
    // Comprobación de la sintaxis de los argumentos.
    if (argc != 3)
    {
        fprintf(stderr, "Sintaxis: mi_cat <disco> </ruta_fichero>\n");
        return EXIT_FAILURE;
    }
    // Monta el dispositivo en el sistema.
    if (bmount(argv[1]) == -1)
    {
        fprintf(stderr, "Error al montar el dispositivo en el sistema.\n");
        return EXIT_FAILURE;
    }
    // Inicializa las variables utilizadas para la lectura.
    int offset = 0;
    int tamBuffer = 4096;
    int bytes = 0;

    // Genera un buffer de tamaño tamBuffer y lo inicializa a 0.
    char buffer[tamBuffer];
    memset(buffer, 0, tamBuffer);

    // Lee del fichero hasta llenar el buffer o bien llegar a fin de fichero.
    int leidos = mi_read(argv[2], buffer, offset, tamBuffer);

    // Mientras lea contenido del archivo realizamos el bucle.
    while (leidos > 0)
    {
        // Actualiza el número de bytes leídos.
        bytes = bytes + leidos;

        // Escribe el contenido del buffer en el destino indicado.
        write(1, buffer, leidos);

        // Limpia el buffer de lectura, actualiza el offset y vuelve a leer.
        memset(buffer, 0, tamBuffer);
        offset = offset + tamBuffer;
        leidos = mi_read(argv[2], buffer, offset, tamBuffer);
    }
    if(leidos < 0)
    {
        mostrar_error_directorios(leidos);
    }
    // Muestra el número de bytes leídos.
    fprintf(stderr, "\n\ntotal_leidos: %d\n", bytes);

    // Desmonta el dispositivo del sistema y finaliza.
    bumount();
    return EXIT_SUCCESS;
}