CC=gcc
CXX=g++

# all targets
TARGETS=hello decay

# the default target, if we don't specify anything else
default : hello

# decay.c requires hdf5 library
LIBS_decay = -L/usr/lib/x86_64-linux-gnu/hdf5/serial -lhdf5
CFLAGS_decay = -I/usr/include/hdf5/serial


% : %.c
	$(CC) $(CFLAGS) $(CFLAGS_$@) $< $(LDFLAGS) $(LIBS_$@) -std=gnu99 -o $@

% : %.cpp
	$(CXX) $(CXXFLAGS) $(CXXFLAGS_$@) $< $(LDFLAGS) $(LIBS_$@) -std=c++11 -o $@

clean:
	rm -f $(TARGETS)

.PHONY: clean default
