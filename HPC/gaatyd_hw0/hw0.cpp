#include <iostream>
#include "file_parser.h"

int main (int argc, char** argv){
	VectorsMap points;
	int i;
	
	if(argc == 1){
		std::cout << "No file was provided.\nPlease execute with ./hw0 <input_filename>\n";
		return 0;
	}
	
	//create a parser object and pass it the filename
	Parser fileParser(argv[1]);
	//parse the file 
	points = fileParser.parseFile();
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
    return 0;
}
