EXECUTABLES = arc
CC = g++
#OBJS_FLAGS  = -pg -O3 -Wall -ansi -m386 
#CXXFLAGS    = -pg -O3 -Wall -ansi -m386 
OBJS_FLAGS  = -O3 -Wall -ansi -m386 
CXXFLAGS    = -O3 -Wall -ansi -m386 
OBJS = main.o kineticMC.o site.o path.o particle.o event.o
kmc : $(OBJS)
	$(CC) -o $(EXECUTABLES) $(OBJS) $(CXXFLAGS) -lm -static
main.o:      kineticMC.h
kineticMC.o: kineticMC.h site.h path.h event.h particle.h
site.o:      site.h
path.o:      path.h site.h
particle.o:  particle.h site.h
event.o:     event.h particle.h site.h
common.h:    constants.h functions.h
$(OBJS) : common.h 
clean:
	-rm -f *.o *~
	-rm -f ${EXECUTABLES}

