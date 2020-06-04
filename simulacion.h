// Autores: Félix Aguilar, Adrián Bennasar, Álvaro Bueno
#include <sys/wait.h>
#include <signal.h>
#include "directorios.h"

#define OPERACIONES 50 // Operaciones a realizar por cada hijo.
#define PROCESOS 100  // Hilos de ejecucion que se van a realizar.

#define REGMAX 500000 // Registro maximo a escribir.
#define WAITP 200000  // Tiempo de espera del padre (microseg)
#define WAITH 50000   // Tiempo de espera del hijo (microseg)

struct registro
{
    time_t fecha;   // Fecha de la escritura en formato epoch.
    pid_t pid;      // PID del proceso que lo ha creado.
    int nEscritura; // Entero de escritura (de 1 a 50).
    int nRegistro;  // Entero del registro dentro del fichero (de 0 a REGMAX-1).
};

void reaper();