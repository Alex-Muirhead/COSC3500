LIBRARY_DIRS = /usr/lib/x86_64-linux-gnu/hdf5/serial
INCLUDE_DIRS = /usr/include/hdf5/serial

output:
	gcc decay.c -I$(INCLUDE_DIRS) -L$(LIBRARY_DIRS) -lhdf5 -std=gnu99 -o decay

clean:
	rm decay