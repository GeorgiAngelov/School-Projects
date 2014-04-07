
MY_FLAGS= -fopenmp -DNDEBUG -DNBOOST_DEBUG -O4 -funroll-loops -ffast-math -march=native -mfpmath=sse -lpthread

# Standard all target
all: hw4

# HW 1
hw4: hw4.o MatrixMultiply.o
	g++ ${MY_FLAGS} -o hw4 hw4.o MatrixMultiply.o

hw4.o:	hw4.cpp 
	g++ ${MY_FLAGS} -Wall -c hw4.cpp

MatrixMultiply.o : MatrixMultiply.hpp MatrixMultiply.cpp
	g++ ${MY_FLAGS} -Wall -c MatrixMultiply.cpp -DNUM_THREADS=8 -DPARTITION_METHOD=2

clean:
	rm -f *.o hw4

