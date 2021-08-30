# set some defaults.  ?= means don't override the value if it was set already
MPICXX?=mpic++

CFLAGS?=-std=c99 -g
CXXFLAGS?=-std=c++11 -g

# all targets
TARGETS=hello send-recv ring

# The first rule in the Makefile is the default target that is made if 'make' is invoked with
# no parameters.  'all' is a dummy target that will make everything
default : all

# wildcard rules - compile source code to object files and executables

%.o : %.cpp
	$(MPICXX) $(CXXFLAGS) $(CFLAGS_$(basename $<)) -c $< -o $@

% : %.cpp
	$(MPICXX) $(CXXFLAGS) $(CXXFLAGS_$@) $(filter %.o %.cpp, $^) $(LDFLAGS) $(LIBS_$@) -o $@

all : $(TARGETS)

clean:
	rm -f $(TARGETS) *.o

.PHONY: clean default all
