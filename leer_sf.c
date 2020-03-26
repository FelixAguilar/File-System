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
       /*printf("RECORRIDO LISTA ENLAZADA DE INODOS LIBRES\n");

       for (int i = SB.posPrimerInodoLibre; i <= SB.cantInodosLibres; i++)
       {
              struct inodo inodo;
              leer_inodo(i, &inodo);
              printf("%d,", inodo.punterosDirectos[0]);
       }

       char ret;
       ret = leer_bit(SBPOS);
       printf("RESERVAMOS UN BLOQUE Y LUEGO LO LIBERAMOS\n");
       int bloque = reservar_bloque();
       bread(0, &SB);
       printf("Se ha reservado el bloque físico nº %d que era el 1r bloque libre indicado por el MB\n", bloque);
       printf("SB.cantBloquesLibres = %d\n", SB.cantBloquesLibres);
       printf("[leer_bit()→ nbloque: %d, posbyte:%d, posbit:%d, nbloqueMB:%d, nbloqueabs:%d) valor del ret correspondiente a SB.posPrimerBloqueDatos (o sea al BF nº %d)  = %d\n",
              SB.posPrimerBloqueDatos, SB.posPrimerBloqueDatos / 8, SB.posPrimerBloqueDatos % 8, (SB.posPrimerBloqueDatos / BLOCKSIZE) / 8, SB.posPrimerBloqueMB + (SB.posPrimerBloqueDatos / BLOCKSIZE) / 8, SB.posPrimerBloqueDatos, ret);
       ret = leer_bit(SB.posUltimoBloqueDatos);
       liberar_bloque(bloque);
       bread(0, &SB);
       printf("Liberamos ese bloque y después SB.cantBloquesLibres = %d\n", SB.cantBloquesLibres);

       printf("DATOS DEL DIRECTORIO RAIZ\n");
       struct inodo inodo;
       leer_inodo(0, &inodo);
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

       // para el nivel 4:

       int indice = reservar_inodo('f', 6);

       traducir_bloque_inodo(indice, 8, 1);
       traducir_bloque_inodo(indice, 204, 1);
       traducir_bloque_inodo(indice, 30004, 1);
       traducir_bloque_inodo(indice, 400004, 1);
       traducir_bloque_inodo(indice, 16843019, 1);

       leer_inodo(indice, &inodo);

       printf("Tipo: %c\n", inodo.tipo);
       printf("Permisos: %d\n", inodo.permisos);
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

       
*/

       bumount();
       return 0;
}
