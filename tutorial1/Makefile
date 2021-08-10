# set some defaults.  ?= means don't override the value if it was set already
CC?=gcc
CXX?=g++

CFLAGS?=-std=c99 -g
CXXFLAGS?=-std=c++11 -g

# all targets
TARGETS=hello decay

# the default target, if we don't specify anything else
default : hello

# decay.c requires hdf5 library
LIBS_decay = -L/usr/lib/x86_64-linux-gnu/hdf5/serial -lhdf5
CFLAGS_decay = -I/usr/include/hdf5/serial


% : %.c
	$(CC) $(CFLAGS) $(CFLAGS_$@) $< $(LDFLAGS) $(LIBS_$@) -o $@

% : %.cpp
	$(CXX) $(CXXFLAGS) $(CXXFLAGS_$@) $< $(LDFLAGS) $(LIBS_$@) -o $@

all : $(TARGETS)

clean:
	rm -f $(TARGETS) *.o

.PHONY: clean default all
