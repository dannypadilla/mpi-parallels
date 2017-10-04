#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

/*
  1. Implement a revised version of Program 3.2: Trapezoidal run to get input from the user
  2. Use MPI_Reduce to rewrite the collective communication part of Problem 1
 */

void get_input(double* a, double* b, int* n, int my_rank, MPI_Comm comm);
double trap(double local_a, double local_b, int local_n, double h); // trap()
double func(double x);

int main(void) {
  int comm_sz, my_rank, n, local_n; // n - numnber of trapezoids, n_local - number of trapezoids to each process
  double a, b, h, local_a, local_b;
  double local_int, total_int;
  MPI_Comm comm;
  
  MPI_Init(NULL, NULL);
  
  comm = MPI_COMM_WORLD;
  MPI_Comm_size(comm, &comm_sz);
  MPI_Comm_rank(comm, &my_rank);

  get_input(&a, &b, &n, my_rank, comm); // read in a, b, n

  h = (b - a) / n; // could broadcast since it's the same for all processes
  local_n = n / comm_sz; // this too

  local_a = a + my_rank * local_n * h; // find this processes left endpoint ::: Maybe use scatter? test..
  local_b = local_a + local_n * h; // find this processes right endpoint
  local_int = trap(local_a, local_b, local_n, h); // calculate local traps area (integral)
  
  MPI_Reduce(&local_int, &total_int, 1, MPI_DOUBLE, MPI_SUM, 0, comm); // use MPI_Reduce to send local_int (sum) to process 0
  
  if(my_rank == 0) {
    printf("Trap estimate is:\n%.4lf\n", total_int); // process 0 prints final estimate
  }
  
  MPI_Finalize();
  
  return 0;
}

double func(double x) {
  return x * x;
}

void get_input(double* a, double* b, int* n, int my_rank, MPI_Comm comm) {
  if (my_rank == 0) {
    printf("Enter a, b, and n:\n");
    scanf("%lf %lf %d", a, b, n);
  }
  MPI_Bcast(a, 1, MPI_DOUBLE, 0, comm);
  MPI_Bcast(b, 1, MPI_DOUBLE, 0, comm);
  MPI_Bcast(n, 1, MPI_INT, 0, comm);
}

double trap(double local_a, double local_b, int local_n, double h) {
  double estimate, x;
  int i;

  estimate = ( func(local_a) + func(local_b) ) / 2.0; // find f(x) function
  for(i = 1; i < local_n; i++) {
    x = local_a + (i * h);
    estimate += func(x); // find f(x) function
  }
  estimate = estimate * h;
  
  return estimate;
}
