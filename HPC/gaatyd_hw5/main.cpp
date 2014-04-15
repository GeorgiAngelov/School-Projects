#include <iostream>
#include <mpi.h>
#include "main.hpp"

int main(int argc, char *argv[]){
	//initialize MPI
	MPI_Init(&argc, &argv);
	
	int rank;
	int world_size;
	
	message_to_worker message;
	
	//give me my current rank(node 1 reiceves 1 , node 2 etc... )
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	
	int counter = 1;
	
	//IF MASTER
	if (rank == 0) {
		int value = 5;
		int result;
		
		//send messages to the workers
		while(counter<world_size){
			value = counter*value;
			//send the message of type int to 
			result = MPI_Ssend(&value, 1, MPI_INT, counter, 0, MPI_COMM_WORLD);
			
			//make sure the mssage was sent correctly
			if (result == MPI_SUCCESS){
				std::cout << "I am master and the world size is : " << world_size << std::endl;
				std::cout << " I send a message to mky worker thread " << std::endl;
			}else{
				std::cout << "There was a problem with the sending for worker " << counter << std::endl;
			}
			counter++;
		}
	}
	//IF WORKER
	else if (rank > 0) {
		int value;
		//the worker will wait until it receives a message
		int result = MPI_Recv(&value, 1, MPI_INT, 0, 0, MPI_COMM_WORLD,
				  MPI_STATUS_IGNORE);
				  
		//make sure the message was RECEIVED correctly
		if (result == MPI_SUCCESS){
			std::cout << "Worker - Rank: " << rank <<" OK! and received value " << value << std::endl;
		}
	}
	
	//shutdown MPI
	MPI_Finalize();
	return 0;
	
}