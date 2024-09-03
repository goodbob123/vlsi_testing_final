#

CFLAGS = -std=c++11 -g  

# atpg.o main.o input.o level.o sim.o podem.o init_flist.o faultsim.o tdfsim.o display.o
SOURCES= $(wildcard src/*.cpp)
INCLUDES= $(wildcard src/*.h)
OBJECTS=$(SOURCES:.c=.o)

all : $(OBJECTS)
	g++ $(CFLAGS) $(OBJECTS) -lm -o ./src/atpg
%.o:  %.c  ${INCLUDES}
	g++ $(CFLAGS) $< -o $@
clean:
	rm -rf src/*.o src/atpg	sample_circuits/*.ckt.uf result/*.pat result
