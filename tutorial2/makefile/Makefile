LIBRARY_DIRS = /usr/lib/x86_64-linux-gnu/hdf5/serial
INCLUDE_DIRS = /usr/include/hdf5/serial
CFLAGS = -I$(INCLUDE_DIRS) -L$(LIBRARY_DIRS) -lhdf5 -std=gnu99

output: decay.o io.o
	gcc decay.o io.o ${CFLAGS} -o decay

decay.o: decay.c io.h
	gcc -c ${CFLAGS} decay.c

io.o: io.c io.h
	gcc -c ${CFLAGS} io.c

clean:
	rm decay.o io.o decay