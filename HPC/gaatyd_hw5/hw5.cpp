/**
*	Name: Georgi Angelov
*	ID: 14120841
*	HW#: Homework 5
*	Date: 4/19/2014
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
#include <mpi.h>

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
		std::cout << i+1 << ".   x:" << one.x << " y:" << one.y << " offset:" << one.offset << " dist:" << one.dist  << std::endl;
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

std::vector<ResultType> sendWorkToWorker(int rank, message_to_worker message, int world_size, char *file_name,std::vector<float> generated_vector){
	//std::cout << "Inside sendWorkerToWorker  and I am rank " << rank << " the file to work on is : " << file_name << " and the size of the vectors is " << generated_vector.size() << std::endl;
	int i=0;
	
	FILE* fPtr; //file pointer to file to be parsed
	char *line; //line to parse file line by line
	VectorsMap points;
	Parser fileParser;
	std::vector<ResultType> results_to_return;
	
	//this is where the final results will be stored
	int space_size = 4*message.N;
	
	float * final_results = (float*)malloc(space_size*sizeof(float));
	
	int total_rows = 0;
	line = (char*)malloc(sizeof(char)*LINE_MAX);
	
	//seed the random generator with the seed value passed to this node
	//this is done so each node can do it's own rand vector
	srand(message.seed);

	int j, process_count=world_size, num_max=message.N;
	
	/////////////////////////////////////////
	/////READ FILE POINTS INTO THE VECTOR////
	/////////////////////////////////////////
	fPtr = fopen(file_name, "r");
	if(fPtr == NULL){
		std::cout << "Cannot open file" << std::endl;
		return results_to_return;
	}

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
	
	//call the function to perform the vector submatch and put the results into final_results
	results_to_return = circularSubvectorMatch(VECTOR_SIZE, &generated_vector, &points, message.N, 0, points.size(), 0, space_size, final_results, false);
	
	//clean up and reset for the next iteration
	total_rows = 0;
	points.clear();
	fclose(fPtr);
	
	return results_to_return;
}

int main (int argc, char *argv[]){
	//initialize MPI
	MPI_Init(&argc, &argv);
	MPI_Status status;
	//////////////////////////////////////
	///DEFINE THE SEND STRUCT MESSAGE///
	//////////////////////////////////////
	message_to_worker message;
	//declare my new type
	MPI_Datatype MessageType;
	//declare the types the the structure will have
	//MPI_Datatype type[3] = { MPI_CHAR, MPI_DOUBLE, MPI_CHAR };
	MPI_Datatype type[3] = { MPI_INT, MPI_INT, MPI_CHAR };
	//the number of results for each type(int[50] will be 50 etc..)
	//int blocklen[3] = { 1, 6, 7 };
	int blocklen[3] = {1,1,ROW*COL};
	
	//this will store the displacement for each var in the structure
	//MPI_Aint disp[3];
	MPI_Aint disp[3] = {0,sizeof(int),sizeof(int)+sizeof(int)};

	//number of vars
	int count = 3;
	
	//define the type
	MPI_Type_struct(count, blocklen, disp, type, &MessageType);
	MPI_Type_commit(&MessageType); 
	
	//////////////////////////////////////
	///DEFINE THE RETURN STRUCT MESSAGE///
	//////////////////////////////////////
	message_to_master m_message;
	MPI_Datatype MessageToMasterType;
	MPI_Datatype type2[5] = {MPI_FLOAT,MPI_FLOAT,MPI_FLOAT,MPI_FLOAT,MPI_FLOAT};
	int blocklen2[5] = {1,1,1,1,1};
	MPI_Aint disp2[5] = {0,sizeof(float), sizeof(float)*2, sizeof(float)*3, sizeof(float)*4};
	int count2 = 5;
	
	MPI_Type_struct(count2, blocklen2, disp2, type2, &MessageToMasterType);
	MPI_Type_commit(&MessageToMasterType); 
	
	int rank;
	int world_size;
	
	//give me my current rank(node 1 reiceves 1 , node 2 etc... )
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	
	int counter = 1;
	
	//IF MASTER
	if (rank == 0) {
		char directory_path[] = {"/cluster/content/hpc/dev_data"};
		message.N = N_RESULTS;
		message.seed = 26123;
	
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
					
		message_to_master buffer[N_RESULTS];
		
		std::vector<ResultType> final_results;
		final_results.reserve(N_RESULTS);
		std::vector<ResultType> tmp;
		tmp.reserve(N_RESULTS);
		ResultType one;
		double time = 0;
		
		//////////////////////////////
		///////SEND THE MESSAGES/////
		//////////////////////////////
		for(int test=0; test<NUMBER_OF_TEST_VECTORS; test++){
			counter = 1;
			//send messages to the workers
			while(counter<world_size){
				//send 1 message of type MessageType using my own structure(first parameter)
				MPI_Ssend(&message, 1, MessageType, counter, tag, MPI_COMM_WORLD);
				counter++;
			}
			
			//read the results from the results!
			for(int w=0; w<world_size-1; w++){
				tmp.clear();
				//the worker will wait until it receives a message
				int result = MPI_Recv(&buffer, N_RESULTS, MessageToMasterType, MPI_ANY_SOURCE, tag, MPI_COMM_WORLD,
					  MPI_STATUS_IGNORE);

				time += buffer[0].time;
				
				for(int i=0; i<N_RESULTS; i++){
					one.x = buffer[i].x;
					one.y = buffer[i].y;
					one.offset = buffer[i].offset;
					one.dist = buffer[i].dist;
					tmp.push_back(one);
				}
				
				final_results.insert( final_results.end(), tmp.begin(), tmp.end() );
				//sort and resize the results
				std::sort(final_results.begin(), final_results.end());
				final_results.resize(N_RESULTS);
			}
			
			for(int i=0; i<N_RESULTS; i++){
				std::cout << "Final results x:" << final_results.at(i).x << " dist:" << final_results.at(i).dist << std::endl;
			}
		}
	}
	//IF WORKER
	else if (rank > 0) {
		std::vector<std::vector<float>> generated_vectors(30);
		//WORLD SIZE MUST EXCLUDE THE MASTER NODE !!!
		world_size -= 1;
		//decrease it by 1 so we can start from 0 for computational purposes
		int job_rank = rank-1;
		
		///////////////////////////////////////////////////////////
		/////START THE BLOCK OF CALCULATING THE SUBVECTOR MATCH////
		///////////////////////////////////////////////////////////
		for(int ii=0; ii< NUMBER_OF_TEST_VECTORS; ii++){
			generated_vectors.at(ii) = generateRandomVector(VECTOR_SIZE);
		}
		
		for(int test=0; test<NUMBER_OF_TEST_VECTORS; test++){
			//the worker will wait until it receives a message
			int result = MPI_Recv(&message, 1, MessageType, 0, tag, MPI_COMM_WORLD,
					  MPI_STATUS_IGNORE);
			std::cout << job_rank << " received message ! " << std::endl;
			//make sure the message was RECEIVED correctly
			if (result == MPI_SUCCESS){
				int i=0,j=0;
				message_to_master buffer[message.N];
				
				segment node_segment;
				
				//calculate the number of files
				int size = TOTAL_FILES/world_size;
				//calculate the start and end files from the files array
				node_segment.start = size*job_rank;
				node_segment.end = size*job_rank + size;
				int left_over = 0;
				
				//if it is the last node, there is left over files
				if(job_rank==world_size-1){
					left_over = TOTAL_FILES%world_size;
				}
				node_segment.end += left_over;

				//seed the random generator with the seed value passed to this node
				//this is done so each node can do it's own rand vector
				srand(message.seed);
			
				std::chrono::time_point<std::chrono::system_clock> start, end;
				double time = 0;
				//create the return results vectors and tmp vectors
				//tmp receives the results from the function and return_results merges them
				std::vector<ResultType> return_results;
				return_results.reserve(message.N);
				std::vector<ResultType> tmp;
			
				//loop through each file that node is assigned to 
				for(i=node_segment.start; i<node_segment.end; i++){
					//for each random vector, perform the subvector match
					start = std::chrono::system_clock::now();
					//get the top N results
					tmp = sendWorkToWorker(job_rank, message, world_size, message.file_names[i], generated_vectors.at(test));
					end = std::chrono::system_clock::now();
					std::chrono::duration<double, std::milli> elapsed_seconds = end-start;
					return_results.insert( return_results.end(), tmp.begin(), tmp.end() );
		
					//sort and resize the results
					std::sort(return_results.begin(), return_results.end());
					return_results.resize(message.N);
					
					time += elapsed_seconds.count();
				}
				
				//put into the buffer to send to master!
				for(int k=0; k<message.N; k++){
					buffer[k].x = return_results.at(k).x;
					buffer[k].y = return_results.at(k).y;
					buffer[k].offset = return_results.at(k).offset;
					buffer[k].dist = return_results.at(k).dist;
					buffer[k].time = time/(node_segment.end - node_segment.start);
				} 
				MPI_Ssend(&buffer, message.N, MessageToMasterType, 0, tag, MPI_COMM_WORLD);
			}
		}
	}//end outermost for loop over test messages
	//shutdown MPI
	MPI_Finalize();
	return 0;
}
