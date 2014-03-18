/* 
  Multi-fork'n
*/

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>
#include <vector>

#include <cstdlib>
#include <set>
#include <iostream>
 
#include <boost/lexical_cast.hpp>

#include "Splitter.hpp"

int main (int argc, char * argv[])
{
	if (argc < 2)
	{
		std::cout << "Usage" << argv[0] << " <threads>" << std::endl;
		exit(1);
	}
	unsigned int procs = boost::lexical_cast<unsigned int>(argv[1]);
	std::cout << "Testing multifork using " << procs << " threads." << std::endl;
	
	scottgs::Splitter splitter;
	for (int p = 0; p < procs ; ++p)
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
			std::cout << "Child Process (" << pid << ")" << "And my number is : " << p << std::endl;	
			
			// Do work on somethig ?
			//sleep(p+2);
			std::cout << "Vector value at " << p << " is " << std::endl;
			_exit(0);
		}
		else
		{
			; // Nothing to do for the parent
		}
	}

	// Blocking function
	splitter.reap_all();	
}

