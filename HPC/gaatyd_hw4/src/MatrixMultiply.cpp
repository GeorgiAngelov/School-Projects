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

//The container struct
//holds the vars that we will
//be passed to the functions
typedef struct{
	mat_struct *m1;
	mat_struct *m2;
	unsigned int i;
	unsigned int start_block;
	unsigned int end_block;
	float *result;
	unsigned int thread_count;
	pthread_mutex_t* mutex_position; //used for interleaved example
	pthread_mutex_t* mutex_result; //used for interleaved example
	unsigned int* counter; //used for interleaved example
}container_struct;

scottgs::MatrixMultiply::MatrixMultiply() 
{
	;
}

scottgs::MatrixMultiply::~MatrixMultiply()
{
	;
}

/**
*	The function performs the two innermost loops of the matrix multiplication.
*	It receives the row to be processed and the container holding all the information.
*/
void matrixMultiplyInner(const unsigned int row, container_struct* container){
	float temp_sum = 0;
	unsigned int j=0,k=0;
	for(j=0; j < container->m2->col; j++){
		temp_sum = 0;
		for(k=0; k < container->m1->col; k++){
			temp_sum = temp_sum + 
			container->m1->matrix[row*container->m1->col + k] * 
			container->m2->matrix[k*container->m2->col + j];
		}
		container->result[row*container->m2->col + j] = temp_sum;
	}
}

/**
*	The function is using a thread to compute the 
*	dot product of two matrices with a CELL interleaved technique using MUTEX.
*/
void* matrix_thread_interleaved(void *t){
	container_struct* container = (container_struct*)t;
	unsigned int result_size = container->m1->row * container->m2->col;
	unsigned int counter = 0;
	unsigned int row = 0;
	unsigned int col = 0;
	unsigned int k = 0;
	float temp_sum = 0;
	while(1){
		/* try to lock(or wait until unlocked) the mutex_position */
		pthread_mutex_lock(container->mutex_position);
		if(*(container->counter) >= result_size){
			pthread_mutex_unlock(container->mutex_position);
			break;
		}
		counter = *(container->counter);
		*(container->counter)+=1;
		/* unlock the mutex */
		pthread_mutex_unlock(container->mutex_position);
		
		//get the row and col values
		row = counter/container->m2->col;
		col = counter%container->m2->col;
		temp_sum = 0;
		//loop through the number of columns in m1
		for(k=0; k<container->m1->col; k++){
			temp_sum = temp_sum + 
			container->m1->matrix[row*container->m1->col + k] * 
			container->m2->matrix[k*container->m2->col + col];
		}
		//lock the result matrix
		pthread_mutex_lock(container->mutex_result);
		container->result[row*container->m2->col + col] = temp_sum;
		pthread_mutex_unlock(container->mutex_result);
	}
	pthread_exit(NULL);
}

/**
*	The function is using a thread to compute the 
*	dot product of two matrices with a block of rows for each thread.
*	The void* t is actually a container_struct pointer containing the information for the thread,
*	Such as the dot product matrix, matrix1 and matrix2.
*/
void* matrix_thread_block(void *t){
	container_struct* container = (container_struct*)t;
	unsigned int i=container->i;
	
	unsigned count = 0;
	unsigned int row = count*container->thread_count + i;
	count++;
	//let the thread loop through the row's it's supposed to run through
	for(row=container->start_block; row < container->end_block; row++){
		matrixMultiplyInner(row, container);
		count++;
	}
	
	//complete the thread
	pthread_exit(NULL);
}

/**
*	The function is using a thread to compute the 
*	dot product of two matrices with a step of total threads for each thread.
*	The void* t is actually a container_struct pointer containing the information for the thread,
*	Such as the dot product matrix, matrix1 and matrix2.
*/
void* matrix_thread_row(void *t){
	container_struct* container = (container_struct*)t;
	unsigned int i=container->i;
	
	unsigned count = 0;
	unsigned int row = count*container->thread_count + i;
	count++;
	//let the thread loop through the row's it's supposed to run through
	while(row < container->m1->row){
		//inner loop runs from 0 to number of columns in matrix m2
		matrixMultiplyInner(row, container);
		//find the row that the thread is working on
		row = count*container->thread_count + i;
		count++;
	}
	
	//complete the thread
	pthread_exit(NULL);
}

scottgs::FloatMatrix scottgs::MatrixMultiply::operator()(const scottgs::FloatMatrix& lhs, const scottgs::FloatMatrix& rhs) const
{
	// Verify acceptable dimensions
	if (lhs.size2() != rhs.size1())
		throw std::logic_error("matrix incompatible lhs.size2() != rhs.size1()");
	mat_struct* m1 = (mat_struct*)malloc(sizeof(mat_struct));
	mat_struct* m2 = (mat_struct*)malloc(sizeof(mat_struct));
	
	//create the unsigned ints used for the three for loops
	unsigned int i; //matrices indexes
	
	//get the sizes I need and put them into a constant
	m1->row = lhs.size1(); //# of row of matrix 1
	m1->col = lhs.size2(); //# of col of matrix 1
	m2->row = rhs.size1(); //# of row of matrix 2
	m2->col = rhs.size2(); //# of column of matrix 2
	
	scottgs::FloatMatrix return_result(m1->row,m2->col);
	
	//MATRIX C IS THE MATRIX THAT IS THE PRODUCT OF AxB
	float* result = &return_result(0,0);
	
	//get a reference of the matrix's first element ( this will be a pointer to the first element )
	m1->matrix = &lhs(0,0);
	m2->matrix = &rhs(0,0);
	//variable used to see if the thread was created
	int created = 0;
	//create a function pointer so we can easily change the different partitoin methods
	void* (*method_func)(void*);
	unsigned int thread_count = NUM_THREADS;
	pthread_mutex_t* mutex_position = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
	pthread_mutex_t* mutex_result = (pthread_mutex_t*)malloc(sizeof(pthread_mutex_t));
	/* initialize mutex */
	pthread_mutex_init(mutex_position, NULL);
	pthread_mutex_init(mutex_result, NULL);
	
	//PARTITION_METHOD is a global variable that passed throuugh the Makefile.
	switch(PARTITION_METHOD){
		case 0:
			method_func = &matrix_thread_row;
		break;
		case 1:
			method_func = &matrix_thread_block;
			if(m1->row < thread_count)
				thread_count = m1->row;
		break;
		case 2:
			method_func = &matrix_thread_interleaved;
		break;
	}
	
	unsigned int step = m1->row/thread_count;
	unsigned int extra = m1->row%thread_count;
		
	//this will hold all the pthreads created
	pthread_t** threads = (pthread_t**) malloc(sizeof(pthread_t*)*thread_count);
	container_struct** container = (container_struct**)malloc(sizeof(container_struct*)*thread_count);
	//this will be used for the interleaved example(pointer so they can change the value across all);
	unsigned int* counter = (unsigned int*) malloc(sizeof(unsigned int));
	*counter = 0;
	
	//Spawn all the threads
	for (i = 0; i < thread_count; ++i){
		
		//allocate memory inside the threads array for each thread and cast the malloc
		threads[i] = (pthread_t*)malloc(sizeof(pthread_t));
		container[i] = (container_struct*)malloc(sizeof(container_struct));	
		
		//collect all the data that is to be passed to the thread
		container[i]->m1 = m1;
		container[i]->m2 = m2;
		container[i]->thread_count = thread_count;
		container[i]->i = i;
		container[i]->result = result;
		container[i]->start_block = i*step;
		container[i]->end_block = container[i]->start_block + step;
		container[i]->mutex_position = mutex_position;
		container[i]->mutex_result = mutex_result;
		container[i]->counter = counter;
		
		//if at last thread, add any extra rows left over
		if(i==(thread_count-1))
			container[i]->end_block += extra;
		
		//create the thread
		created = pthread_create(threads[i], NULL, method_func, (void*)container[i]);
		//if the value is negative then we have an error creating the thread
		if(created < 0){
			std::cout << "Thread failed to create\n";
		} 
	}
 
	//loop through the number of threads and join them(waiting for them to finish their work)
	for(i=0; i<thread_count; i++){
		//join the threads back one by one
		pthread_join(*threads[i], NULL);
	}
	
	/****CLEANUP****/
	for(i=0; i<thread_count; i++){
		//free the thread's structure
		free(threads[i]);
		free(container[i]);
	}
	//free the threads structure
	free(threads);
	free(container);
	free(mutex_position);
	free(mutex_result);
	return return_result;
}

scottgs::FloatMatrix scottgs::MatrixMultiply::multiply(const scottgs::FloatMatrix& lhs, const scottgs::FloatMatrix& rhs) const
{
	// Verify acceptable dimensions
	if (lhs.size2() != rhs.size1())
		throw std::logic_error("matrix incompatible lhs.size2() != rhs.size1()");

	return boost::numeric::ublas::prod(lhs,rhs);
}

