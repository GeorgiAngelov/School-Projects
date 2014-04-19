#include <iostream>
#include <mpi.h>
#include "main.hpp"
#include "scanner.cpp"
/*
char** returnFiles(char *directory_path){
	// Define a template type, and its iterator	
	typedef std::map<std::string,path_list_type> content_type;
	typedef content_type::const_iterator content_type_citr;
	// Get the file list from the directory
	content_type directoryContents = getFiles(directory_path);
	char file_names[ROW][COL];
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
			//boost::filesystem::path file_path(boost::filesystem::system_complete(*i));
			boost::filesystem::path file_path(*i);
			//std::cout << "\t" << file_path.file_string() << std::endl;
			//char *files = my_copy.c_str();
			strncpy(file_names[counter],file_path.file_string().c_str(), 60);
			std::cout << "File name : " << file_names[counter] << std::endl;
			counter++;
		}
	}
	return file_names;
}*/

int main(int argc, char *argv[]){
	//initialize MPI
	MPI_Init(&argc, &argv);
	
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
	
	int rank;
	int world_size;
	
	//give me my current rank(node 1 reiceves 1 , node 2 etc... )
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	
	int counter = 1;
	
	//IF MASTER
	if (rank == 0) {
	
		char directory_path[] = {"/cluster/content/hpc/distributed_data/"};
		message.N = 5;
		message.seed = 12312;
		
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
				//boost::filesystem::path file_path(boost::filesystem::system_complete(*i));
				boost::filesystem::path file_path(*i);
				//std::cout << "\t" << file_path.file_string() << std::endl;
				//char *files = my_copy.c_str();
				strncpy(message.file_names[counter],file_path.file_string().c_str(), 60);
				//std::cout << "File name : " << file_names[counter] << std::endl;
				counter++;
			}
		}
		
		//////////////////////////////
		///////SEND THE MESSAGES/////
		//////////////////////////////
		counter=1;
		//send messages to the workers
		while(counter<world_size){
			//send 1 message of type MessageType using my own structure(first parameter)
			MPI_Ssend(&message, 1, MessageType, counter, tag, MPI_COMM_WORLD);
			std::cout << "I am master and the world size is : " << world_size << std::endl;
			std::cout << " I send a message to mky worker thread " << std::endl;
			counter++;
		}
	}
	//IF WORKER
	else if (rank > 0) {
		//the worker will wait until it receives a message
		int result = MPI_Recv(&message, 1, MessageType, 0, tag, MPI_COMM_WORLD,
				  MPI_STATUS_IGNORE);
				  
		//make sure the message was RECEIVED correctly
		if (result == MPI_SUCCESS){
			std::cout << "Worker - Rank: " << rank <<" OK! and the value received through the message is " << message.N << " and seed is " << message.seed << std::endl;
			std::cout << "File at position " << rank << " Is : " << message.file_names[rank] << std::endl;
		}
	}
	
	//shutdown MPI
	MPI_Finalize();
	return 0;
}