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
    char buffer[BLOCKSIZE];
    memset(buffer, 0, BLOCKSIZE);
    int bytes = 0;
    int i = mi_read_f(ninodo, buffer, offset, BLOCKSIZE);
    while (i > 0)
    {
        bytes = bytes + i;
        write(1, buffer, BLOCKSIZE);
        memset(buffer, 0, BLOCKSIZE);
        offset = offset + BLOCKSIZE;
        i = mi_read_f(ninodo, buffer, offset, BLOCKSIZE);
    }

    struct inodo inodo;
    leer_inodo(ninodo, &inodo);

    char aux[20];
    //sprintf(aux, "%d", bytes);
    //write(2, aux , sizeof(int));
    sprintf(aux, "%d", inodo.tamEnBytesLog);
    write(2, aux, sizeof(int));
    bumount();
    return 0;
}