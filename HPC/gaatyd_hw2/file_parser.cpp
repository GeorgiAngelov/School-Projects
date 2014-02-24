/**
*	Name: Georgi Angelov
*	ID: 14120841
*	HW#: Homework 2
*	Date: 2/21/2014
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include "file_parser.hpp"

Parser::Parser(){
	vectors.reserve(ITEMS_PER_LINE);
}

/**
*	Parse a line and tokenize it
*	while extracting X and Y points
*	and vectors and put them in a VectorsMap(deifned in file_parser.h)
*/
std::vector<float> Parser::parseLine(char* line){
	//collection of vectors.
	vectors.clear();
	
	char* point;
	
	//grab the x and y tokens
	char* tk1 = strtok(line, ",");
	char* tk2 = strtok(NULL, ",");
	
	//make sure we have two correct x and y points
	if(tk1 == NULL || tk2 == NULL){ return vectors;	}
	
	//convert the tokens to floats
	const float x = strtof(tk1, NULL);
	const float y = strtof(tk2, NULL);
	
	//create the x and y pair used to insert vectors into the map
	//XYPair pair = XYPair(x, y);
	vectors.push_back(x);
	vectors.push_back(y);
	//tokenize until end of line
	while(point=strtok(NULL, ",")){
		//convert the token to float
		const float f_point = strtof(point, NULL);
		//push the float to the vector
		vectors.push_back(f_point);
	}
	//insert in the vectormap.
	return vectors;
}

int Parser::getTotalRows(){
	return total_rows;
}