#include <iostream>
#include <mpi.h>
#include "main.hpp"

int main(int argc, char *argv[]){
	//initialize MPI
	MPI_Init(&argc, &argv);
	
	message_to_worker message;
	//declare my new type
	MPI_Datatype MessageType;
	
	//declare the types the the structure will have
	//MPI_Datatype type[3] = { MPI_CHAR, MPI_DOUBLE, MPI_CHAR };
	MPI_Datatype type[2] = { MPI_INT, MPI_INT };
	//the number of results for each type(int[50] will be 50 etc..)
	//int blocklen[3] = { 1, 6, 7 };
	int blocklen[2] = {1,1};
	
	//this will store the displacement for each var in the structure
	//MPI_Aint disp[3];
	MPI_Aint disp[2] = {0,0};
	MPI_Aint var1, var2;
	
	//get the address of the message and put in var1(to used for calculating displacement)
	/*MPI_Get_address(&(message), &var1);
	MPI_Get_address(&(message.N), &var2);*/
	
	//number of vars
	int count = 2;
	
	//define the type
	//MPI_Type_create_struct(3, blocklen, disp, type, &Particletype);
    //MPI_Type_commit(&MessageType);
	MPI_Type_struct(count, blocklen, disp, type, &MessageType);
	MPI_Type_commit(&MessageType); 

	std::cout << "The displacement is " << disp[0] << std::endl;
	
	int rank;
	int world_size;
	
	//give me my current rank(node 1 reiceves 1 , node 2 etc... )
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);
	
	int counter = 1;
	message.N = 5;
	message.seed = 12312;
	
	//IF MASTER
	if (rank == 0) {
		int result;
		
		//send messages to the workers
		while(counter<2){
			//send 1 message of type MessageType using my own structure(first parameter)
			result = MPI_Ssend(&message, 1, MessageType, counter, tag, MPI_COMM_WORLD);
			
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
		//the worker will wait until it receives a message
		int result = MPI_Recv(&message, 1, MessageType, 0, tag, MPI_COMM_WORLD,
				  MPI_STATUS_IGNORE);
				  
		//make sure the message was RECEIVED correctly
		if (result == MPI_SUCCESS){
			std::cout << "Worker - Rank: " << rank <<" OK! and the value received through the message is " << message.N << " and seed is " << message.seed << std::endl;
		}
	}
	
	//shutdown MPI
	MPI_Finalize();
	return 0;
}