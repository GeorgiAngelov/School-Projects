#include <iostream>
#include <vector>
#include <boost/assign/std/vector.hpp>

float dot_product(const std::vector<float>& one, const std::vector<float>& two);

int main (int argc, char * argv[])
{

	// Build up the test vectors
	std::vector<float> a;
	std::vector<float> b;
	{
		using namespace boost::assign;
		a += 1,2,3,4,5,6,7,8;
		b += 8,7,6,5,4,3,2,1;
	}

	float dp = dot_product(a,b);
	
	//std::cout << "Dot Product: " << dp << std::endl;
	
}
