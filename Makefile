
OBJS_LIB = $(shell ls src/*.cpp | sed 's/\.cpp/.o/')
PROGS    = $(shell ls src/*.C | sed 's/\.C//' | sed 's/src\///')
INCLUDES = -I./include -I$(shell root-config --incdir) -I${DK2NU}/include
DEPLIBS  = $(shell root-config --libs) -lEG

CC  	 = g++
COPTS 	 = -fPIC -DLINUX -O0  -g $(shell root-config --cflags) 
FLAGS    = -g

all:    lib programs 

lib: libtgtexit.so 

libtgtexit.so: $(OBJS_LIB)
	if [ ! -d lib ]; then mkdir -p lib; fi

	$(CC) -shared -o lib/$@ $^ -L${DK2NU_LIB} -ldk2nuTree

programs: $(PROGS)
	echo making $(PROGS)

$(PROGS): % : src/%.o $(OBJS_LIB)  libtgtexit.so
	if [ ! -d bin ]; then mkdir -p bin; fi

	$(CC) -Wall -o bin/$@ $< $(DEPLIBS) -L$(WORKDIR)/lib -ltgtexit -L${DK2NU_LIB} -ldk2nuTree

%.o: %.cpp
	$(CC) $(COPTS) $(INCLUDES) -c -o $@ $<

%.o: %.C
	$(CC) $(COPTS) $(INCLUDES) -c -o $@ $<

clean:  delobj dellib delbin

delobj:
	-rm src/*.o

dellib:
	if [ -d lib ]; then rm -rf lib; fi 		

delbin:
	if [ -d bin ]; then rm -rf bin; fi

