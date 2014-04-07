#include <iostream>
#include <mpi.h>

int main (int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    char hostname[255]; int len;
    int id, procs;
    MPI_Comm_rank(MPI_COMM_WORLD, &id);
    MPI_Comm_size(MPI_COMM_WORLD, &procs);
    MPI_Get_processor_name(hostname, &len);

    if (id == 0 && argc != 2) {
        std::cout << "Invalid usage please run with: " << argv[1] << " <name>\n";
        return 1;
    }
  std::cout << "Hello " << argv[1] << " from thread id " << id << " out of " << procs 
<< " threads using hostname " << hostname << ".\n";
    MPI_Finalize();
    return 0;
}
