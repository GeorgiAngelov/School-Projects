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

	//create the matrix
	scottgs::FloatMatrix result(lhs.size1(),rhs.size2());
	
	//create the unsigned ints used for the three for loops
	unsigned int i,j,k,ii,jj,kk; //matrices indexes
	
	//get the sizes I need and put them into a constant
	const unsigned int m1_num_row = lhs.size1(); //# of row of matrix 1
	const unsigned int m1_num_col = lhs.size2(); //# of col of matrix 1
	const unsigned int m2_num_row = rhs.size1(); //# of row of matrix 2
	const unsigned int m2_num_col = rhs.size2(); //# of column of matrix 2
	
	//block size calculation
	//2 * (blockSize)^2 * 4 = 32768 (L1 cache)
	//SQRT(32768/8) = blockSize = 64
	
	const int block_size = 64;
	
	//get a reference of the matrix's first element ( this will be a pointer to the first element )
	const float *m1 = &lhs(0,0);
	
	//malloc memory for the soon to be transposed matrix ( matrix #2)
	//float *transposed = (float*)malloc(sizeof(float)*m2_num_col*m2_num_row);
	std::vector<float> transposed;
	transposed.resize(m2_num_col*m2_num_row);
	
	//get a copy of the first element.
	float *r = &result(0,0);
	
	//transpose the second matrix
	for (j = 0; j < m2_num_col; ++j)
		for (i = 0; i < m2_num_row; ++i)
			transposed[j*m2_num_row + i] = m1[i*m2_num_col + j];
	
	if(m1_num_row > 100 || m1_num_col > 100 || m2_num_row > 100 || m2_num_col > 100){
		for (i = 0; i < m1_num_row; ++i)
			//loop through each column of matrix 2
			for (j = 0; j < m2_num_col; ++j)
				//loop through each column of matrix 1
				for (k = 0; k < m1_num_col; ++k)
					r[i*m2_num_col + j] = r[i*m2_num_col + j] + m1[i*m1_num_col + k] * transposed[j*m1_num_col + k];
	}else{
		//loop through each row of matrix 1
		for (ii = 0; ii < m1_num_row; ii+=block_size)
			//loop through each column of matrix 2
			for (jj = 0; jj < m2_num_col; jj+=block_size)
				//loop through each column of matrix 1
				for (kk = 0; kk < m1_num_col; kk+=block_size)
					for (i = ii; i < std::min(m1_num_row, ii+block_size); ++i)
						//loop through each column of matrix 2
						for (j = jj; j < std::min(m2_num_col, jj+block_size); ++j)
							//loop through each column of matrix 1
							for (k = kk; k < std::min(m1_num_col, kk+block_size); ++k)
								r[i*m2_num_col + j] = r[i*m2_num_col + j] + m1[i*m1_num_col + k] *transposed[j*m1_num_col + k];
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

