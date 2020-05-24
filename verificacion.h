#include "simulacion.h"

// verificacion.h
struct informacion {
    int pid; 
    unsigned int nEscrituras; // validadas 
    struct registro PrimeraEscritura; 
    struct registro UltimaEscritura;
    struct registro MenorPosicion;
    struct registro MayorPosicion;
};
