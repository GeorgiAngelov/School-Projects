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
	unsigned int i; //matrix 1 column index
	unsigned int j; //matrix 1 row index
	unsigned int k; //matrix 2 column index
	
	//get the sizes I need and put them into a constant
	const unsigned int m1_num_row = lhs.size1(); //# of row of matrix 1
	const unsigned int m1_num_col = lhs.size2(); //# of col of matrix 1
	const unsigned int m2_num_row = rhs.size1(); //# of row of matrix 2
	const unsigned int m2_num_col = rhs.size2(); //# of column of matrix 2
	
	//get a reference of the matrix's first element ( this will be a pointer to the first element )
	const float *m1 = &lhs(0,0);
	const float *m2 = &rhs(0,0);
	
	//malloc memory for the soon to be transposed matrix ( matrix #2)
	float *transposed = (float*)malloc(sizeof(float)*m2_num_col*m2_num_row);
	
	std::cout << "\nBefore\n";
	//loop through each row of matrix 1
	for (i = 0; i < m2_num_row; ++i){
		//loop through each column of matrix 2
        for (j = 0; j < m2_num_col; ++j){
			std::cout << *(m2 + i*m2_num_col + j) << " ";
		}
		std::cout << "\n";
	}
	
	std::cout << "After\n";
	
	//transpose the second matrix
	for (j = 0; j < m2_num_col; ++j){
		for (i = 0; i < m2_num_row; ++i){
			*(transposed + j*m2_num_row + i) = *(m1 + i*m2_num_col + j);
			std::cout << *(transposed + j*m2_num_row + i) << " ";
		}
		std::cout << "\n";
	}
	
	//get a copy of the first element.
	float *r = &result(0,0);
	
	//loop through each row of matrix 1
	for (i = 0; i < m1_num_row; ++i)
		//loop through each column of matrix 2
        for (j = 0; j < m2_num_col; ++j)
			//loop through each column of matrix 1
			for (k = 0; k < m1_num_col; ++k)
				*(r + i*m2_num_col + j) += *(m1 + i*m1_num_col + k) * *(m2 + k*m2_num_col + j);
				
	return result;
}

scottgs::FloatMatrix scottgs::MatrixMultiply::multiply(const scottgs::FloatMatrix& lhs, const scottgs::FloatMatrix& rhs) const
{
	// Verify acceptable dimensions
	if (lhs.size2() != rhs.size1())
		throw std::logic_error("matrix incompatible lhs.size2() != rhs.size1()");

	return boost::numeric::ublas::prod(lhs,rhs);
}

