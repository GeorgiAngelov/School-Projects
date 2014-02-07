# set this to what I want
OTHER_FLAGS=$(ENV_HPC_OPTS)

MY_FLAGS= -O3 -BOOST_NDEBUG -BOOST_UBLAS_NDEBUG

# Standard all target
all: hw1

# HW 1
hw1: hw1.o MatrixMultiply.o
	g++ ${MY_FLAGS} -o hw1 hw1.o MatrixMultiply.o

hw1.o:	hw1.cpp 
	g++ ${OTHER_FLAGS} ${MY_FLAGS} -Wall -c hw1.cpp

MatrixMultiply.o : MatrixMultiply.hpp MatrixMultiply.cpp
	g++ ${OTHER_FLAGS} ${MY_FLAGS} -Wall -c MatrixMultiply.cpp

clean:
	rm -f *.o hw1


