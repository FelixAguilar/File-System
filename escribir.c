// Autores: Felix Aguilar, Adrian Bennasar, Alvaro Bueno
#include <stdlib.h>
#include "fichero.h"

/* Fichero: escribir.c:
* ---------------------
* Escribe el contenido de un buffer de memoria pasado por parametro dentro del
* dispositivo virtual de almacenamiento.
*
*  argc: Número de argumentos del comando.
*  argv: Array de strings con los argumentos introducidos junto al comando.
*
* returns: Exit_Success o si se ha producido un error devuelve Exit_Failure.
*/
int main(int argc, char const *argv[])
{
    // Comprueba la sintaxis, si es errónea muestra el formato aceptado.
    if (argc < 4)
    {
        fprintf(stderr, "Sintaxis: escribir <nombre_dispositivo><\"$(cat fiche"
                        "ro)\"><diferentes_inodos>\nOffsets: 9.000, 209.000, 3"
                        "0.725.000, 409.605.000, 480.000.000\nSi diferentes_in"
                        "odos=0 se reserva un solo inodo para todos los offset"
                        "s\n");
        return EXIT_FAILURE;
    }

    // Muestra los offsets y la longitud del texto (bytes).
    printf("Offsets:  9.000, 209.000, 30.725.000, 409.605.000, 480.000.000\n");
    printf("longitud texto: %ld\n\n", strlen(argv[2]));

    // Monta el dispositivo virtual en el sistema.
    if (bmount(argv[1]) == -1)
    {
        fprintf(stderr, "Error al montar el dispositivo virtual.\n");
        return EXIT_FAILURE;
    }

    // Lee el superbloque.
    struct superbloque SB;
    if (bread(0, &SB) == -1)
    {
        fprintf(stderr, "Error de lectura del superbloque.\n");
        return EXIT_FAILURE;
    }

    // Array de offsets a utilizar.
    int offsets[5] = {9000, 209000, 30725000, 409605000, 480000000};

    // Reserva un inodo para la escritura y se comprueba.
    int numInodo = reservar_inodo('f', 6);
    if (numInodo == -1)
    {
        fprintf(stderr, "Error al reservar el inodo.\n");
        return EXIT_FAILURE;
    }

    // Bucle de escritura en todos los offsets del array.
    for (int index = 0; index < (sizeof(offsets) / sizeof(int)); index++)
    {
        // Muestra por pantalla el inodo y el offset donde se va a escribir.
        printf("Nº inodo reservado: %d\n", numInodo);
        printf("offset: %d\n", offsets[index]);

        // Escribe en el dispositivo el buffer pasado por parámetro.
        int bytes_escritos = mi_write_f(numInodo, argv[2], offsets[index],
                                        strlen(argv[2]));

        // Comprueba que la escritura se haya ejecutado correctamente.
        if (bytes_escritos == -1)
        {
            fprintf(stderr, "Error de lectura del dispositivo.\n");
            return EXIT_FAILURE;
        }
        printf("Bytes escritos: %d\n\n", bytes_escritos);

        // Obtiene la información del inodo una vez escrito.
        struct STAT p_stat;
        if (mi_stat_f(numInodo, &p_stat))
        {
            fprintf(stderr, "Error de lectura contenido inodo.\n");
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
        printf("DATOS INODO %d:\n", numInodo);
        printf("tipo=%c\n", p_stat.tipo);
        printf("permisos=%d\n", p_stat.permisos);
        printf("atime: %s\n", atime);
        printf("ctime: %s\n", ctime);
        printf("mtime: %s\n", mtime);
        printf("nLinks = %d\n", p_stat.nlinks);
        printf("tamEnBytesLog = %d\n", p_stat.tamEnBytesLog);
        printf("numBloquesOcupados = %d\n\n", p_stat.numBloquesOcupados);

        // Si el parámetro <diferentes_indodos> es 1, reserva un nuevo inodo.
        if (strcmp(argv[3], "0"))
        {
            numInodo = reservar_inodo('f', 6);
            if (numInodo == -1)
            {
                fprintf(stderr, "Error al reservar el inodo.\n");
                return EXIT_FAILURE;
            }
        }
    }

    // Desmonta el dispositivo virtual del sistema.
    bumount();
    return EXIT_SUCCESS;
}