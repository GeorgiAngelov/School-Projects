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
	unsigned int m1_c_i; //matrix 1 column index
	unsigned int m1_r_i; //matrix 1 row index
	unsigned int m2_c_i; //matrix 2 column index
	
	//get the sizes I need and put them into a constant
	const unsigned int m1_num_row = lhs.size1(); //# of row of result matrix
	const unsigned int m1_num_col = lhs.size2();
	const unsigned int m2_num_col = rhs.size2(); //# of column of result matrix
	
	//get a reference of the matrix's first element ( this will be a pointer to the first element )
	const float *m1 = &lhs(0,0);
	const float *m2 = &rhs(0,0);
	
	//get a copy of the first element.
	float *r = &result(0,0);
	
	//loop through each row of matrix 1
	for (m1_c_i = 0; m1_c_i < m1_num_row; ++m1_c_i)
		//loop through each column of matrix 2
        for (m1_r_i = 0; m1_r_i < m2_num_col; ++m1_r_i)
			//loop through each column of matrix 1
			for (m2_c_i = 0; m2_c_i < m1_num_col; ++m2_c_i)
				*(r + m1_c_i*m2_num_col + m1_r_i) += *(m1 + m1_c_i*m1_num_col + m2_c_i) * *(m2 + m2_c_i*m2_num_col + m1_r_i);
				
	return result;
}

scottgs::FloatMatrix scottgs::MatrixMultiply::multiply(const scottgs::FloatMatrix& lhs, const scottgs::FloatMatrix& rhs) const
{
	// Verify acceptable dimensions
	if (lhs.size2() != rhs.size1())
		throw std::logic_error("matrix incompatible lhs.size2() != rhs.size1()");

	return boost::numeric::ublas::prod(lhs,rhs);
}

