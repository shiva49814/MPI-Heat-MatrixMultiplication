CXX=mpicxx
LD=mpicxx
CXXFLAGS=-O3 -std=c++11
CFLAGS=-O3

all: heat mat_mul

heat: heat.o
	$(LD) $(LDFLAGS) heat.o -o heat

mat_mul: mat_mul.o
	$(LD) $(LDFLAGS) mat_mul.o -o mat_mul
