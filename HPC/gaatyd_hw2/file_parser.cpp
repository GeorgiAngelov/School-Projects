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

Parser::Parser(char* fname){
	fileName = fname;
}

/*
*	The function parses a file that was set in the constructor
*	And returns a map of the file
*/
VectorsMap Parser::parseFile(){
	//open file
	fPtr = fopen(fileName, "r");
	total_rows = 0;
	line = (char*)malloc(sizeof(char)*LINE_MAX);
	
	//parse the file line by line
	while(fgets(line, LINE_MAX, fPtr)){
		//make sure that we do not read an empty line
		if(line[0] != '\0') {
			//send the line to be further parsed
			parseLine(line);
			//increment total rows count
			total_rows++;	
		}
	}
	return vector_points;
}

void Parser::doCleanUp(){
	fclose(fPtr);
	free(line);
	vector_points.clear();
}

/**
*	Parse a line and tokenize it
*	while extracting X and Y points
*	and vectors and put them in a VectorsMap(deifned in file_parser.h)
*/
void Parser::parseLine(char* line){
	//collection of vectors.
	std::vector<float> vectors;
	char* point;
	
	//grab the x and y tokens
	char* tk1 = strtok(line, ",");
	char* tk2 = strtok(NULL, ",");
	
	//value for indexing
	int i=0;
	char* tmp;
	
	//make sure we have two correct x and y points
	if(tk1 == NULL || tk2 == NULL){ return;	}
	
	//convert the tokens to floats
	float x = strtof(tk1, NULL);
	float y = strtof(tk2, NULL);
	
	//create the x and y pair used to insert vectors into the map
	XYPair pair = XYPair(x, y);
	
	//tokenize until end of line
	while(point=strtok(NULL, ",")){
		//convert the token to float
		float f_point = strtof(point, NULL);
		//push the float to the vector
		vectors.push_back(f_point);
		i++;
	}
	//insert in the vectormap.
	vector_points.insert(VectorsPair(pair, vectors));
}

int Parser::getTotalRows(){
	return total_rows;
}