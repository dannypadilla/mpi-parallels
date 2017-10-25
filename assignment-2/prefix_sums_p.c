#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

void read_n(int* n, int my_rank, MPI_Comm comm);
void read_input(double* local_user_input, int n, int local_n,
		int my_rank, int comm_sz, MPI_Comm comm);

int main(void) {
  int n, local_n;
  double *user_input;

  int comm_sz, my_rank;
  MPI_Comm comm;

  MPI_Init(NULL, NULL);

  comm = MPI_COMM_WORLD;
  MPI_Comm_size(comm, &comm_sz);
  MPI_Comm_rank(comm, &my_rank);

  // stuff
  read_n(&n, my_rank, comm);
  user_input = malloc(local_n * sizeof(double) );

  read_input();
  

  free(user_input);

  MPI_Finalize();
  
  return 0;
}

void read_n(int* n, int my_rank, MPI_Comm comm) {
  if(my_rank == 0) {
    printf("Enter n:\n");
    scanf("%d", n);
  }
  MPI_Bcast(n, 1, MPI_INT, 0, comm);
}

void read_input(double local_user_input[], int n, int local_n,
		int my_rank, int comm_sz, MPI_Comm comm) {
  
  
}
