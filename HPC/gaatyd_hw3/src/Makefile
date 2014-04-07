
MY_FLAGS= -fopenmp -DNDEBUG -DNBOOST_DEBUG -O4 -funroll-loops -ffast-math -march=native -mfpmath=sse

# Standard all target
all: hw1

# HW 1
hw1: hw1.o MatrixMultiply.o
	g++ ${MY_FLAGS} -o hw1 hw1.o MatrixMultiply.o

hw1.o:	hw1.cpp 
	g++ ${MY_FLAGS} -Wall -c hw1.cpp

MatrixMultiply.o : MatrixMultiply.hpp MatrixMultiply.cpp
	g++ ${MY_FLAGS} -Wall -c MatrixMultiply.cpp

clean:
	rm -f *.o hw1


