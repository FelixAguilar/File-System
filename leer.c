#include <stdlib.h>
#include "fichero.h"

int main(int argc, char const *argv[])
{
    if (argc < 2)
    {
        printf("Sintaxis: leer <nombre_dispositivo><numero_inodo>\n");
        return 0;
    }

    int ninodo = atoi(argv[2]);

    bmount(argv[1]);
    struct superbloque SB;
    bread(0, &SB);

    int offset = 0;
    int tamBuffer = 1024;
    char buffer[tamBuffer];
    memset(buffer, 0, tamBuffer);
    int bytes = 0;

    printf("inodo: %d\n", ninodo);
    
    int leidos = mi_read_f(ninodo, buffer, offset, tamBuffer);
    
    
    while (leidos > 0)
    {
        printf("i: %d", leidos);
        bytes = bytes + leidos;
       // write(1, buffer, leidos);
        memset(buffer, 0, tamBuffer);
        offset = offset + tamBuffer;
        leidos = mi_read_f(ninodo, buffer, offset, tamBuffer);
    }

    struct inodo inodo;
    leer_inodo(ninodo, &inodo);

    /* char aux[200];
    sprintf(aux, "%d", bytes);
    write(2, aux, sizeof(int));
    sprintf(aux, "%d", inodo.tamEnBytesLog);
    write(2, aux, sizeof(int));
   */

    // printf("bytes: %d, tamEnBytesLog: %d\n", bytes, inodo.tamEnBytesLog);
    bumount();
    return 0;
}