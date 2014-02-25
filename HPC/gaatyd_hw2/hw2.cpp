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
#include "Splitter.hpp"

struct segment{
	int proc_id;
	unsigned int start;
	unsigned int end;
	unsigned int shm_start;
	unsigned int shm_end;
};

std::vector<float> generateScottVector(const unsigned int size){
	int i=0;
	//1k file
	float array1[] = {0.0536727,0.0384691,0.00146231,0.0122459,0.0198738,-0.116341,0.0998519,0.0269831,-0.000772231};
	float array2[] = {0.0572175,-0.139987,-0.143134,-0.0428729,0.118296,0.0105897,0.0302701,-0.134377,-0.0855214,-0.0757894,0.140506};
	float array3[] = {0.0416003,0.0107126,0.0120675,-0.00547709,-0.00533309,0.00230428,0.00302696,0.0717615,0.0265672,-0.0319207,0.0132625,-0.0669076,-0.0196825,0.0726006,-0.0277976,0.0813408,-0.0819924};
	float array4[] = {0.0208535,0.00124793,0.0221286,-0.0251518,-0.00935498,0.000144995,0.0543099,0.152197,-0.0436211,-0.0536875,-0.0389395,-0.163685,0.105508,0.0135837,-0.0582674,-0.0525398,0.107217,-0.0480279,0.00522108,0.0145284,0.10181,-0.19426,0.0345067,0.126417,-0.143776,0.125843,-0.0239083,0.0613458,-0.265121};

	std::vector<float> return_vector;
	
	switch(size){
		case 9:
			return_vector.reserve(9);
			for(i; i<9; i++)
				return_vector.push_back(array1[i]);
			return return_vector;
		case 11:
			return_vector.reserve(11);
			for(i; i<11; i++)
				return_vector.push_back(array2[i]);
			return return_vector;
		case 17:
			return_vector.reserve(17);
			for(i; i<17; i++)
				return_vector.push_back(array3[i]);
			return return_vector;
		case 29:
			return_vector.reserve(29);
			for(i; i<29; i++)
				return_vector.push_back(array4[i]);
			return return_vector;
		default:
		return return_vector;
	}
}

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

bool runTest(const unsigned int vector_size, std::vector<ResultType>* searchVector){
	float test_array[10][3];
	switch(vector_size){
		case 9:
			test_array = {
			{34.4661,68.3391,290},
			{34.4661,68.3552,140},
			{34.4664,68.356,140},
			{34.467,68.3493,20},
			{34.4664,68.3482,355},
			{34.4672,68.3638,175},
			{34.4667,68.3502,25},
			{34.4675,68.3399,265},
			{34.4678,68.3574,355},
			{34.4675,68.3521,25}};
		break;
		
		case 11:
			test_array =  {
			{34.4678,68.3346,310},
			{34.4675,68.3343,310},
			{34.4672,68.3341,310},
			{34.467,68.3338,310},
			{34.4672,68.3343,310},
			{34.4664,68.351,245},
			{34.4675,68.3366,215},
			{34.4678,68.3349,310},  
			{34.4672,68.3368,215},
			{34.4661,68.3652,105}};
		break;
		
		case 17:
			test_array =  {
			{34.467,68.3624,285},
			{34.4672,68.3357,155},
			{34.467,68.3446,155},
			{34.467,68.3593,75},
			{34.4659,68.3396,340},
			{34.4678,68.3571,185},
			{34.4664,68.3624,325},
			{34.4678,68.3516,45},
			{34.4664,68.3471,205},
			{34.4664,68.3352,10}};
		break;
		
		case 29:
			test_array = {
			{34.4667,68.3568,335},
			{34.4661,68.3441,130},
			{34.4661,68.3357,145},
			{34.4664,68.3574,330},
			{34.4659,68.3435,130},
			{34.4664,68.3504,60},
			{34.4664,68.3366,145},
			{34.4664,68.3374,145},
			{34.467,68.3385,260},
			{34.4667,68.3629,10}};
		break;
	}
	
	for(int i=0; i<10; i++){
		if(test_array[i][0] != (*searchVector).at(i).x)
			return false;
		if(test_array[i][1] != (*searchVector).at(i).y)
			return false;
		if(test_array[i][2] != (*searchVector).at(i).offset)
			return false;
	}
	
	return true;
}

/**
*	The function receives the shared memory segment and it gets it's values and prints the top 10 results
*	The function also receives the number of results that need to be printed.
* 	The function also receives the number of processes so it knows how to loop.
*/
void printResults(float* shm, unsigned int n, unsigned int process_count, unsigned int vector_size){
	unsigned int i=0;
	//total entries in the shared memory
	const unsigned int shm_size = process_count*n*4;
	ResultType one;
	std::vector<ResultType> results;

	for(i=0; i<shm_size; i++){
		one.x = shm[i];
		one.y = shm[i+1];
		one.offset = shm[i+2];
		one.dist = shm[i+3];
		results.push_back(one);
		i+=3;
	}
	std::sort(results.begin(), results.end());
	results.resize(n);
	std::cout << std::setw(10) << "x" << "|" << std::setw(10) << "y" << "|" << std::setw(8) << "offset" << "|" << std::setw(12) << "score" << std::endl;
	std::cout << "----------+----------+--------+------------" << std::endl;
	//print the results
	for(i=0; i<n; i++){
		one = results.at(i);
		std::cout << std::setw(10) << one.x << "|" << std::setw(10) << one.y << "|" << std::setw(8) << one.offset << "|" << std::setw(12) << one.dist << std::endl;
	}
	std::cout << "(" << n << " rows)" << std::endl;
	
	if(runTest(vector_size, &results)){
		std::cout << " Test SUCCESSFUL with size " << vector_size << std::endl;
	}else{
		std::cout << " Test UNSUCCESSFUL with size " << vector_size << std::endl;
	}
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
			if(results.size() < 10){
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

int main (int argc, char** argv){
	if(argc < 4){
		std::cout << "3 command line arguments are needed.\nPlease execute with ./hw0 <input_filename> <process #> <number of match list>\n";
		return 0;
	}
	FILE* fPtr; //file pointer to file to be parsed
	char *line; //line to parse file line by line
		
	fPtr = fopen(argv[1], "r");
	int total_rows = 0;
	line = (char*)malloc(sizeof(char)*LINE_MAX);
	VectorsMap points;
	Parser fileParser;
	//vectors that will be generated for the runs.
	std::vector<std::vector<float>> generated_vectors(30);

	//parse the file line by line
	while(fgets(line, LINE_MAX, fPtr)){
		//make sure that we do not read an empty line
		if(line[0] != '\0') {
			//send the line to be further parsed
			points.push_back(fileParser.parseLine(line));
			//increment total rows count
			total_rows++;	
		}
	}
	free(line);
	fclose(fPtr);

	srand(34122);
	int i,j, process_count=atoi(argv[2]), num_max=atoi(argv[3]), size=0;
	//the size of the search vectors
	int sizes[] = {9,11,17,29};
	int sizes_count = 4;
	//will hold the offsets for each process
	std::vector<segment> segments(process_count);
	
	//initialize shared memory
	size_t memory_space = process_count*40;
	float shm_size = memory_space * sizeof(float);
	int shmId;
	// use current time as seed for random generator
	std::srand(std::time(0));
	key_t shmKey = std::rand();
	int shmFlag = IPC_CREAT | 0666;
	float * shm;
	
	/* Initialize shared memory */
	if((shmId = shmget(shmKey, shm_size, shmFlag)) < 0)
	{
		std::cerr << "Init: Failed to initialize shared memory (" << shmId << ")" << std::endl; 
		exit(1);
	}
	
	if((shm = (float *)shmat(shmId, NULL, 0)) == (float *) -1)
	{
		std::cerr << "Init: Failed to attach shared memory (" << shmId << ")" << std::endl; 
		exit(1);
	}
	
	//initialize offsets
	for(i=0; i< process_count; i++){
		int size = total_rows/process_count;
		segments.at(i).start = size*i;
		segments.at(i).end = size*i + size;
		segments.at(i).shm_start = i*40;
		segments.at(i).shm_end = i*40 + 40;
		//if at the last process, check to see if the division is not even
		if(i==process_count-1)
			segments.at(i).end += total_rows%process_count;
	}
	
	//create the final results vector
	//reserve enough space to be able to merge all of our processes' stuff
	std::vector<ResultType> final_results;
	std::vector<float> copy;
	final_results.reserve(process_count*num_max);

	//create start and end chrono time points
	std::chrono::time_point<std::chrono::system_clock> start, end;
	
	//print header.
	std::cout << "===========" << std::endl;
	std::cout << " Put Data Set Count Here " << std::endl;
	std::cout << "===========" << std::endl;
	
	//loop through the 4 different sizes of vectors
	for(i=0; i<sizes_count; i++){
	
		//run the test:
		/*copy = generateScottVector(sizes[i]);
		final_results = circularSubvectorMatch(sizes[i], &copy, &points, num_max, 0, total_rows, 0, 0, NULL, true);
		copy.clear();
		if(runTest(sizes[i], &final_results)){
			std::cout << "Test was Successful against vector: " << std::endl;
			std::cout << scottgs::vectorToCSV(generateScottVector(sizes[i])) << std::endl;
		}else{
			std::cout << "Test FAILED against vector: " << std::endl;
			std::cout << scottgs::vectorToCSV(generateScottVector(sizes[i])) << std::endl;
			//exit(-1);
		}
		final_results.clear();*/
		
		//generate 30 random vectors of size size[i]
		//for(int ii=0; ii< 30; ii++){
		//	generated_vectors.at(ii) = generateRandomVector(sizes[ii]);
		//}
		
		//for testing purposes I am only doing 1.
		generated_vectors.at(0).reserve(sizes[i]);
		generated_vectors.at(0) = generateScottVector(sizes[i]);
		
		//let the first process print the results.
		std::cout << "-----------------" << std::endl;
		std::cout << "Search: "<< sizes[i] << "-D" << std::endl;
		std::cout << "-----------------" << std::endl;
		//get an object of the process spawner class.
		start = std::chrono::system_clock::now();
		scottgs::Splitter splitter;
		for (int p = 0; p < process_count ; ++p)
		{
			pid_t pid = splitter.spawn();
			if (pid < 0)
			{
				std::cerr << "Could not fork!!! ("<< pid <<")" << std::endl;
				// do not exit, we may have a process 
				// spawned from an earlier iteration
				break; 
			}
			if (0 == pid) // Child
			{
				/* Attach shared memory */
				if((shm = (float *)shmat(shmId, NULL, 0)) == (float *) -1)
				{
					std::cerr << "Init: Failed to attach shared memory (" << shmId << ")" << std::endl; 
					exit(1);
				}
				//loop through the (30 vectors specified in the description)
				for(j=0; j<1; j++){	
						
					//let only process 0 to print this vector.
					if(p == 0){
						//print the created vector.
						std::cout << scottgs::vectorToCSV(generated_vectors[j]) << std::endl;
					}

					//perform the test(delete this as it is not needed)
					//pas the size of the search vector, the auto generated vector, the vectors from the file,
					//the number of top results to return, and the offset from which to search.
					circularSubvectorMatch(sizes[i], &generated_vectors[j], &points, num_max, segments.at(p).start, segments.at(p).end, segments.at(p).shm_start, segments.at(p).shm_end, shm, false);
				}
				_exit(0);
			}
		}
		//wait for all children before looping again.
		splitter.reap_all();
		//calculate end time.
		end = std::chrono::system_clock::now();
		//now perform printing and stuff. from shared memory.		
		//print top num_max results
		printResults(shm, num_max, process_count, sizes[i]);
		//print end time		
		std::chrono::duration<double> elapsed_seconds = end-start;
		std::cout << "\nTime: " << elapsed_seconds.count() << " seconds" << std::endl;
	}
	//delete shared memory after we are done with it.
	shmctl(shmKey, IPC_RMID, NULL);
	
	return 0;
}
