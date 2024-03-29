/**
*	Name: Georgi Angelov
*	ID: 14120841
*	HW#: Homework 0
*	Date: 2/21/2014
*/

#ifndef _FILE_PARSER_H
#define _FILE_PARSER_H

#include <iostream>
#include <cstdio>
#include <map>
#include <iomanip>
#include <vector>
#include <chrono>
#define LINE_MAX 5000
#define VECTOR_COUNT 360
#define ITEMS_PER_LINE 362

//typedef std::map<XYPair, std::vector<float> > VectorsMap;
typedef std::vector<std::vector<float>> VectorsMap;

class Parser{
	public:
		//constructor
        Parser();
		int getTotalRows();
		int row_values;
		int total_rows;
		std::vector<float> vectors;
		std::vector<float> parseLine(char* line);
	private:
		//collection of all x y points and their vectors 
		VectorsMap vector_points;
		FILE* fPtr; //file pointer to file to be parsed
		char *line; //line to parse file line by line
		char* fileName; //path/name of file to be parsed
};

#endif