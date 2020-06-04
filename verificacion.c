// Félix Aguilar, Adrián Bennasar, Álvaro Bueno
#include "verificacion.h"

/* Fichero: verificacion.c
* ------------------------
*  
*
*  argc: número de argumentos introducidos.
*  argv: char array de los argumentos introducidos.
*
*  returns: 
*/
int main(int argc, char const *argv[])
{
    // Variables de estado.
    int error;
    int entrada = 0;
    int escritos = 0;
    int registro;

    // Variables del directorio.
    struct STAT stat;
    int numEntradas;
    char camino[250];

    // Variables del proceso.
    char *pid;
    int totalRegistros;
    char caminoProceso[250];
    struct registro *registros;
    struct informacion info;

    // Variables de salida.
    char buffer[1000];
    char pritime[80];
    char ulttime[80];
    char mentime[80];
    char maytime[80];
    struct tm *ts;

    // Comprobación de la sintaxis de los argumentos.
    if (argc != 3)
    {
        fprintf(stderr, "Sintaxis: verificacion <disco> <directorio_simulacion>\n");
        return EXIT_FAILURE;
    }
    // Monta el disco en el sistema.
    if (bmount(argv[1]) < 0)
    {
        fprintf(stderr, "Error: no se ha podido montar el disco\n");
    }
    // Lectura del inodo del directorio.
    if ((error = mi_stat(argv[2], &stat)) < 0)
    {
        mostrar_error_directorios(error);
        return EXIT_FAILURE;
    }

    // Calculo de entradas en el directorio.
    numEntradas = stat.tamEnBytesLog / sizeof(struct entrada);
    if (numEntradas != PROCESOS)
    {
        return EXIT_FAILURE;
    }
    struct entrada entradas[numEntradas];

    // Crea el camino al informe y el documento.
    sprintf(camino, "%sinforme.txt", argv[2]);
    if ((error = mi_creat(camino, 6)) < 0)
    {
        mostrar_error_directorios(error);
        return EXIT_FAILURE;
    }
    // Lee las entradas del directorio y las guarda.
    if ((error = mi_read(argv[2], entradas, 0, stat.tamEnBytesLog)) < 0)
    {
        mostrar_error_directorios(error);
        return EXIT_FAILURE;
    }
    // Mientras haya entradas que tratar.
    while (entrada < numEntradas)
    {
        // Obtiene el pid del proceso con el nombre de la entrada.
        pid = strchr(entradas[entrada].nombre, '_');
        info.pid = atoi(pid + 1);

        // Obtiene el camino del archivo del proceso.
        memset(caminoProceso, 0, 250);
        sprintf(caminoProceso, "%s%s/prueba.dat", argv[2], entradas[entrada].nombre);

        //Obtiene tamaño del archivo.
        if ((error = mi_stat(caminoProceso, &stat)) < 0)
        {
            mostrar_error_directorios(error);
            return EXIT_FAILURE;
        }

        // Inicio de la lectura del archivo.
        totalRegistros = (stat.tamEnBytesLog / sizeof(struct registro));
        registros = malloc(stat.tamEnBytesLog);
        if (!registros)
        {
            return EXIT_FAILURE;
        }
        info.nEscrituras = 0;
        registro = 0;

        // Lectura de registros.
        if ((error = mi_read(caminoProceso, registros, 0, stat.tamEnBytesLog)) < 0)
        {
            mostrar_error_directorios(error);
            return EXIT_FAILURE;
        }

        // Procesado de los valores de registros.
        while (registro < totalRegistros)
        {

            //fprintf(stderr, "Registro %d de %d\n", registro, totalRegistros);

            // El registro leido es del mismo pid que el procesado.
            if (registros[registro].pid == info.pid)
            {
                // Actualiza los elementos del primer
                if (!info.nEscrituras)
                {
                    info.PrimeraEscritura = registros[registro];
                    info.UltimaEscritura = registros[registro];
                    info.MenorPosicion = registros[registro];
                    info.MayorPosicion = registros[registro];
                    info.nEscrituras = 1;
                }
                else
                {
                    if ((difftime(registros[registro].fecha, info.PrimeraEscritura.fecha) < 0) || ((difftime(registros[registro].fecha, info.PrimeraEscritura.fecha) == 0) && (registros[registro].nEscritura < info.PrimeraEscritura.nEscritura)))
                    {
                        info.PrimeraEscritura = registros[registro];
                    }
                    if ((difftime(registros[registro].fecha, info.UltimaEscritura.fecha) > 0) || ((difftime(registros[registro].fecha, info.UltimaEscritura.fecha) == 0) && (registros[registro].nEscritura > info.UltimaEscritura.nEscritura)))
                    {
                        info.UltimaEscritura = registros[registro];
                    }
                    if (registros[registro].nRegistro < info.MenorPosicion.nRegistro)
                    {
                        info.MenorPosicion = registros[registro];
                    }
                    if (registros[registro].nRegistro > info.MayorPosicion.nRegistro)
                    {
                        info.MayorPosicion = registros[registro];
                    }
                    info.nEscrituras++;
                }
            }
            registro++;
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
                        "\t%d\t%d\t%s\nUltima escritura:\t%d\t%d\t%s\nMenor po"
                        "sición:\t\t%d\t%d\t%s\nMayor posición:\t\t%d\t%d\t%s\n\n",
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

        fprintf(stderr, "%d escrituras validadas en %s\n", info.nEscrituras, caminoProceso);
    }

    fprintf(stderr, "Procesos validados: %d\n", entrada);
    return EXIT_SUCCESS;
}