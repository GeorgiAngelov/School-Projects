/**
*	Name: Georgi Angelov
*	ID: 14120841
*	HW#: Homework 2
*	Date: 2/26/2014
*/
#include <iostream>
#include <iostream>
#include <chrono>
#include <vector>
#include <math.h>
#include <algorithm>
#include "file_parser.hpp"
#include "vectorCSV.hpp"
#include "Splitter.hpp"
#include "main.hpp"
#include "scanner.cpp"
#include <cstring>

struct segment{
	int proc_id;
	unsigned int start;
	unsigned int end;
	unsigned int shm_start;
	unsigned int shm_end;
};

struct ResultType {
	float x; float y; int offset; float dist;
	
	//boolean operator for this struct so we can sort it using std::sort();
	bool operator < (const ResultType& obj) const
	{
		return (dist < obj.dist);
	}
};

/**
*	Function to generate a random vector stored in a vector of floats format.
*/
std::vector<float> generateRandomVector(unsigned int size)
{
	// Again, there is a better way to do this using STL generator or STL bind
	std::vector<float> rv(size, 0);
	for (std::vector<float>::iterator i=rv.begin(); i!=rv.end(); ++i)
		*i =  ((static_cast<float>(rand()) / RAND_MAX) * 2.0) - 1.0;

	return rv;
}

void printVector(const unsigned int n, std::vector<ResultType> results, int vector_size){
	ResultType one;
	int i=0;
	std::cout << "Results:" << std::endl;
	for(i=0; i<n; i++){
		one = results.at(i);
		std::cout << i+1 << ".   " << one.x << ":" << one.y << ":" << one.offset << std::endl;
	}
}

/**
*	The function receives the shared memory segment and it gets it's values and prints the top 10 results
*	The function also receives the number of results that need to be printed.
* 	The function also receives the number of processes so it knows how to loop.
*/
void printResults(float* shm, unsigned int n, unsigned int total_number_of_results, unsigned int vector_size){
	unsigned int i=0;
	ResultType one;
	std::vector<ResultType> results;
	
	//run through the shared memory and insert the values in ResultType
	//structures and stored them into the results vector.
	for(i=0; i<total_number_of_results; i++){
		one.x = shm[i];
		one.y = shm[i+1];
		one.offset = shm[i+2];
		one.dist = shm[i+3];
		results.push_back(one);
		i+=3;
	}
	//sort and resize the results to keep only the top 10
	std::sort(results.begin(), results.end());
	results.resize(n);
	//print the actual results
	printVector(n, results, vector_size);
}

/**
*	The function computes the difference between a search vector and a segment of a circular vector.
*	@vector_size - the size of the segment we are comparing with
*	@searchVector - the generated vector which we use to subtract each point from our circular vector
*	@circularVector - a vector of x,y,points pairs where points is a 360 degree vector
*	@n - number of top results to return
*	@all_offest - value used to indicate from where to begin the search.
*/
std::vector<ResultType> circularSubvectorMatch(const unsigned int vector_size, std::vector<float>* searchVector, VectorsMap* circularVector, const unsigned int n, unsigned int begin_index, unsigned int end_index, unsigned int shm_start, unsigned int shm_end, float* shm, const bool is_test){
	unsigned int i, j, row_index;
	const unsigned int item_size = (*circularVector).size();
	//vector for the returned top N results;
	std::vector<ResultType> results;
	results.reserve(n);
	ResultType one;
	std::vector<float> tmp;
	float x, y, dist, dist_tmp;
	int offset;
	
	//iterate over the whole set of vectors parsed from the file.
	for (row_index=begin_index; row_index < end_index; row_index++) {
		//get a copy of the the vector at position row_index and remove it
		tmp = (*circularVector).at(row_index);
		//get the first and second key and erase them
		x = tmp.at(0);
		tmp.erase(tmp.begin());
		y = tmp.at(0);
		tmp.erase(tmp.begin());

		//run through every vector point at steps of 5
		for(i=0; i<VECTOR_COUNT; i+=5){
			dist = 0;
			offset = i;
			dist_tmp = 0;
			//loop through the vector size(9,11,17,29)
			for(j=0; j<vector_size; j++){
				dist_tmp = fabs((*searchVector).at(j) - tmp.at((j+i)%360));
				dist += dist_tmp;
			}
			
			//put the result into the structure
			one.x = x;
			one.y = y;
			one.offset = i;
			one.dist = dist;

			//Begins min heap process
			if(results.size() < n){
				results.push_back(one);
			}
			// Compare it to the max element in the heap 
			else if (one < results.front()) {
				 // Add the new element to the vector
				 results.push_back(one);
				 // Move the existing minimum to the back and "re-heapify" the rest
				 std::pop_heap(results.begin(), results.end());
				 // Remove the last element from the vector
				 results.pop_back();
			}
		}
	}
	//sort to fix the min heap operations
	std::sort(results.begin(), results.end());
	results.resize(n);
	int count = 0;
	
	//if we are not running a test, save to memory
	if(!is_test){
		for(i=shm_start; i<shm_end; i++){
			shm[i] = results.at(count).x;
			shm[i+1] = results.at(count).y;
			shm[i+2] = results.at(count).offset;
			shm[i+3] = results.at(count).dist;
			count++;
			i+=3;
		}
	}
	return results;
}

void sendWorkToWorkerTmp(int rank, message_to_worker message, int world_size){
	int i=0;
	segment node_segment;
	//WORLD SIZE MUST EXCLUDE THE MASTER NODE !!!
	world_size -= 1;
	//make the node be rank 0 (This is just for calculation purposes. Master is still node 0)
	rank -= 1;
	
	//calculate the number of files
	int size = TOTAL_FILES/world_size;
	const unsigned int per_procc_mem = message.N*4;
	//calculate the start and end files from the files array
	node_segment.start = size*rank;
	node_segment.end = size*rank + size;
	int left_over = 0;
	
	//if it is the last node, there is left over files
	if(rank==world_size-1){
		left_over = TOTAL_FILES%world_size;
	}
	
	node_segment.end += left_over;
	
	std::cout << "I am node " << rank + 1 << " and I will process files from " << node_segment.start << " to " << node_segment.end << std::endl;
	FILE* fPtr; //file pointer to file to be parsed
	char *line; //line to parse file line by line
	VectorsMap points;
	Parser fileParser;
	
	//this is where the final results will be stored
	//size_t memory_space = (node_segment.end - node_segment.start)*4*VECTOR_SIZE;
	int space_size = 4*message.N;
	
	std::cout << "space size is : " << space_size << std::endl;
	float * final_results = (float*)malloc(space_size*sizeof(float));
	
	int total_rows = 0;
	line = (char*)malloc(sizeof(char)*LINE_MAX);
	
	//seed the random generator with the seed value passed to this node
	//this is done so each node can do it's own rand vector
	srand(message.seed);

	int j, process_count=world_size, num_max=message.N;
	//create start and end chrono time points
	std::chrono::time_point<std::chrono::system_clock> start, end;
	double time = 0;
	
	//loop through each file that node is assigned to
	for(i=node_segment.start; i<node_segment.end; i++){
		
		/////////////////////////////////////////
		/////READ FILE POINTS INTO THE VECTOR////
		/////////////////////////////////////////
		fPtr = fopen(message.file_names[i], "r");
		if(fPtr == NULL){
			std::cout << "Cannot open file" << std::endl;
		}
		
		//vectors that will be generated for the runs.
		std::vector<std::vector<float>> generated_vectors(NUMBER_OF_TEST_VECTORS);

		//parse the file line by line and insert each line's points into the points vector
		while(fgets(line, LINE_MAX, fPtr)){
			//make sure that we do not read an empty line
			if(line[0] != '\0') {
				//send the line to be further parsed
				//parseLine returns a vector of x,y,point,point,point ....359 points
				//(first 2 values are x and y)
				points.push_back(fileParser.parseLine(line));
				//increment total rows count
				total_rows++;	
			}
		}
		std::cout << "Opened the first file" << std::endl;
		
		///////////////////////////////////////////////////////////
		/////START THE BLOCK OF CALCULATING THE SUBVECTOR MATCH////
		///////////////////////////////////////////////////////////
		for(int ii=0; ii< NUMBER_OF_TEST_VECTORS; ii++){
			generated_vectors.at(ii) = generateRandomVector(VECTOR_SIZE);
		}
		
		//let the first process print the results.
		std::cout << "\n-----------------" << std::endl;
		std::cout << "Search: "<< VECTOR_SIZE << "-D" << std::endl;
		std::cout << "-----------------" << std::endl;
		
		//for each random vector, perform the subvector match
		for(j=0; j<NUMBER_OF_TEST_VECTORS; j++){
			start = std::chrono::system_clock::now();
			std::cout << "\nSearch Vector: " << std::endl;
			
			//print the created vector.
			std::cout << scottgs::vectorToCSV(generated_vectors[j]) << std::endl;
			
			//call the function to perform the vector submatch and put the results into final_results
			circularSubvectorMatch(VECTOR_SIZE, &generated_vectors[j], &points, message.N, 0, points.size(), 0, space_size, final_results, false);
			
			////////////////
			/////TIMING/////
			////////////////
			//calculate end time.
			end = std::chrono::system_clock::now();
			//now perform printing and stuff. from shared memory.		
			//print end time
			std::chrono::duration<double, std::milli> elapsed_seconds = end-start;

			time += elapsed_seconds.count();
			
			printResults(final_results, message.N, space_size, VECTOR_SIZE);
			
			
			////////////////////////////////////////
			//////////SEND MESSAGE!!!//////////////
			//////////////////////////////////////
			
			//reinitialize the final_results
			free(final_results);
			final_results = (float*)malloc(space_size*sizeof(float));
		}
		
		std::cout << "Average time elapsed is : " << time/NUMBER_OF_TEST_VECTORS << std::endl;
		
		//clean up and reset for the next iteration
		total_rows = 0;
		points.clear();
		fclose(fPtr);
	}
	
	return;
}

int main (void){
	message_to_worker message;
	char directory_path[] = {"/cluster/content/hpc/distributed_data/"};
	message.N = N_RESULTS;
	message.seed = 12312;
	int world_size = NUM_NODES;
	
	//////////////////////////////////
	////////GET THE FILE NAMES////////
	//////////////////////////////////
	typedef std::map<std::string,path_list_type> content_type;
	typedef content_type::const_iterator content_type_citr;
	// Get the file list from the directory
	content_type directoryContents = getFiles(directory_path);
	
	// For each type of file found in the directory, 
	// List all files of that type
	int counter = 0;
	for (content_type_citr f = directoryContents.begin(); 
		f!=directoryContents.end();
		++f)
	{
		path_list_type file_list(f->second);
		
		std::cout << "Showing: " << f->first << " type files (" << file_list.size() << ")" << std::endl;
		for (path_list_type::const_iterator i = file_list.begin();
			i!=file_list.end(); ++i)
		{
			boost::filesystem::path file_path(*i);
			//copy the file name into a char array to pass easily on my custom MPI struct
			strncpy(message.file_names[counter],file_path.file_string().c_str(), COL);
			counter++;
		}
	}
	
	counter = 1;
	for(int i=1; i<world_size; i++){
		//send rank and message
		sendWorkToWorkerTmp(i, message, world_size);
	}
	return 0;
}
