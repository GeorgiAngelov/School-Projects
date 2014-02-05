/**
*	Name: Georgi Angelov
*	ID: 14120841
*	HW#: Homework 0
*	Date: 1/30/2014
*/

#include <iostream>
#include "file_parser.h"

int main (int argc, char** argv){
	VectorsMap points;
	VectorsMap::iterator iter;
	int i;
	//create start and end chrono time points
	std::chrono::time_point<std::chrono::system_clock> start, end;
	
	if(argc == 1){
		std::cout << "No file was provided.\nPlease execute with ./hw0 <input_filename>\n";
		return 0;
	}
	
	//create a parser object and pass it the filename
	Parser fileParser(argv[1]);
	//parse the file 
	points = fileParser.parseFile();
	
	//record the start tiem.
    start = std::chrono::system_clock::now();
	
	//iterate over all vectors
	for (iter = points.begin(); iter != points.end(); iter++) {
		XYPair key = iter->first;
		fileParser.isMaxXY(key.first, key.second);
		int i=0;
		//loop through each vector'
		for(std::vector<float>::iterator it = iter->second.begin(); it != iter->second.end(); ++it) {
			fileParser.isMaxMinVector(*it, i);
			i++;
		}
	}
	
	//calculate end time.
	end = std::chrono::system_clock::now();
	//calculate duration
	std::chrono::duration<double> elapsed_seconds = end-start;
	//print info on total timel
	std::cout << "\nTotal time to find Min and Max : " << elapsed_seconds.count() << " seconds\n";
	
	float* vectors;
	XYPair xy_pair_min = fileParser.getMinXY();
	XYPair xy_pair_max = fileParser.getMaxXY();
	
	std::cout << "\nMin X: " << xy_pair_min.first << " Min Y: " << xy_pair_min.second << "\n";
	
	std::cout << "Minimum vector: \n";
	//min vector
	vectors = fileParser.getMinVector();
	for(i=0; i<VECTOR_COUNT; i++){
		std::cout << vectors[i] << ",";
	}
	
	std::cout << "\n\nMax X: " << xy_pair_max.first << " Max Y: " << xy_pair_max.second << "\n";
	
	std::cout << "Maximum vector: \n";
	
	//max vector
	vectors = fileParser.getMaxVector();
	for(i=0; i<VECTOR_COUNT; i++){
		std::cout << vectors[i] << ",";
	}
	
	std::cout << "\n\nTotal rows read: " << fileParser.getTotalRows() << "\n\n";
	
	fileParser.doCleanUp();
    return 0;
}
