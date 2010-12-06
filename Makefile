CXX		= g++
CXXFLAGS	= -g -W -Wall -Wwrite-strings -Wpointer-arith -Wimplicit \
		  -Wcast-qual -Winline -Wmissing-noreturn -Wsign-compare
LDFLAGS		= -g

all: main

clean:
	rm -f *.o main core

main: main.o file.o tokenize.o iscparser.o configdata.o
	$(CXX) $(LDFLAGS) -o $@ $^ -lstdc++

file.o: file.cc file.hh
iscparser.o: iscparser.cc iscparser.hh tokenize.hh file.hh configdata.hh
main.o: main.cc tokenize.hh file.hh iscparser.hh configdata.hh
mmap.o: mmap.cc mmap.hh
tokenize.o: tokenize.cc tokenize.hh file.hh
configdata.o: configdata.cc configdata.hh

