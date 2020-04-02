#include <stdlib.h>
#include "fichero.h"

int main(int argc, char const *argv[])
{
    if (argc < 3)
    {
        printf("Sintaxis: truncar <nombre_dispositivo><numero_inodo><n_bytes>\n");
        return 0;
    }

    int ninodo = atoi(argv[2]);
    int n_bytes = atoi(argv[3]);

    bmount(argv[1]);
    struct superbloque SB;
    bread(0, &SB);

    if (!n_bytes)
    {
        liberar_inodo(ninodo);
    }
    else
    {
        mi_truncar_f(ninodo, n_bytes);
    }

    struct STAT mi_stat;

    mi_stat_f(ninodo, &mi_stat);
    printf("tamEnBytesLog: %d, numBloquesOcupados: %d", mi_stat.tamEnBytesLog, mi_stat.numBloquesOcupados);

    // printf("bytes: %d, tamEnBytesLog: %d\n", bytes, inodo.tamEnBytesLog);
    bumount();
    return 0;
}