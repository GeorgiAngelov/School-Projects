all: hw0
options= -std=c++0x -g
hw0: hw0.o file_parser.o
	g++ hw0.o file_parser.o -O2 -o hw0 ${options}

hw0.o: hw0.cpp
	g++ -c hw0.cpp ${options}

file_parser.o: file_parser.cpp file_parser.h
	g++ -c file_parser.cpp ${options}

clean:
	rm -rf *o hw0
