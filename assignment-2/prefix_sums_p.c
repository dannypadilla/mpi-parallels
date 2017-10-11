#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

int main(void) {

  int comm_sz, my_rank;
  MPI_Comm comm;

  MPI_Init(NULL, NULL);

  comm = MPI_COMM_WORLD;
  MPI_Comm_size(comm, &comm_sz);
  MPI_Comm_rank(comm, &my_rank);

  // stuff

  MPI_Finalize();
  
  return 0;
}
