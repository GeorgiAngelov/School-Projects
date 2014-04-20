all: hw5
options= -lboost_filesystem-mt -std=c++0x -g -m64
hw5: hw5.o file_parser.o Splitter.o
	mpic++ ${options} hw5.o file_parser.o Splitter.o -O2 -o hw5 

hw5.o: hw5.cpp
	mpic++ ${options} -c hw5.cpp -DN_RESULTS=5

file_parser.o: file_parser.cpp file_parser.hpp
	g++ ${options} -c file_parser.cpp
	
Splitter.o: Splitter.cpp Splitter.hpp
	g++ ${options} -c Splitter.cpp
	
clean:
	rm -rf *o hw5
