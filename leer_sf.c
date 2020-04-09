// Autores: Félix Aguilar, Adrián Bennasar, Álvaro Bueno.
#include <stdlib.h>
#include "ficheros_basico.h"

/* Fichero: leer_sf.c:
* --------------------
* La ejecución de este fichero permite mostrar el contenido del superbloque.
*  
*  argc: número de argumentos introducidos por el usuario.
*  argsv: array de strings de argumentos introducidos por el usuario.
*
* return: devuelve Exit_Success o Exit_Failure si ha habido un error.
*/
int main(int argc, char const *argv[])
{
       // Comprueba que la sintaxis sea correcta.
       if (argc != 2)
       {
              fprintf(stderr,
                      "Error de sintaxis: ./leer_sf <nombre_dispositivo>\n");
              return EXIT_FAILURE;
       }

       // Monta el disco en el sistema.
       if (bmount(argv[1]) == -1)
       {
              fprintf(stderr, "Error de montaje de disco.\n");
              return EXIT_FAILURE;
       }

       // Lee el superbloque del disco.
       struct superbloque SB;
       if (bread(0, &SB) == -1)
       {
              fprintf(stderr, "Error de lectura del superbloque.\n");
              return EXIT_FAILURE;
       }

       // Muestra por consola el contenido del superbloque.
       printf("DATOS DEL SUPERBLOQUE\n");
       printf("posPrimerBloqueMB = %d\n", SB.posPrimerBloqueMB);
       printf("posUltimoBloqueMB = %d\n", SB.posUltimoBloqueMB);
       printf("posPrimerBloqueAI = %d\n", SB.posPrimerBloqueAI);
       printf("posUltimoBloqueAI = %d\n", SB.posUltimoBloqueAI);
       printf("posPrimerBloqueDatos = %d\n", SB.posPrimerBloqueDatos);
       printf("posUltimoBloqueDatos = %d\n", SB.posUltimoBloqueDatos);
       printf("posInodoRaiz = %d\n", SB.posInodoRaiz);
       printf("posPrimerInodoLibre = %d\n", SB.posPrimerInodoLibre);
       printf("cantBloquesLibres = %d\n", SB.cantBloquesLibres);
       printf("cantInodosLibres = %d\n", SB.cantInodosLibres);
       printf("totBloques = %d\n", SB.totBloques);
       printf("totInodos = %d\n", SB.totInodos);

       // Desmonta el disco del sistema.
       bumount();
       return EXIT_SUCCESS;
}