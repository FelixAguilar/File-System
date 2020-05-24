// Félix Aguilar, Adrián Bennasar, Álvaro Bueno
#include "verificacion.h"

/* Fichero: mi_cat.c
* ------------------
*  
*
*  argc: número de argumentos introducidos.
*  argv: char array de los argumentos introducidos.
*
*  returns: 
*/
int main(int argc, char const *argv[])
{
    struct STAT stat;
    int error;
    int numEntradas;
    char camino[250];
    int entrada = 0;
    char *pid;
    char buffer[1000];
    char caminoProceso[250];
    struct registro registro;
    int numRegistro;
    struct informacion info;
    struct tm *ts;
    char pritime[80];
    char ulttime[80];
    char mentime[80];
    char maytime[80];
    int escritos = 0;

    // Comprobación de la sintaxis de los argumentos.
    if (argc != 3)
    {
        fprintf(stderr, "Sintaxis: verificacion <disco> <directorio_simulacion>\n");
        return EXIT_FAILURE;
    }
    if (bmount(argv[1]) < 0)
    {
        fprintf(stderr, "Error: no se ha podido montar el disco\n");
    }
    if ((error = mi_stat(argv[2], &stat)) < 0)
    {
        mostrar_error_directorios(error);
        return EXIT_FAILURE;
    }
    numEntradas = stat.tamEnBytesLog / sizeof(struct entrada);
    struct entrada entradas[numEntradas];
    if (numEntradas != PROCESOS)
    {
        return EXIT_FAILURE;
    }
    sprintf(camino, "%sinforme.txt", argv[2]);
    if ((error = mi_creat(camino, 6)) < 0)
    {
        mostrar_error_directorios(error);
        return EXIT_FAILURE;
    }

    if ((error = mi_read(argv[2], entradas, 0, stat.tamEnBytesLog)) < 0)
    {
        mostrar_error_directorios(error);
        return EXIT_FAILURE;
    }

    while (entrada < numEntradas)
    {
        pid = strchr(entradas[entrada].nombre, '_');
        info.pid = atoi(pid + 1);
        sprintf(caminoProceso, "%s%s/prueba.dat", argv[2], entradas[entrada].nombre);

        numRegistro = 0;
        info.nEscrituras = 0;

        while ((error = mi_read(caminoProceso, &registro, numRegistro * sizeof(struct registro), sizeof(struct registro))) > 0)
        {
            if (registro.pid == info.pid)
            {
                if (!info.nEscrituras)
                {
                    info.PrimeraEscritura = registro;
                    info.UltimaEscritura = registro;
                    info.MenorPosicion = registro;
                    info.MayorPosicion = registro;
                    info.nEscrituras = 1;
                }
                else
                {
                    if ((difftime(registro.fecha, info.PrimeraEscritura.fecha) < 0) || ((difftime(registro.fecha, info.PrimeraEscritura.fecha) == 0) && (registro.nEscritura < info.PrimeraEscritura.nEscritura)))
                    {
                        info.PrimeraEscritura = registro;
                    }
                    if ((difftime(registro.fecha, info.UltimaEscritura.fecha) > 0) || ((difftime(registro.fecha, info.UltimaEscritura.fecha) == 0) && (registro.nEscritura > info.UltimaEscritura.nEscritura)))
                    {
                        info.UltimaEscritura = registro;
                    }
                    if (registro.nRegistro < info.MenorPosicion.nRegistro)
                    {
                        info.MenorPosicion = registro;
                    }
                    if (registro.nRegistro > info.MayorPosicion.nRegistro)
                    {
                        info.MayorPosicion = registro;
                    }
                    info.nEscrituras++;
                }
            }
            numRegistro++;
        }
        if (error < 0)
        {
            mostrar_error_directorios(error);
            return EXIT_FAILURE;
        }

        // Cambia el formato de la fecha y la hora de los campos del inodo.
        ts = localtime(&info.PrimeraEscritura.fecha);
        strftime(pritime, sizeof(pritime), "%a %Y-%m-%d %H:%M:%S", ts);
        ts = localtime(&info.UltimaEscritura.fecha);
        strftime(ulttime, sizeof(ulttime), "%a %Y-%m-%d %H:%M:%S", ts);
        ts = localtime(&info.MenorPosicion.fecha);
        strftime(mentime, sizeof(mentime), "%a %Y-%m-%d %H:%M:%S", ts);
        ts = localtime(&info.MayorPosicion.fecha);
        strftime(maytime, sizeof(maytime), "%a %Y-%m-%d %H:%M:%S", ts);

        memset(buffer, 0, sizeof(buffer));

        sprintf(buffer, "PID: %d\nNumero de escrituras:\t%d\nPrimera escritura:"
                        "\t%d\t%d\t%s\nUltima escritura:\t%d\t%d\t%s\nMayor po"
                        "sición:\t\t%d\t%d\t%s\nMenor posición:\t\t%d\t%d\t%s\n\n",
                info.pid, info.nEscrituras,
                info.PrimeraEscritura.nEscritura,
                info.PrimeraEscritura.nRegistro,
                pritime,
                info.UltimaEscritura.nEscritura,
                info.UltimaEscritura.nRegistro,
                ulttime,
                info.MenorPosicion.nEscritura,
                info.MenorPosicion.nRegistro,
                mentime,
                info.MayorPosicion.nEscritura,
                info.MayorPosicion.nRegistro,
                maytime);

        if ((error = mi_write(camino, buffer, escritos, strlen(buffer))) < 0)
        {
            mostrar_error_directorios(error);
            return EXIT_FAILURE;
        }
        escritos = strlen(buffer) + escritos;
        entrada++;
    }
    return EXIT_SUCCESS;
}