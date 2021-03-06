# Autores: Felix Aguilar, Adrian Bennasar, Alvaro Bueno
CC=gcc
CFLAGS=-c -g -Wall -std=gnu99
#LDFLAGS=-pthread

SOURCES=bloques.c mi_mkfs.c ficheros_basico.c fichero.c leer_sf.c escribir.c leer.c permitir.c truncar.c directorios.c prueba.c mi_mkdir.c mi_chmod.c mi_touch.c mi_ls.c mi_stat.c mi_escribir.c mi_cat.c mi_escribir_varios.c mi_link.c mi_rm.c mi_rmdir.c mi_rm_r.c#todos los .c
LIBRARIES=bloques.o ficheros_basico.o fichero.o directorios.o #todos los .o de la biblioteca del SF
INCLUDES=bloques.h ficheros_basico.h fichero.h directorios.h #todos los .h
PROGRAMS=mi_mkfs leer_sf escribir leer permitir truncar prueba mi_mkdir mi_chmod mi_touch mi_ls mi_stat mi_escribir mi_cat mi_escribir_varios mi_link mi_rm mi_rmdir mi_rm_r
OBJS=$(SOURCES:.c=.o)

all: $(OBJS) $(PROGRAMS)

$(PROGRAMS): $(LIBRARIES) $(INCLUDES) 
	$(CC) $(LDFLAGS) $(LIBRARIES) $@.o -o $@

%.o: %.c $(INCLUDES) 
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	rm -rf *.o *~ $(PROGRAMS)
