#include <stdlib.h>
#include "fichero.h"

int main(int argc, char const *argv[])
{
    if (argc < 3)
    {
        printf("Sintaxis: permitir <nombre_dispositivo><numero_inodo><permisos>\n");
        return 0;
    }

    int ninodo = atoi(argv[2]);
    char permisos = (*argv[3]);

    bmount(argv[1]);
    struct superbloque SB;
    bread(0, &SB);

    if(mi_chmod_f(ninodo, permisos))
    {
        perror("Error");
        return -1;
    }
    
    
    bumount();
    return 0;
}