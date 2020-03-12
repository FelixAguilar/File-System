#include <stdlib.h>
#include "ficheros_basico.h"

int main(int argc, char const *argv[])
{
       bmount(argv[1]);
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

       /*for (int i = SB.posPrimerBloqueAI; i <= SB.posUltimoBloqueAI; i++){
        struct inodo inodos [8];
        int j = 0;
        bread(i, inodos);
        while (j < 8){
            print("%d,", inodos[j].punterosDirectos[0]);
        }
    }*/

       printf("RESERVAMOS UN BLOQUE Y LUEGO LO LIBERAMOS\n");
       int bloque = reservar_bloque();
       bread(0, &SB);
       printf("Se ha reservado el bloque físico nº %d que era el 1r bloque libre indicado por el MB\n", bloque);
       printf("SB.cantBloquesLibres = %d\n", SB.cantBloquesLibres);
       liberar_bloque(bloque);
       bread(0, &SB);
       printf("Liberamos ese bloque y después SB.cantBloquesLibres = %d\n", SB.cantBloquesLibres);

       char ret;
       printf("MAPA DE bitS CON BLOQUES DE METADATOS OCUPADOS\n");
       ret = leer_bit(SBPOS);
       printf("[leer_bit()→ nbloque: %d, posbyte:%d, posbit:%d, nbloqueMB:%d, nbloqueabs:%d) valor del ret correspondiente a posSB (o sea al BF nº %d)  = %d\n",
              SBPOS, SBPOS / 8, SBPOS % 8, SBPOS / BLOCKSIZE, SB.posPrimerBloqueMB + SBPOS / BLOCKSIZE, SBPOS, ret);
       ret = leer_bit(SB.posPrimerBloqueMB);
       printf("[leer_bit()→ nbloque: %d, posbyte:%d, posbit:%d, nbloqueMB:%d, nbloqueabs:%d) valor del ret correspondiente a SB.posPrimerBloqueMB (o sea al BF nº %d)  = %d\n",
              SB.posPrimerBloqueMB, SB.posPrimerBloqueMB / 8, SB.posPrimerBloqueMB % 8, (SB.posPrimerBloqueMB / BLOCKSIZE) / 8, SB.posPrimerBloqueMB + (SB.posPrimerBloqueMB / BLOCKSIZE) / 8, SB.posPrimerBloqueMB, ret);
       ret = leer_bit(SB.posUltimoBloqueMB);
       printf("[leer_bit()→ nbloque: %d, posbyte:%d, posbit:%d, nbloqueMB:%d, nbloqueabs:%d) valor del ret correspondiente a SB.posUltimoBloqueMB (o sea al BF nº %d)  = %d\n",
              SB.posUltimoBloqueMB, SB.posUltimoBloqueMB / 8, SB.posUltimoBloqueMB % 8, (SB.posUltimoBloqueMB / BLOCKSIZE) / 8, SB.posPrimerBloqueMB + (SB.posUltimoBloqueMB / BLOCKSIZE) / 8, SB.posUltimoBloqueMB, ret);
       ret = leer_bit(SB.posPrimerBloqueAI);
       printf("[leer_bit()→ nbloque: %d, posbyte:%d, posbit:%d, nbloqueMB:%d, nbloqueabs:%d) valor del ret correspondiente a SB.posPrimerBloqueAI (o sea al BF nº %d)  = %d\n",
              SB.posPrimerBloqueAI, SB.posPrimerBloqueAI / 8, SB.posPrimerBloqueAI % 8, (SB.posPrimerBloqueAI / BLOCKSIZE) / 8, SB.posPrimerBloqueMB + (SB.posPrimerBloqueAI / BLOCKSIZE) / 8, SB.posPrimerBloqueAI, ret);
       ret = leer_bit(SB.posUltimoBloqueAI);
       printf("[leer_bit()→ nbloque: %d, posbyte:%d, posbit:%d, nbloqueMB:%d, nbloqueabs:%d) valor del ret correspondiente a SB.posUltimoBloqueAI (o sea al BF nº %d)  = %d\n",
              SB.posUltimoBloqueAI, SB.posUltimoBloqueAI / 8, SB.posUltimoBloqueAI % 8, (SB.posUltimoBloqueAI / BLOCKSIZE) / 8, SB.posPrimerBloqueMB + (SB.posUltimoBloqueAI / BLOCKSIZE) / 8, SB.posUltimoBloqueAI, ret);
       ret = leer_bit(SB.posPrimerBloqueDatos);
       printf("[leer_bit()→ nbloque: %d, posbyte:%d, posbit:%d, nbloqueMB:%d, nbloqueabs:%d) valor del ret correspondiente a SB.posPrimerBloqueDatos (o sea al BF nº %d)  = %d\n",
              SB.posPrimerBloqueDatos, SB.posPrimerBloqueDatos / 8, SB.posPrimerBloqueDatos % 8, (SB.posPrimerBloqueDatos / BLOCKSIZE) / 8, SB.posPrimerBloqueMB + (SB.posPrimerBloqueDatos / BLOCKSIZE) / 8, SB.posPrimerBloqueDatos, ret);
       ret = leer_bit(SB.posUltimoBloqueDatos);
       printf("[leer_bit()→ nbloque: %d, posbyte:%d, posbit:%d, nbloqueMB:%d, nbloqueabs:%d) valor del ret correspondiente a SB.posUltimoBloqueDatos (o sea al BF nº %d)  = %d\n",
              SB.posUltimoBloqueDatos, SB.posUltimoBloqueDatos / 8, SB.posUltimoBloqueDatos % 8, (SB.posUltimoBloqueDatos / BLOCKSIZE) / 8, SB.posPrimerBloqueMB + (SB.posUltimoBloqueDatos / BLOCKSIZE) / 8, SB.posUltimoBloqueDatos, ret);

       bumount();
       return 0;
}
