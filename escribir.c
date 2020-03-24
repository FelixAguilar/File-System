#include <stdlib.h>
#include "fichero.h"

int main(int argc, char const *argv[])
{
    if (argc < 3)
    {
        printf("Sintaxis: escribir <nombre_dispositivo><\"$(cat fichero)\"><diferentes_inodos>\n");
        printf("Offsets:  9000, 209000, 30725000, 409605000\n");
        printf("Si diferentes_inodos=0 se reserva un solo inodo para todos los offsets\n");
        return 0;
    }

    printf("longitud texto: %ld\n", strlen(argv[2]));

    bmount(argv[1]);
    struct superbloque SB;
    bread(0, &SB);

    struct tm *ts;
    char atime[80];
    char mtime[80];
    char ctime[80];

    int numInodo = reservar_inodo('f', 6);

    printf("Nº inodo reservado: %d\n", numInodo);

    //Escribimos con diferentes offsets para utilizar los diferentes tipos de punteros.

    //OFFSET 9000 #########################################################################################################
    printf("offset: %d\n", 9000);
    int bytes_escritos = mi_write_f(numInodo, argv[2], 9000, strlen(argv[2]));
    printf("Bytes escritos: %d\n", bytes_escritos);
    struct STAT p_stat;

    mi_stat_f(numInodo, &p_stat);
    printf("DATOS INODO %d:\n", numInodo);
    printf("tipo=%c\n", p_stat.tipo);
    printf("permisos=%d\n", p_stat.permisos);

    ts = localtime(&p_stat.atime);
    strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&p_stat.mtime);
    strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&p_stat.ctime);
    strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S", ts);

    printf("atime: %s\n", atime);
    printf("mtime: %s\n", mtime);
    printf("ctime: %s\n", ctime);

    printf("nLinks = %d\n", p_stat.nlinks);
    printf("tamEnBytesLog = %d\n", p_stat.tamEnBytesLog);
    printf("numBloquesOcupados = %d\n", p_stat.numBloquesOcupados);

    //OFFSET 209000 #########################################################################################################
    if (strcmp(argv[3],"0"))
    {
        numInodo = reservar_inodo('f', 6);
        printf("Nº inodo reservado: %d\n", numInodo);
    }
    printf("offset: %d\n", 209000);
    bytes_escritos = mi_write_f(numInodo, argv[2], 209000, strlen(argv[2]));
    printf("Bytes escritos: %d\n", bytes_escritos);

    mi_stat_f(numInodo, &p_stat);
    printf("DATOS INODO %d:\n", numInodo);
    printf("tipo=%c\n", p_stat.tipo);
    printf("permisos=%d\n", p_stat.permisos);

    ts = localtime(&p_stat.atime);
    strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&p_stat.mtime);
    strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&p_stat.ctime);
    strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S", ts);

    printf("atime: %s\n", atime);
    printf("mtime: %s\n", mtime);
    printf("ctime: %s\n", ctime);

    printf("nLinks = %d\n", p_stat.nlinks);
    printf("tamEnBytesLog = %d\n", p_stat.tamEnBytesLog);
    printf("numBloquesOcupados = %d\n", p_stat.numBloquesOcupados);

    //OFFSET 30725000 #########################################################################################################
    if (strcmp(argv[3],"0"))
    {
        numInodo = reservar_inodo('f', 6);
        printf("Nº inodo reservado: %d\n", numInodo);
    }
    printf("offset: %d\n", 30725000);
    bytes_escritos = mi_write_f(numInodo, argv[2], 30725000, strlen(argv[2]));
    printf("Bytes escritos: %d\n", bytes_escritos);

    mi_stat_f(numInodo, &p_stat);
    printf("DATOS INODO %d:\n", numInodo);
    printf("tipo=%c\n", p_stat.tipo);
    printf("permisos=%d\n", p_stat.permisos);

    ts = localtime(&p_stat.atime);
    strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&p_stat.mtime);
    strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&p_stat.ctime);
    strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S", ts);

    printf("atime: %s\n", atime);
    printf("mtime: %s\n", mtime);
    printf("ctime: %s\n", ctime);

    printf("nLinks = %d\n", p_stat.nlinks);
    printf("tamEnBytesLog = %d\n", p_stat.tamEnBytesLog);
    printf("numBloquesOcupados = %d\n", p_stat.numBloquesOcupados);

    //OFFSET 409605000 #########################################################################################################
    if (strcmp(argv[3],"0"))
    {
        numInodo = reservar_inodo('f', 6);
        printf("Nº inodo reservado: %d\n", numInodo);
    }
    printf("offset: %d\n", 409605000);
    bytes_escritos = mi_write_f(numInodo, argv[2], 409605000, strlen(argv[2]));
    printf("Bytes escritos: %d\n", bytes_escritos);

    mi_stat_f(numInodo, &p_stat);
    printf("DATOS INODO %d:\n", numInodo);
    printf("tipo=%c\n", p_stat.tipo);
    printf("permisos=%d\n", p_stat.permisos);

    ts = localtime(&p_stat.atime);
    strftime(atime, sizeof(atime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&p_stat.mtime);
    strftime(mtime, sizeof(mtime), "%a %Y-%m-%d %H:%M:%S", ts);
    ts = localtime(&p_stat.ctime);
    strftime(ctime, sizeof(ctime), "%a %Y-%m-%d %H:%M:%S", ts);

    printf("atime: %s\n", atime);
    printf("mtime: %s\n", mtime);
    printf("ctime: %s\n", ctime);

    printf("nLinks = %d\n", p_stat.nlinks);
    printf("tamEnBytesLog = %d\n", p_stat.tamEnBytesLog);
    printf("numBloquesOcupados = %d\n", p_stat.numBloquesOcupados);

    bumount();
    return 0;
}