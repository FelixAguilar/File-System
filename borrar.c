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

    struct inodo inodo;

    printf("%d", liberar_inodo(ninodo));

    leer_inodo(ninodo, &inodo);

    printf("DATOS DEL DIRECTORIO RAIZ\n");

    printf("Tipo: %c\n", inodo.tipo);
    printf("Permisos: %d\n", inodo.permisos);
    struct tm *ts;
    char atime[80];
    char mtime[80];
    char ctime[80];
    ts = localtime(&inodo.atime);
    strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&inodo.mtime);
    strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&inodo.ctime);
    strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S", ts);
    printf("ATIME: %s MTIME: %s CTIME: %s\n", atime, mtime, ctime);
    printf("nLinks = %d\n", inodo.nlinks);
    printf("tamEnBytesLog = %d\n", inodo.tamEnBytesLog);
    printf("numBloquesOcupados = %d\n", inodo.numBloquesOcupados);

    // printf("bytes: %d, tamEnBytesLog: %d\n", bytes, inodo.tamEnBytesLog);
    bumount();
    return 0;
}