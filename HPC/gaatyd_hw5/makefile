all: hw5
options= -std=c++0x -g -m64
hw5: hw5.o file_parser.o Splitter.o
	g++ ${options} hw5.o file_parser.o Splitter.o -O2 -o hw5 

hw5.o: hw5.cpp
	g++ ${options} -c hw5.cpp 

file_parser.o: file_parser.cpp file_parser.hpp
	g++ ${options} -c file_parser.cpp
	
Splitter.o: Splitter.cpp Splitter.hpp
	g++ ${options} -c Splitter.cpp
	
main.o: main.cpp
	mpic++ main.cpp
	
clean:
	rm -rf *o hw5
