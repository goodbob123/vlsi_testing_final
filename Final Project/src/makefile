#

CFLAGS = -std=c++11 -g  

# atpg.o main.o input.o level.o sim.o podem.o init_flist.o faultsim.o tdfsim.o display.o
SOURCES= $(wildcard *.cpp)
INCLUDES= $(wildcard *.h)
OBJECTS=$(SOURCES:.c=.o)

all : $(OBJECTS)
	g++ $(CFLAGS) $(OBJECTS) -lm -o atpg
%.o:  %.c  ${INCLUDES}
	g++ $(CFLAGS) $< -o $@
clean:
	rm *.o atpg	
