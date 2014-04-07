# set this to what I want
OTHER_FLAGS=-std=c++0x -fopenmp -O4 -funroll-loops -ffast-math -march=native -mfpmath=sse -DNDEBUG -DNBOOST_DEBUG

# Standard all target
all: hw4_analyze

# HW 1 Analysis Code
hw4_analyze: hw4_analyze.o ../src/MatrixMultiply.o
	g++ ${OTHER_FLAGS} -o hw4_analyze hw4_analyze.o ../src/MatrixMultiply.o

hw4_analyze.o:	hw4_analyze.cpp 
	g++ ${OTHER_FLAGS} -Wall -c hw4_analyze.cpp

clean:
	rm -f *.o hw4_analyze p1.time p1.out


### Analysis Commands and Dependencies

analysis: data
	@echo "Generating Plots:" ; \
	(gnuplot < plot_hw4.gnu && \
	gnuplot < plot_hw4.logxy.gnu && \
	gnuplot < plot_hw4.logx.gnu && \
	gnuplot < plot_hw4.logy.gnu ) 2> /dev/null ; \
	echo "Program Execution Times" ; \
	cat p1.time

data: p1.out
	@echo "Scrapping Timing Statistics:"; ./analyze_1.sh

test_1: hw4_analyze
	@echo "Running hw4_analyze:"; (time ./hw4_analyze > p1.out) 2> p1.time

# this will always run the test_1 because the p1.out 
# always has a time stamp newer that the dependency of test_1
p1.out: test_1

