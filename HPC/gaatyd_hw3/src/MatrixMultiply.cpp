#include "MatrixMultiply.hpp"

#include <exception>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <numeric>

#include <vector>
#include <stdexcept>

#include <omp.h>
#include <iostream>



scottgs::MatrixMultiply::MatrixMultiply() 
{
	;
}

scottgs::MatrixMultiply::~MatrixMultiply()
{
	;
}


scottgs::FloatMatrix scottgs::MatrixMultiply::operator()(const scottgs::FloatMatrix& lhs, const scottgs::FloatMatrix& rhs) const
{
	// Verify acceptable dimensions
	if (lhs.size2() != rhs.size1())
		throw std::logic_error("matrix incompatible lhs.size2() != rhs.size1()");

	//create the matrix
	scottgs::FloatMatrix result(lhs.size1(),rhs.size2());
	
	//create the unsigned ints used for the three for loops
	unsigned int i,j,k; //matrices indexes
	
	//get the sizes I need and put them into a constant
	const unsigned int m1_num_row = lhs.size1(); //# of row of matrix 1
	const unsigned int m1_num_col = lhs.size2(); //# of col of matrix 1
	const unsigned int m2_num_row = rhs.size1(); //# of row of matrix 2
	const unsigned int m2_num_col = rhs.size2(); //# of column of matrix 2
	
	//get a reference of the matrix's first element ( this will be a pointer to the first element )
	const float *m1 = &lhs(0,0);
	const float *m2 = &rhs(0,0);
	float temp_sum = 0;
	//get a copy of the first element.
	float *r = &result(0,0);
	
	float* transposed = (float*) malloc(sizeof(float)*m2_num_col * m2_num_row);
	
	//transpose the second matrix
	for (j = 0; j < m2_num_col; ++j){
		for (i = 0; i < m2_num_row; ++i){
			transposed[j*m2_num_row + i] = m2[i*m2_num_col + j];
		}
	}

	#pragma omp parallel for schedule(dynamic, 4)
	for (i = 0; i < m1_num_row; ++i){
		const float* m1_row = m1 + i*m1_num_col;
		float* const r_row = r + i*m2_num_col;
		//loop through each column of matrix 2
		for (j = 0; j < m2_num_col; ++j){
			const float* m2_col = transposed + j*m1_num_col; 
			temp_sum = 0;
			//loop through each column of matrix 1
			for (k = 0; k < m1_num_col; ++k){
				temp_sum = temp_sum + m1_row[k] * m2_col[k];
			}
			r_row[j] = temp_sum;
		}
	}
	return result;
}

scottgs::FloatMatrix scottgs::MatrixMultiply::multiply(const scottgs::FloatMatrix& lhs, const scottgs::FloatMatrix& rhs) const
{
	// Verify acceptable dimensions
	if (lhs.size2() != rhs.size1())
		throw std::logic_error("matrix incompatible lhs.size2() != rhs.size1()");

	return boost::numeric::ublas::prod(lhs,rhs);
}

