#include "MatrixMultiply.hpp"

#include <exception>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <numeric>

#include <vector>
#include <stdexcept>

#include <omp.h>
#include <iostream>

#include <pthread.h>

//the matrix structure
//used for matrix A,B,C
typedef struct{
	unsigned int row;
	unsigned int col;
	const float *matrix;
}mat_struct;

//define 3 pointers to structure type matrix
//A and B are the matrices read in
//C is the matrix of multiplying AxB
mat_struct *m1;
mat_struct *m2;

//array for the result of the matrix multiplication
float *result;
unsigned int thread_count;

scottgs::MatrixMultiply::MatrixMultiply() 
{
	;
}

scottgs::MatrixMultiply::~MatrixMultiply()
{
	;
}

/**
*	The function is used for the threads to compute
*	the product for an individual ROW of matrix C
*	The row number for each thread is passed using void* t.
*	The function also prints the thread id.
*/
void* matrix_thread_row(void *t){
	//cast the pointer as an int pointer
	//and dereference it to get the value passed.
	//THIS IS THE CURRENT ROW FOR THE THREAD!
	unsigned int i = *((unsigned int*)t);
	unsigned int j=0,k=0;
	
	//pthread_t variable to print the id of the thread
	pthread_t tid = pthread_self();
	
	//print the thread is and the row that it is going to work on
	std::cout << "Created worker thread %u for row %d\n" << std::cout << (unsigned int)tid << i;
	float temp_sum = 0;
	//inner loop runs from 0 to number of columns in matrix m2
	for(j=0; j < m2->col; j++){
		temp_sum = 0;
		for(k=0; k< m1->col; k++){
			temp_sum = temp_sum + m1->matrix[i*m1->col + k] * m2->matrix[k*m2->col + j];
		}
		result[i*m2->col + j] = temp_sum;
	}
	
	//complete the thread
	pthread_exit(NULL);
}


scottgs::FloatMatrix scottgs::MatrixMultiply::operator()(const scottgs::FloatMatrix& lhs, const scottgs::FloatMatrix& rhs) const
{
	// Verify acceptable dimensions
	if (lhs.size2() != rhs.size1())
		throw std::logic_error("matrix incompatible lhs.size2() != rhs.size1()");

	m1 = (mat_struct*)malloc(sizeof(mat_struct));
	m2 = (mat_struct*)malloc(sizeof(mat_struct));
	
	//create the unsigned ints used for the three for loops
	unsigned int i; //matrices indexes
	
	//get the sizes I need and put them into a constant
	m1->row = lhs.size1(); //# of row of matrix 1
	m1->col = lhs.size2(); //# of col of matrix 1
	m2->row = rhs.size1(); //# of row of matrix 2
	m2->col = rhs.size2(); //# of column of matrix 2
	
	scottgs::FloatMatrix return_result(m1->row,m2->col);
	
	//MATRIX C IS THE MATRIX THAT IS THE PRODUCT OF AxB
	result = &return_result(0,0);
	
	//get a reference of the matrix's first element ( this will be a pointer to the first element )
	m1->matrix = &lhs(0,0);
	m2->matrix = &rhs(0,0);
	
	//create a double :pointer of type pthread_t(an array of pthread_t variables)
	//that will store all the threads so we can easily traverse them one by one to join them later
	//we make the same amount of threads as the amount of rows in the new matrix(hence A->i)
	pthread_t** threads = (pthread_t**) malloc(sizeof(pthread_t*)*m1->row);
	
	//an array of rows to specify which row the thread is going to execute over.
	//this int pointer(int array) will hold the number for each index A[0] = 0, A[1] = 1
	//and so fort just so we can pass it by address to the threading function
	int* rows = (int*) malloc(sizeof(int)*m1->row);
	//variable used to see if the thread was created
	int created = 0;
	//get number of cores on the current system
	unsigned int numCPU = sysconf( _SC_NPROCESSORS_ONLN );
	//if the requested number of threads is larger than the max number of cores
	//then only use max cores.
	thread_count = NUM_THREADS;
	if(numCPU < NUM_THREADS) 
		thread_count = numCPU;
		
	for (i = 0; i < thread_count; ++i){
		//allocate memory inside the threads array for each thread and cast the malloc
		threads[i] = (pthread_t*)malloc(sizeof(pthread_t));

		//insert the row number for this thread
		rows[i] = i;

		//create and execute the thread, pass it the thread pointer, NULL, the function to execute,
		//and a the address of the row number but cast it as void pointer as that is what the function expect
		created = pthread_create(threads[i], NULL, matrix_thread_row, (void*)&rows[i]);

		//if the value is negative then we have an error creating the thread
		if(created < 0){
			std::cout << "Thread failed to create\n";
		}
	}
	
	return return_result;
}

scottgs::FloatMatrix scottgs::MatrixMultiply::multiply(const scottgs::FloatMatrix& lhs, const scottgs::FloatMatrix& rhs) const
{
	// Verify acceptable dimensions
	if (lhs.size2() != rhs.size1())
		throw std::logic_error("matrix incompatible lhs.size2() != rhs.size1()");

	return boost::numeric::ublas::prod(lhs,rhs);
}

