#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

/* 3.0
Implement matrix-vector multiplication using block distribution of the
matrix. You can have process 0 read the matrix and the vector. Assume the
matrix is square of order n and that n is evenly divisible by comm_sz
*/



int main(void) {

  int comm_sz, my_rank;
  MPI_Comm comm;

  MPI_Init(NULL, NULL);

  comm = MPI_COMM_WORLD;
  MPI_Comm_size(comm, &comm_sz);
  MPI_Comm_rank(comm, &my_rank);

  
  
  MPI_Finalize();
  
}
