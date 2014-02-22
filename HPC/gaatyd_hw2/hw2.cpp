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

std::vector<ResultType> circularSubvectorMatch(const unsigned int vector_size, std::vector<float> searchVector, VectorsMap circularVector, const unsigned int n, const unsigned int all_ofset){
	unsigned int i=0;
	unsigned int j;
	//vector for the returned top N results;
	std::vector<ResultType> results;
	results.reserve(72);
	//iterator for the circularVector
	VectorsMap::iterator iter = circularVector.begin();;
	unsigned int count = 0;
	
	for (iter = circularVector.begin(); iter != circularVector.end(); iter++) {
		count++;
		for(i; i<VECTOR_COUNT; i+=5){
			float dist = 0;
			int offset = i;
			XYPair pair = iter->first;
			//get the first and second key(x and y)
			float x = pair.first;
			float y = pair.second;
			
			//loop through the vector size(7,9,11,17,29)
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

int main (int argc, char** argv){
	if(argc < 4){
		std::cout << "3 command line arguments are needed.\nPlease execute with ./hw0 <input_filename> <process #> <number of match list>\n";
		return 0;
	}
	
	srand(3412412);
	VectorsMap points;
	VectorsMap::iterator iter;
	int i,j, process_count=atoi(argv[2]), num_max=atoi(argv[3]), size=0;
	int sizes[] = {9,11,17,29};
	int sizes_count = 4;
	std::vector<ResultType> results;
	//create start and end chrono time points
	std::chrono::time_point<std::chrono::system_clock> start, end;
	std::cout << process_count << std::endl;
	//create a parser object and pass it the filename
	Parser fileParser(argv[1]);
	//parse the file 
	points = fileParser.parseFile();	
	//record the start time.

	//print header.
	std::cout << "===========" << std::endl;
	std::cout << " Put Data Set Count Here " << std::endl;
	std::cout << "===========" << std::endl;
	
	
	//loop through the 4 different sizes of vectors
	for(i=0; i<sizes_count; i++){
		std::cout << "-----------------" << std::endl;
		std::cout << "Search: "<< sizes[i] << "-D" << std::endl;
		std::cout << "-----------------" << std::endl;
		for(j=0; j<30; j++){
		    start = std::chrono::system_clock::now();
			//generate random vector of appropriate size
			std::vector<float> copy = generateRandomVector(sizes[i]);
			//print the created vector.
			std::cout << scottgs::vectorToCSV(copy) << std::endl;
			//perform the test(delete this as it is not needed)
			results = circularSubvectorMatch(sizes[i], copy, points, num_max, 0);
			printResults(results, num_max);
			
			//calculate end time.
			end = std::chrono::system_clock::now();
			std::chrono::duration<double> elapsed_seconds = end-start;
			std::cout << "\nTime: " << elapsed_seconds.count() << " seconds" << std::endl;
		}
	}
}