// Autores: Félix Aguilar, Adrián Bennasar, Álvaro Bueno
#include "simulacion.h"

int acabados = 0;

/* Fichero: simulacion.c:
* -----------------------
* Ejecuta simultaneamte escrituras en el sistema utilizando los hilos.
*
*  argc: número de argumentos introducidos.
*  argv: char array de los argumentos introducidos.
*
*/
int main(int argc, char *argv[])
{
    // Variables utilizadas en la simulacion.
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
    sprintf(camino, "/simul_%s/", tiempo);

    // Crea el directorio para la simulación en la raiz.
    if ((error = mi_creat(camino, 6)) < 0)
    {
        mostrar_error_directorios(error);
        bumount();
        return EXIT_FAILURE;
    }
    printf("Directorio simulacion: %s\n", camino);

    // Asigna la señal de finalizacion de hijo con el reaper.
    signal(SIGCHLD, reaper);

    // Entra en el bucle para la creacion de todos los hijos.
    proceso = 0;
    while (proceso < PROCESOS)
    {
        // Crea un nuevo hijo.
        pid = fork();

        // Si es un hijo.
        if (!pid)
        {
            // Monta el disco.
            if (bmount(disco) < 0)
            {
                fprintf(stderr, "Error: No se ha podido montar el disco.\n");
                exit(0);
            }
            // Crea el directorio.
            sprintf(camino_hijo, "%sproceso_%d/", camino, getpid());
            if ((error = mi_creat(camino_hijo, 6)) < 0)
            {
                fprintf(stderr, "%s\n", camino_hijo);
                mostrar_error_directorios(error);
                bumount();
                exit(0);
            }
            // Crea el archivo.
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
                // Introduce los valores del registro.
                registro.fecha = time(NULL);
                registro.pid = getpid();
                registro.nEscritura = operacion + 1;
                registro.nRegistro = rand() % REGMAX;

                
                // Escribe el registro en su posicion asignada.
                if ((error = mi_write(camino_hijo, &registro,
                                      registro.nRegistro * sizeof(struct registro),
                                      sizeof(struct registro))) < 0)
                {
                    mostrar_error_directorios(error);
                    bumount();
                    exit(0);
                }

                // Augmenta el contador y espera para la siguiente operacion.
                operacion++;
                usleep(WAITH);
            }
            // Imprime el mensaje de finalizacion y desmonta el disco.
            printf("Proceso %d: Completadas %d escrituras en %s\n", proceso + 1,
                   operacion, camino_hijo);
            bumount();
            exit(0);
        }
        // Augmenta el contador y espera para crear el siguiente hijo.
        proceso++;
        usleep(WAITP);
    }
    // Mientras no hayan acabado todos, el padre espera.
    while (acabados < PROCESOS)
    {
        pause();
    }
    // Desmonta el disco y finaliza su ejecucion.
    bumount();
    printf("Total procesos terminados: %d\n", proceso);
    return EXIT_SUCCESS;
}

/* Funcion: reaper:
* -----------------
* Funcion la cual se le llama cuando un hijo finaliza su ejecucion.
*/
void reaper()
{
    pid_t ended;
    signal(SIGCHLD, reaper);
    while ((ended = waitpid(-1, NULL, WNOHANG)) > 0)
    {
        acabados++;
#if DEBUG
        fprintf(stderr, "acabado %d total acabados: %d\n", ended, acabados);
#endif
    }
}