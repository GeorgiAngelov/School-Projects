all: hw2
options= -std=c++0x -g -m64
hw2: hw2.o file_parser.o
	g++ ${options} hw2.o file_parser.o -O2 -o hw2 

hw2.o: hw2.cpp
	g++ ${options} -c hw2.cpp 

file_parser.o: file_parser.cpp file_parser.hpp
	g++ ${options} -c file_parser.cpp ${options}

clean:
	rm -rf *o hw2
