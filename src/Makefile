CXX           = icpc
CC            = icpc
CXXFLAGS      = -O3 \
                -qopenmp \
                -D_LINUX \
                -D_USE_OMP \
                -DNDEBUG 
DEST          = ./
OBJS          = main.o \
                MeshData.o \
                MeshDataTetraElement.o \
                ResistivityBlock.o \
                Util.o
PROGRAM       = changeResistivity

all:            $(PROGRAM)

$(PROGRAM):     $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) $(LDFLAGS) $(LIBS) -o $(PROGRAM)

clean:;		rm -f *.o *~ $(PROGRAM)
