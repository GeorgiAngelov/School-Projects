/* Fork'n example

*/

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <cstdlib>

#include <iostream>


int main(int argc, char * argv[])
{
	pid_t pid;	// process identifier, pid_t is a process id type defined in sys/types

	pid = fork();	// fork, which replicates the process 

	if ( pid < 0 )
	{ 
		std::cerr << "Could not fork!!! ("<< pid <<")" << std::endl;
		exit(1);
	}
	
	std::cout << "I just forked without error, I see ("<< pid <<")" << std::endl;
	
	if ( pid == 0 ) // Child process 
	{
		std::cout << "In the child (if): " << std::endl; 
		
		//int myChildExitCode = 0; 	// this could be set to something meaningfull or 0;
		int myChildExitCode = 8; 	// this could be set to something meaningfull or 0;
		
		// bad op
		int *iPtr = new int;  	// declare pointer
		delete iPtr;		// free
		delete iPtr;		// double free, segfault

		_exit(myChildExitCode);
	} 
	else		// Parent Process
	{
		std::cout << "In the parent (if-else): " << std::endl; 
	}
	

	// ============================== 
	// All this code is boiler-plate	
	// ============================== 

	std::cout << "In the parent: " << std::endl; 

	int status;	// catch the status of the child

	do  // in reality, mulptiple signals or exit status could come from the child
	{

		pid_t w = waitpid(pid, &status, WUNTRACED | WCONTINUED);
		if (w == -1)
		{
			std::cerr << "Error waiting for child process ("<< pid <<")" << std::endl;
			break;
		}
		
		if (WIFEXITED(status))
		{
			if (status > 0)
			{
				std::cerr << "Child process ("<< pid <<") exited with non-zero status of " << WEXITSTATUS(status) << std::endl;
				continue;
			}
			else
			{
				std::cout << "Child process ("<< pid <<") exited with status of " << WEXITSTATUS(status) << std::endl;
				continue;
			}
		}
		else if (WIFSIGNALED(status))
		{
			std::cout << "Child process ("<< pid <<") killed by signal (" << WTERMSIG(status) << ")" << std::endl;
			continue;			
		}
		else if (WIFSTOPPED(status))
		{
			std::cout << "Child process ("<< pid <<") stopped by signal (" << WSTOPSIG(status) << ")" << std::endl;
			continue;			
		}
		else if (WIFCONTINUED(status))
		{
			std::cout << "Child process ("<< pid <<") continued" << std::endl;
			continue;
		}
	}
	while (!WIFEXITED(status) && !WIFSIGNALED(status));
	
	
	return 0;
}

