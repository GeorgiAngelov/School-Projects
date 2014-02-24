all: hw2
options= -std=c++0x -g -m64
hw2: hw2.o file_parser.o Splitter.o
	g++ ${options} hw2.o file_parser.o Splitter.o -O2 -o hw2 

hw2.o: hw2.cpp
	g++ ${options} -c hw2.cpp 

file_parser.o: file_parser.cpp file_parser.hpp
	g++ ${options} -c file_parser.cpp
	
Splitter.o: Splitter.cpp Splitter.hpp
	g++ ${options} -c Splitter.cpp
	
clean:
	rm -rf *o hw2
