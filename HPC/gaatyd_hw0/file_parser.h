#ifndef _FILE_PARSER_H
#define _FILE_PARSER_H

#include <iostream>
#include <cstdio>
#include <map>
#include <iomanip>
#include <vector>
#define LINE_MAX 5000
#define VECTOR_COUNT 360
//create specific types to make my life easier later on
typedef std::pair<float, float> XYPair;
typedef std::pair<XYPair, std::vector<float> > VectorsPair;
typedef std::map<XYPair, std::vector<float> > VectorsMap;

class Parser{   	
	public:
		//constructor  s
        Parser(char* fname);
		VectorsMap parseFile();
		XYPair getMaxXY();
		XYPair getMinXY();
		float* getMaxVector();
		float* getMinVector();
		int getTotalRows();
		int row_values;
		int total_rows;
	private:
		//collection of all x y points and their vectors 
		VectorsMap vector_points;
		FILE* fPtr;
		char* fileName;
		//variables to hold largest and smallest valuess
		//these values are arbitrary
		float xMax, xMin, yMax, yMin;
		float* vectorMax;
		float* vectorMin;

		void parseLine(char* line);
		void isMaxXY(float x, float y);
		void isMaxMinVector(float value, int index);
};

#endif