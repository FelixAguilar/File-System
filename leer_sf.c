#include <stdlib.h>
#include "ficheros_basico.h"

int main(int argc, char const *argv[])
{
    int fd = bmount(argv[1]);
    struct superbloque SB;
    bread(0, &SB);

    printf("DATOS DEL SUPERBLOQUE\n");
    printf("posPrimerBloqueMB = %d\n", SB.posPrimerBloqueMB);
    printf("posUltimoBloqueMB = %d\n", SB.posUltimoBloqueMB);
    printf("posPrimerBloqueAI = %d\n", SB.posPrimerBloqueAI);
    printf("posPrimerBloqueDatos = %d\n", SB.posPrimerBloqueDatos);
    printf("posUltimoBloqueDatos = %d\n", SB.posUltimoBloqueDatos);
    printf("posInodoRaiz = %d\n", SB.posInodoRaiz);
    printf("posPrimerInodoLibre = %d\n", SB.posPrimerInodoLibre);
    printf("cantBloquesLibres = %d\n", SB.cantBloquesLibres);
    printf("cantInodosLibres = %d\n", SB.cantInodosLibres);
    printf("totBloques = %d\n", SB.totBloques);
    printf("totInodos = %d\n", SB.totInodos);
    printf("sizeof struct superbloque is: %lu\n", sizeof(struct superbloque));
    printf("sizeof struct inodo is: %lu\n", sizeof(struct inodo));
    printf("RECORRIDO LISTA ENLAZADA DE INODOS LIBRES\n");
    for (int i = SB.posPrimerBloqueAI; i <= SB.posUltimoBloqueAI; i++){
        struct inodo inodos [8];
        int j = 0;
        bread(i, inodos);
        while (j < 8){
            printf("%d,", inodos[j].punterosDirectos[0]);
            j++;
        }
    }
    return 0;
}