#include "bloques.h"

/*
* Function:  main
* --------------------
* Libera el descriptor de ficheros con la función close() 
*
*  argc: número de argumentos introducidos.
*  argv: char array de los argumentos introducidos.
*  returns: 
*/
int main(int argc, char **argv)
{
    unsigned char buf[BLOCKSIZE];
    char nombre[] = argv[1];
    int nBloques = atoi(argv[2]);

    memset(buf, 0, BLOCKSIZE);

    bmount(nombre);
    for (int i; i < nBloques; i++)
    {
        bread(i, buf);
    }
    bumount();

    return 0;
}