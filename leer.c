// Félix Aguilar, Adrián Bennasar, Álvaro Bueno
#include <stdlib.h>
#include "fichero.h"

/* Fichero: leer.c:
* -----------------
* Lee el contenido de un inodo y permite mostrarlo por pantalla o bien 
* redirigirlo a un fichero.
*
* returns: Exit_Success o si se produce un error Exit_Failure.
*/
int main(int argc, char const *argv[])
{
    // Comprobación de la sintaxis de los argumentos.
    if (argc != 3)
    {
        fprintf(stderr, "Sintaxis: leer <nombre_dispositivo><numero_inodo>\n");
        return EXIT_FAILURE;
    }

    // Tratamiento de los valores de los parametros.
    int ninodo = atoi(argv[2]);

    // Monta el dispositivo en el sistema.
    if (bmount(argv[1]) == -1)
    {
        fprintf(stderr, "Error al montar el dispositivo en el sistema.\n");
        return EXIT_FAILURE;
    }

    // Lee el superbloque del dispositivo.
    struct superbloque SB;
    if (bread(0, &SB) == -1)
    {
        fprintf(stderr, "Error de lectura del superbloque.\n");
        return EXIT_FAILURE;
    }

    // Inicializa las variables utilizadas para la lectura.
    int offset = 0;
    int tamBuffer = 1500;
    int bytes = 0;

    // Genera un buffer de tamaño tamBuffer y lo inicializa a 0.
    char buffer[tamBuffer];
    memset(buffer, 0, tamBuffer);

    // Lee del fichero hasta llenar el buffer o bien llegar a fin de fichero.
    int leidos = mi_read_f(ninodo, buffer, offset, tamBuffer);

    // Mientras lea contenido del archivo realizamos el bucle.
    while (leidos > 0)
    {
        // Actualiza el número de bytes leidos.
        bytes = bytes + leidos;

        // Escribe el contenido del buffer en el destino indicado.
        write(1, buffer, leidos);

        // Limpia el buffer de lectura, actualiza el offset y vuelve a leer.
        memset(buffer, 0, tamBuffer);
        offset = offset + tamBuffer;
        leidos = mi_read_f(ninodo, buffer, offset, tamBuffer);
    }

    // Lee el inodo en el cual se encontraba el archivo.
    struct inodo inodo;
    if (leer_inodo(ninodo, &inodo))
    {
        fprintf(stderr, "Error con la lectura del inodo.\n");
        return EXIT_FAILURE;
    }

    // Muestra el número de bytes leídos y el tamaño en bytes lógicos.
    fprintf(stderr, "total_leidos: %d\ntamEnBytesLog: %d\n", bytes,
            inodo.tamEnBytesLog);

    // Desmonta el dispositivo del sistema y finaliza.
    bumount();
    return EXIT_SUCCESS;
}