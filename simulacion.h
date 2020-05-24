#include <sys/wait.h>
#include <signal.h>
#include "directorios.h"

#define OPERACIONES 50
#define PROCESOS 100

#define REGMAX 500000
#define WAITP 200000
#define WAITH 50000

struct registro
{
    time_t fecha;   //fecha de la escritura en formato epoch
    pid_t pid;      //PID del proceso que lo ha creado
    int nEscritura; //Entero con el número de escritura (de 1 a 50)
    int nRegistro;  //Entero con el número del registro dentro del fichero (de 0 a REGMAX-1)
};

void reaper();