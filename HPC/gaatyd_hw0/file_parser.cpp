/**
*	Name: Georgi Angelov
*	ID: 14120841
*	HW#: Homework 0
*	Date: 1/30/2014
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include "file_parser.h"

Parser::Parser(char* fname){
	fileName = fname;
	xMax=-11111111.0, xMin=10000000.0, yMax=-111111111.0, yMin=10000000.0;
	vectorMax = (float*)malloc(sizeof(float)*VECTOR_COUNT);
	vectorMin = (float*)malloc(sizeof(float)*VECTOR_COUNT);
}

/*
*	The functoin parses a file that was set in the constructor
*	And returns a map of the file
*/
VectorsMap Parser::parseFile(){
	//open file
	fPtr = fopen(fileName, "r");
	total_rows = 0;
	line = (char*)malloc(sizeof(char)*LINE_MAX);
	//create start and end chrono time points
	std::chrono::time_point<std::chrono::system_clock> start, end;
	//record the start tiem.
    start = std::chrono::system_clock::now();
	
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
	//calculate end time.
	end = std::chrono::system_clock::now();
	//calculate duration
	std::chrono::duration<double> elapsed_seconds = end-start;
	//print info on total timel
	std::cout << "\nTotal time to parse file : " << elapsed_seconds.count() << " seconds\n";
	return vector_points;
}

void Parser::doCleanUp(){
	fclose(fPtr);
	free(line);
	free(vectorMax);
	free(vectorMin);
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
	isMaxXY(x,y);
	
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

void Parser::isMaxXY(float x, float y){
	if(x > xMax) xMax = x;
	if(x < xMin) xMin = x;
	if(y > yMax) yMax = y;
	if(y < yMin) yMin = y;
}

void Parser::isMaxMinVector(float value, int index){	
	//if we are at the first row, initialize the vectors
	if(total_rows == 0){
		vectorMax[index] = -11111111.11;
		vectorMin[index] = 11111111.11;
	}
	//if new vector is larger, then overwrite it
	if(value > vectorMax[index]) vectorMax[index] = value;
	if(value < vectorMin[index]) vectorMin[index] = value;
}

float* Parser::getMaxVector(){
	return vectorMax;
}

float* Parser::getMinVector(){
	return vectorMin;
}

XYPair Parser::getMaxXY(){
	return XYPair(xMax, yMax);
}

XYPair Parser::getMinXY(){
	return XYPair(xMin, yMin);
}

int Parser::getTotalRows(){
	return total_rows;
}