CXX           = icpc
CC            = icpc
CXXFLAGS      = -O3 \
                -D_LINUX \
                -DNDEBUG 
DEST          = ./
OBJS          = main.o \
                MeshData.o \
                MeshDataTetraElement.o \
                MeshDataNonConformingHexaElement.o \
                ResistivityBlock.o \
                ResistivityBlockIsotropic.o \
                ResistivityBlockAnisotropic.o \
                Util.o
PROGRAM       = changeResistivity

all:            $(PROGRAM)

$(PROGRAM):     $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) $(LDFLAGS) $(LIBS) -o $(PROGRAM)

clean:;		rm -f *.o *~ $(PROGRAM)
