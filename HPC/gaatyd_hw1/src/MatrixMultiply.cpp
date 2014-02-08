#include "MatrixMultiply.hpp"

#include <exception>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/ublas/matrix_proxy.hpp>
#include <numeric>

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

	scottgs::FloatMatrix result(lhs.size1(),rhs.size2());
	unsigned int i,j,k,ii,jj,kk;
	const unsigned int row = lhs.size1();
	const unsigned int col = lhs.size2();
	const unsigned int row2 = rhs.size1();
	const unsigned int col2 = rhs.size2();
	
	const unsigned int my_size = row2*col2;
	const unsigned int matrix1_size = row*col;
	const int block_size = 30;
	
	std::vector<float> transposed;
	std::vector<float> matrix1_vector;
	std::vector<float> result_vector;
	
	transposed.resize(my_size);
	matrix1_vector.resize(matrix1_size);
	result_vector.resize(row*col2);
	
	//transpose matrix 2 and copy the vlaues into a vector
	for (i = 0; i < col2; ++i)
		for (j = 0; j < row2; ++j)
			transposed[i*row2 + j] = rhs(j,i);
	
	//copy the values of matrix 1 into a vector
	for (i = 0; i < row; ++i)
		for (j = 0; j < col; ++j)
			matrix1_vector[i*col + j] = lhs(i,j);
	
	//calculate the matrix multiplication
	/*for (i = 0; i < row; ++i)
		for (j = 0; j < col2; ++j)
			for (k = 0; k < col; ++k)
				result_vector[i*col2 + j] += matrix1_vector[i*col+k] * transposed[j*col+k];*/
				
	for (ii = 0; ii < row; ii+=block_size){
		for (jj = 0; jj < col2; jj+=block_size){
			for (kk = 0; kk < col; kk+=block_size){
				for (i = ii; i < std::min(row, ii+block_size); ++i){
					for (j = jj; j < std::min(col2, jj+block_size); ++j){
						for (k = kk; k < std::min(col, kk+block_size); ++k){
							result_vector[i*col2 + j] += matrix1_vector[i*col+k] * transposed[j*col+k];
						}
					}
				} 
			}
		}
	}

	//add the values of the result vector back into the result matrix
	//that the function is expecting
	for (i = 0; i < row; ++i)
		for (j = 0; j < col2; ++j)
			result(i,j) = result_vector[i*col2 + j];
	
	return result;
}

scottgs::FloatMatrix scottgs::MatrixMultiply::multiply(const scottgs::FloatMatrix& lhs, const scottgs::FloatMatrix& rhs) const
{
	// Verify acceptable dimensions
	if (lhs.size2() != rhs.size1())
		throw std::logic_error("matrix incompatible lhs.size2() != rhs.size1()");

	return boost::numeric::ublas::prod(lhs,rhs);
}

