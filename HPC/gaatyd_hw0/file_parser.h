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

#define FILEPATH "/content/cs/hpc/data/sp14_1k.csv"
#define NUMINTS  (1000)
#define FILESIZE (NUMINTS * sizeof(int))

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
		void doCleanUp();
		void isMaxXY(float x, float y);
		void isMaxMinVector(float value, int index);
	private:
		//collection of all x y points and their vectors 
		VectorsMap vector_points;
		FILE* fPtr; //file pointer to file to be parsed
		char *line; //line to parse file line by line
		char* fileName; //path/name of file to be parsed
		
		//variables to hold largest and smallest valuess
		//these values are arbitrary
		float xMax, xMin, yMax, yMin;
		float* vectorMax; //array of floats for max vector
		float* vectorMin; //array of floats for min vector

		void parseLine(char* line);

};

#endif