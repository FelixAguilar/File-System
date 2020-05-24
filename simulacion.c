// Félix Aguilar, Adrián Bennasar, Álvaro Bueno
#include "simulacion.h"

int acabados = 0;

/* Fichero: simulacion.c:
* ----------------------
* 
*
*  argc: número de argumentos introducidos.
*  argv: char array de los argumentos introducidos.
*
*  returns: 
*/
int main(int argc, char *argv[])
{
    // Variables.
    char *disco;
    char tiempo[80];
    char camino[150];
    char camino_hijo[150];
    time_t t;
    int error;
    int proceso;
    int pid;
    int operacion;
    struct registro registro;

    // Comprobación de la sintaxis de los argumentos.
    if (argc != 2)
    {
        fprintf(stderr, "Sintaxis: simulacion <disco>\n");
        return EXIT_FAILURE;
    }
    // Monta el disco.
    disco = argv[1];
    if (bmount(disco) < 0)
    {
        fprintf(stderr, "Error: No se ha podido montar el disco.\n");
        return EXIT_FAILURE;
    }
    // Obtencion de la fecha y hora para el path de la raiz.
    t = time(NULL);
    strftime(tiempo, sizeof(tiempo), "%Y%m%d%H%M%S", localtime(&t));
    strcat(camino, "/simul_");
    strcat(camino, tiempo);
    strcat(camino, "/");

    if ((error = mi_creat(camino, 6)) < 0)
    {
        mostrar_error_directorios(error);
        bumount();
        return EXIT_FAILURE;
    }
    signal(SIGCHLD, reaper);

    printf("Directorio simulacion: %s\n", camino);
    proceso = 0;
    while (proceso < PROCESOS)
    {
        pid = fork();
        if (!pid)
        {
            // Monta el disco para el hijo.
            if (bmount(disco) < 0)
            {
                fprintf(stderr, "Error: No se ha podido montar el disco.\n");
                exit(0);
            }
            // Crea el directorio del hijo.
            sprintf(camino_hijo, "%sproceso_%d/", camino, getpid());
            if ((error = mi_creat(camino_hijo, 6)) < 0)
            {
                fprintf(stderr, "%s\n", camino_hijo);
                mostrar_error_directorios(error);
                bumount();
                exit(0);
            }
            // Crea el archivo del hijo.
            sprintf(camino_hijo, "%sprueba.dat", camino_hijo);
            if ((error = mi_creat(camino_hijo, 6)) < 0)
            {
                fprintf(stderr, "%s\n", camino_hijo);
                mostrar_error_directorios(error);
                bumount();
                exit(0);
            }
            // Escribe de forma sistematica en el archivo 50 veces registros.
            srand(time(NULL) + getpid());
            operacion = 0;
            while (operacion < OPERACIONES)
            {
                registro.fecha = time(NULL);
                registro.pid = getpid();
                registro.nEscritura = operacion + 1;
                registro.nRegistro = rand() % REGMAX;
                if ((error = mi_write(camino_hijo, &registro,
                                      registro.nRegistro * sizeof(struct registro),
                                      sizeof(struct registro))) < 0)
                {
                    mostrar_error_directorios(error);
                    bumount();
                    exit(0);
                }
                operacion++;
                usleep(WAITH);
            }

            printf("Proceso %d: Completadas %d escrituras en %s\n", proceso + 1,
                   operacion, camino_hijo);
            // Desmonta el disco.
            bumount();
            exit(0);
        }
        proceso++;
        usleep(WAITP);
    }

    while (acabados < PROCESOS)
    {
        pause();
    }
    bumount();

    printf("Total procesos terminados: %d\n", proceso);
    return EXIT_SUCCESS;
}

void reaper()
{
    pid_t ended;
    signal(SIGCHLD, reaper);
    while ((ended = waitpid(-1, NULL, WNOHANG)) > 0)
    {
        acabados++;
        //fprintf(stderr, "acabado %d total acabados: %d\n", ended, acabados);
    }
}