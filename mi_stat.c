// Autores: Félix Aguilar, Adrián Bennasar, Álvaro Bueno.
#include "directorios.h"

/* Fichero: mi_stat.c:
* --------------------
* Este archivo permite mostrar los metadatos de un inodo.
*
* returns: Exit_Success o bien Exit_Failure si se produce un error.
*/
int main(int argc, char const *argv[])
{
    // Comprueba que la sintaxis sea correcta.
    if (argc != 3)
    {
        fprintf(stderr,
                "Error de sintaxis: ./mi_chmod <disco></ruta>\n");
        return EXIT_FAILURE;
    }
    // Monta el disco en el sistema.
    if (bmount(argv[1]) == -1)
    {
        fprintf(stderr, "Error de montaje de disco.\n");
        return EXIT_FAILURE;
    }
    // Obtiene los datos del inodo.
    struct STAT p_stat;
    int p_inodo;
    if ((p_inodo = mi_stat(argv[2], &p_stat)) < 0)
    {
        mostrar_error_directorios(p_inodo);
        return EXIT_FAILURE;
    }
    // Variables utilizadas para cambiar el formato de la fecha y hora.
    struct tm *ts;
    char atime[80];
    char mtime[80];
    char ctime[80];

    // Cambia el formato de la fecha y la hora de los campos del inodo.
    ts = localtime(&p_stat.atime);
    strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&p_stat.mtime);
    strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&p_stat.ctime);
    strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S", ts);

    // Mustra los valores de los meta datos.
    printf("inodo: %d\n", p_inodo);
    printf("tipo: %c\n", p_stat.tipo);
    printf("permisos: %d\n", p_stat.permisos);
    printf("atime: %s\n", atime);
    printf("mtime: %s\n", mtime);
    printf("ctime: %s\n", ctime);
    printf("nlinks: %d\n", p_stat.nlinks);
    printf("tamEnBytesLog: %d\n", p_stat.tamEnBytesLog);
    printf("numBloquesOcupados: %d\n", p_stat.numBloquesOcupados);

    bumount();
}