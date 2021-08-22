# set some defaults.  ?= means don't override the value if it was set already
CC?=gcc
CXX?=g++

CFLAGS?=-std=c99 -g
CXXFLAGS?=-std=c++11 -g

# all targets
TARGETS=race_thread race_openmp hello_openmp pi_serial pi_parallel_v1  pi_parallel_v2

# The first rule in the Makefile is the default target that is made if 'make' is invoked with
# no parameters.  'all' is a dummy target that will make everything
default : all

CXXFLAGS_race_thread = -pthread
CXXFLAGS_race_openmp = -fopenmp
CXXFLAGS_hello_openmp = -fopenmp
CXXFLAGS_pi_parallel_v1 = -fopenmp
CXXFLAGS_pi_parallel_v2 = -fopenmp

# io.o uses the hdf5 header files
CFLAGS_io = -I/usr/include/hdf5/serial

# Add dependencies for the object files and executable
decay.o : decay.c io.h

io.o : io.h io.c

# we don't need to specify that decay depends on decay.o - this is will be found automatically
decay : io.o


# wildcard rules - compile source code to object files and executables

%.o : %.c
	$(CC) $(CFLAGS) $(CFLAGS_$(basename $<)) -c $< -o $@

% : %.c
	$(CC) $(CFLAGS) $(CFLAGS_$@) $(filter %.o %.c, $^) $(LDFLAGS) $(LIBS_$@) -o $@

%.o : %.cpp
	$(CXX) $(CXXFLAGS) $(CFLAGS_$(basename $<)) -c $< -o $@

% : %.cpp
	$(CXX) $(CXXFLAGS) $(CXXFLAGS_$@) $(filter %.o %.cpp, $^) $(LDFLAGS) $(LIBS_$@) -o $@

all : $(TARGETS)

clean:
	rm -f $(TARGETS) *.o

.PHONY: clean default all
