CPP	= g++
CC	= mpicc
CPPFLAGS	= -O3 -std=c++11 -Wall -fexceptions -msse4.2
CCFLAGS		= -O3 -std=gnu99 -Wall
LIBPATH = -L/usr/lib/ -L~/lib/
INCPATH	= -I../ -I../boost_1_61_0 -I../boost.simd/include
SRC	= ga.c element.c
LFLAGS	= -lm

DEPS = setup.h molecule.h solver.h neighbourhood.h stats.h stratify.h structural_similarity.h descriptor.h power_spectrum.h local_similarity.h run.h
OBJ = setup.o molecule.o solver.o neighbourhood.o stats.o stratify.o structural_similarity.o descriptor.o power_spectrum.o local_similarity.o run.o

COBJ = soap_c_wrap.o
CDEPS = soap_c_wrap.h

default: all

all: static.exe dynamic.exe

%.o: %.cpp $(DEPS) $(CDEPS)
	$(CPP) $(CPPFLAGS) $(INCPATH) -c -o $@ $<

mainc.exe: $(OBJ) $(COBJ)  mainc.c
	$(CC) -c mainc.c -o mainc.o
	$(CPP) -o $@ $(OBJ) $(COBJ) mainc.o $(LFLAGS)

#static compilation
static.exe: $(OBJ) $(COBJ) static.c 
	$(CC) $(CCFLAGS) $(INCPATH) -c static.c
	mpic++ -O3 -Wall -o static.exe $(OBJ) $(COBJ) static.o ~/lib/libga.a $(LFLAGS)

dynamic.exe: static.c 
	$(CC) $(CCFLAGS) $(INCPATH) -c static.c
	mpic++ -O3 -Wall -o dynamic.exe $(OBJ) $(COBJ) static.o $(LFLAGS) -L../ -lga

clean:
	rm -rf *.o *.exe










