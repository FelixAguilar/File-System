CC=gcc
CFLAGS=-c -g -Wall -std=gnu99
#LDFLAGS=-pthread

SOURCES=bloques.c mi_mkfs.c ficheros_basico.c fichero.c leer_sf.c escribir.c leer.c permitir.c borrar.c #todos los .c
LIBRARIES=bloques.o ficheros_basico.o fichero.o #todos los .o de la biblioteca del SF
INCLUDES=bloques.h ficheros_basico.h fichero.h #todos los .h
PROGRAMS=mi_mkfs leer_sf escribir leer permitir borrar
OBJS=$(SOURCES:.c=.o)

all: $(OBJS) $(PROGRAMS)

$(PROGRAMS): $(LIBRARIES) $(INCLUDES) 
	$(CC) $(LDFLAGS) $(LIBRARIES) $@.o -o $@

%.o: %.c $(INCLUDES) 
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	rm -rf *.o *~ $(PROGRAMS)
