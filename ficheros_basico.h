#include <stdio.h>  // printf(), fprintf(), stderr, stdout, stdin
#include <stdlib.h> // exit(), EXIT_SUCCESS, EXIT_FAILURE, atoi()
#include <unistd.h> // SEEK_SET, read(), write(), open(), close(), lseek()
#include <errno.h>  // errno
#include <string.h> // strerror()
#include <limits.h> // límite numérico 

int tamMB(unsigned int nbloques);
int tamAI(unsigned int ninodos);
int initSB (unsigned int nbloques, unsigned int ninodos);
int initMB();
int initAI();