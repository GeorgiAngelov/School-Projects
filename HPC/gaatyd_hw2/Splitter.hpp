#ifndef SCOTTGS_FORK_SPLITTER_H
#define SCOTTGS_FORK_SPLITTER_H

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <unistd.h>

#include <cstdlib>
#include <set>
#include <iostream>

namespace scottgs {

#define DEBUG 0

class Splitter
{
public:
	Splitter();
	~Splitter();
	// Spawn a process
	pid_t spawn();
	// Reap all children in my process group
	void reap_all();
	// Reap a process
	pid_t reap(pid_t pid = 0);
private:
	pid_t reap_any();
	pid_t reap_this(pid_t pid);
	std::set<pid_t> _processGroup;
};

}; // END of scottgs Namespace

#endif

