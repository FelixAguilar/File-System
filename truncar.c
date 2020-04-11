// Autores: Félix Aguilar, Adrián Bennasar, Álvaro Bueno
#include <stdlib.h>
#include "fichero.h"

/* Fichero: truncar.c: 
* --------------------
* Permite el borrado parcial o total del contenido de un archivo.
*
* argc: número de parámetros.
* argv: array de strings que contiene los parámetros.
*
* returns: Exit_Success o Exit_Failure si se produce un error.
*/
int main(int argc, char const *argv[])
{
    // Comprueba que la sintaxis es correcta.
    if (argc < 3)
    {
        fprintf(stderr, "Sintaxis: truncar <nombre_dispositivo><numero_inodo>"
                        "<n_bytes>\n");
        return EXIT_FAILURE;
    }

    // Guardamos los parámetros en las correspondientes variables.
    int ninodo = atoi(argv[2]);
    int n_bytes = atoi(argv[3]);

    // Monta el dispositivo virtual en el sistema y lee el superbloque.
    if (bmount(argv[1]) == -1)
    {
        fprintf(stderr, "Error al montar el dispositivo en el sistema\n");
        return EXIT_FAILURE;
    }
    struct superbloque SB;
    if (bread(0, &SB) == -1)
    {
        fprintf(stderr, "Error al leer el superbloque\n");
        return EXIT_FAILURE;
    }

    // Si n_bytes es 0, libera el inodo y si no trunca el archivo.
    if (!n_bytes)
    {
        if (liberar_inodo(ninodo) == -1)
        {
            fprintf(stderr, "Error al liberar el inodo\n");
            return EXIT_FAILURE;
        }
    }
    else
    {
        mi_truncar_f(ninodo, n_bytes);
    }

    // Lee el inodo liberado y muestra la metainformación.
    struct STAT p_stat;
    if (mi_stat_f(ninodo, &p_stat))
    {
        fprintf(stderr, "Error al leer el inodo\n");
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

    // Muestra la información del inodo utilizado para la escritura.
    printf("DATOS INODO %d:\n", ninodo);
    printf("tipo=%c\n", p_stat.tipo);
    printf("permisos=%d\n", p_stat.permisos);
    printf("atime: %s\n", atime);
    printf("ctime: %s\n", ctime);
    printf("mtime: %s\n", mtime);
    printf("nLinks = %d\n", p_stat.nlinks);
    printf("tamEnBytesLog = %d\n", p_stat.tamEnBytesLog);
    printf("numBloquesOcupados = %d\n\n", p_stat.numBloquesOcupados);

    // Desmonta el dispositivo.
    bumount();
    return 0;
}