#include <vector>
#include <stdexcept>

#include <omp.h>
#include <iostream>

float dot_product(const std::vector<float>& one, const std::vector<float>& two)
{
	
	unsigned int i;
	const unsigned int count=one.size();

	// Verify acceptable dimensions
	if (count != two.size())
		throw std::logic_error("vector incompatible one.size() != two.size()");

	std::cout << "Number of processors: " << omp_get_num_procs() << std::endl;
	

	const float * a = &(one.front());
	const float * b = &(two.front());
	float elem;
	float sum = 0;
	// forks off the threads and starts the work-sharing construct
	// Note: elem is now private to each thread
	// Note: Sum is defined as a reduction, 
	//	each threads local sum is added to the global sum variable
	//#pragma omp parallel for private(elem) reduction(+:sum) schedule(static,2)
	for(i = 0; i < count; i++)
	{
		std::cout << "I am thread: " << omp_get_thread_num() << std::endl;		
		
		elem = a[i] * b[i];
		sum = sum + elem;
	}
	return sum;
}

