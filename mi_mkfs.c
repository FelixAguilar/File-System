#include "ficheros_basico.h"

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
    if (argc < 3)
    {
        // mostrar error.
        return EXIT_FAILURE;
    }

    unsigned char buf[BLOCKSIZE];


    char* nombre = argv[1];
    int nBloques = atoi(argv[2]);
    int ninodos = nBloques/4;

    memset(buf, 0, BLOCKSIZE);

    bmount(nombre);
    
    // inicialización de metadatos
    initSB(nBloques, ninodos);
    initMB();
    initAI();

    for (int i = 0; i < nBloques; i++)
    {
        bwrite(i, buf);
    }
    bumount();

    return EXIT_SUCCESS;
}
