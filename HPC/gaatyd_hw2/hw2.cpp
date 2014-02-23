/**
*	Name: Georgi Angelov
*	ID: 14120841
*	HW#: Homework 2
*	Date: 2/21/2014
*/

#include <iostream>
#include <chrono>
#include <vector>
#include <math.h>
#include <algorithm>
#include "file_parser.hpp"
#include "vectorCSV.hpp"

struct ResultType {
	float x; float y; int offset; float dist;
	
	//boolean operator for this struct so we can sort it using std::sort();
	bool operator < (const ResultType& obj) const
	{
		return (dist < obj.dist);
	}
};

std::vector<float> generateRandomVector(unsigned int size)
{
	// Again, there is a better way to do this using STL generator or STL bind
	std::vector<float> rv(size, 0);
	for (std::vector<float>::iterator i=rv.begin(); i!=rv.end(); ++i)
		*i =  ((static_cast<float>(rand()) / RAND_MAX) * 2.0) - 1.0;

	return rv;
}

/**
*	The function computes the difference between a search vector and a segment of a circular vector.
*	@vector_size - the size of the segment we are comparing with
*	@searchVector - the generated vector which we use to subtract each point from our circular vector
*	@circularVector - a vector of x,y,points pairs where points is a 360 degree vector
*	@n - number of top results to return
*	@all_offest - value used to indicate from where to begin the search.
*/
std::vector<ResultType> circularSubvectorMatch(const unsigned int vector_size, std::vector<float> searchVector, VectorsMap circularVector, const unsigned int n, const unsigned int all_offset){
	unsigned int i=0;
	unsigned int j;
	//72 because this is how many distances we will get per vector.
	const unsigned int vector_space = circularVector.size()*72;
	//vector for the returned top N results;
	std::vector<ResultType> results;
	results.reserve(vector_space);
	
	//iterator for the circularVector
	VectorsMap::iterator iter = circularVector.begin();;
	unsigned int count = 0;
	
	//iterate over the whole set of vectors parsed from the file.
	for (iter; iter != circularVector.end(); iter++) {
		count++;
		//get the x y pair
		XYPair pair = iter->first;
		//get the first and second key(x and y)
		float x = pair.first;
		float y = pair.second;
		
		//run through every vector point at steps of 5
		for(i=0; i<VECTOR_COUNT; i+=5){
			float dist = 0;
			int offset = i;
			
			//loop through the vector size(9,11,17,29)
			for(j=0; j<vector_size; j++){
				const float dist_tmp = fabs(iter->second.at((j+i)%360) - searchVector.at(j));
				dist += dist_tmp;
			}
			
			//put the result into the structure
			ResultType one;
			one.x = x;
			one.y = y;
			one.offset = i;
			one.dist = dist;
			results.push_back(one);
		}
	}
	return results; 
}

void printResults(std::vector<ResultType> results, const unsigned int n){
	unsigned int i=0;
	
	//sort the results.
	std::sort(results.begin(), results.end());
	std::cout << "\tx" << "\t|" << "\ty" << "\t|" << "\toffset" << "\t|" << "\tscore" << std::endl;
	std::cout << "---------+---------+--------+-----------" << std::endl;
	//print the results
	for(i; i<n; i++){
		ResultType res = results.at(i);
		std::cout << res.x << "\t|" << res.y << "\t|" << res.offset << "\t|" << res.dist << std::endl;
	}
	std::cout << "(" << n << " rows)" << std::endl;
}

std::vector<float> generateScottVector(const unsigned int size){
	int i=0;
	float array1[] = {-0.0536727,0.0384691,0.00146231,0.0122459,0.0198738,-0.116341,0.0998519,0.0269831,-0.000772231};
	float array2[] = {0.0572175,-0.139987,-0.143134,-0.0428729,0.118296,0.0105897,0.0302701,-0.134377,-0.0855214,-0.0757894,0.140506};
	float array3[] = {0.0416003,0.0107126,0.0120675,-0.00547709,-0.00533309,0.00230428,0.00302696,0.0717615,0.0265672,-0.0319207,0.0132625,-0.0669076,-0.0196825,0.0726006,-0.0277976,0.0813408,-0.0819924};
	float array4[] = {0.0208535,0.00124793,0.0221286,-0.0251518,-0.00935498,0.000144995,0.0543099,0.152197,-0.0436211,-0.0536875,-0.0389395,-0.163685,0.105508,0.0135837,-0.0582674,-0.0525398,0.107217,-0.0480279,0.00522108,0.0145284,0.10181,-0.19426,0.0345067,0.126417,-0.143776,0.125843,-0.0239083,0.0613458,-0.265121};
	
	std::vector<float> return_vector;
	
	switch(size){
		case 9:
			return_vector.reserve(9);
			for(i; i<9; i++)
				return_vector.push_back(array1[i]);
			return return_vector;
		case 11:
			return_vector.reserve(11);
			for(i; i<11; i++)
				return_vector.push_back(array2[i]);
			return return_vector;
		case 17:
			return_vector.reserve(17);
			for(i; i<17; i++)
				return_vector.push_back(array3[i]);
			return return_vector;
		case 29:
			return_vector.reserve(29);
			for(i; i<29; i++)
				return_vector.push_back(array4[i]);
			return return_vector;
		default:
		return return_vector;
	}
}

int main (int argc, char** argv){
	if(argc < 4){
		std::cout << "3 command line arguments are needed.\nPlease execute with ./hw0 <input_filename> <process #> <number of match list>\n";
		return 0;
	}
	
	srand(34122);
	//holds the results from parsing the file.
	VectorsMap points;
	//iterator for the items from the file.
	VectorsMap::iterator iter;
	int i,j, process_count=atoi(argv[2]), num_max=atoi(argv[3]), size=0;
	//the size of the search vectors
	int sizes[] = {9,11,17,29};
	int sizes_count = 4;
	
	//create the vector that will hold all the results from each vector before combining them
	std::vector<std::vector<ResultType>> results;
	results.reserve(process_count);
	
	//create the final results vector
	//reserve enough space to be able to merge all of our processes' stuff
	std::vector<ResultType> final_results;
	final_results.reserve(process_count*num_max);

	//create start and end chrono time points
	std::chrono::time_point<std::chrono::system_clock> start, end;
	std::cout << process_count << std::endl;
	
	//create a parser object and pass it the filename
	//and parse the file 
	Parser fileParser(argv[1]);
	points = fileParser.parseFile();	

	//print header.
	std::cout << "===========" << std::endl;
	std::cout << " Put Data Set Count Here " << std::endl;
	std::cout << "===========" << std::endl;
	
	//loop through the 4 different sizes of vectors
	for(i=0; i<sizes_count; i++){
		std::cout << "-----------------" << std::endl;
		std::cout << "Search: "<< sizes[i] << "-D" << std::endl;
		std::cout << "-----------------" << std::endl;
		for(j=0; j<1; j++){
		    start = std::chrono::system_clock::now();
			
			//generate random vector of appropriate size
			//std::vector<float> copy = generateRandomVector(sizes[i]);
			
			//get the test vector from Grant's example:
			std::vector<float> copy = generateScottVector(sizes[i]);
			
			//print the created vector.
			std::cout << scottgs::vectorToCSV(copy) << std::endl;
			std::cout << "total circular vectors passed is: " << points.size() << std::endl;
			//perform the test(delete this as it is not needed)
			final_results = circularSubvectorMatch(sizes[i], copy, points, num_max, 0);
			
			//print top num_max results
			printResults(final_results, num_max);
			
			//calculate end time.
			end = std::chrono::system_clock::now();
			std::chrono::duration<double> elapsed_seconds = end-start;
			std::cout << "\nTime: " << elapsed_seconds.count() << " seconds" << std::endl;
		}
	}
}