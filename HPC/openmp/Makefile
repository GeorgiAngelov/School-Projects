all: omp_dp_test

omp_dp_test: dot_product_omp.o use_openmp_dot_product.o
	g++ -fopenmp -o omp_dp_test dot_product_omp.o use_openmp_dot_product.o

dot_product_omp.o: dot_product_omp.cpp
	g++ -Wall -fopenmp -c dot_product_omp.cpp

use_openmp_dot_product.o: use_openmp_dot_product.cpp
	g++ -Wall -c use_openmp_dot_product.cpp

clean:
	rm omp_dp_test dot_product_omp.o use_openmp_dot_product.o

