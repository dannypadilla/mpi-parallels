#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

void read_order(int* order,
		int my_rank, MPI_Comm comm);
void read_vector(double local_vec_x[], double local_vec_y[], int order, int local_order,
		 int my_rank, int comm_sz, MPI_Comm comm);
void print_vector(double local_vec[], int local_order, int order, char title[],
		  int my_rank, MPI_Comm comm);
void add_two_vectors(double local_vec_x[], double local_vec_y[], double local_result[], int local_order);

void build_mpi_contiguous_type(int order, MPI_Datatype* input_mpi_t);

int main(void) {
  int order, local_order;
  double *local_vec_x, *local_vec_y, *local_vec_sum;

  int comm_sz, my_rank;
  MPI_Comm comm;

  MPI_Init(NULL, NULL);

  comm = MPI_COMM_WORLD;
  MPI_Comm_size(comm, &comm_sz);
  MPI_Comm_rank(comm, &my_rank);

  // read order
  read_order(&order, my_rank, comm);

  // calculate how many items each process gets
  local_order = order / comm_sz;

  // init vector sizes
  local_vec_x = malloc(local_order * sizeof(double) );
  local_vec_y = malloc(local_order * sizeof(double) );
  local_vec_sum = malloc(local_order * sizeof(double) );
  
  // read vector x and y
  read_vector(local_vec_x, local_vec_y, order, local_order, my_rank, comm_sz, comm);
  
  // print x and y
  print_vector(local_vec_x, local_order, order, "Vector X = ", my_rank, comm);
  print_vector(local_vec_y, local_order, order, "Vector Y = ", my_rank, comm);
  
  // add x + y
  add_two_vectors(local_vec_x, local_vec_y, local_vec_sum, local_order);

  // print x + y
  print_vector(local_vec_sum, local_order, order, "X + Y = ", my_rank, comm);

  free(local_vec_x);
  free(local_vec_y);
  free(local_vec_sum);

  MPI_Finalize();

  return 0;
}

void read_order(int* order, int my_rank, MPI_Comm comm) {
  if(my_rank == 0) {
    printf("Enter the order(length) of the vectors:\n");
    scanf("%d", order);
  }
  MPI_Bcast(order, 1, MPI_INT, 0, comm);
}

void build_mpi_contiguous_type(int order, MPI_Datatype* input_mpi_t) {
  
  MPI_Type_contiguous(order, MPI_DOUBLE, input_mpi_t);
  MPI_Type_commit(input_mpi_t);
}

void read_vector(double local_vec_x[], double local_vec_y[], int order, int local_order,
		  int my_rank, int comm_sz, MPI_Comm comm) {
  double* user_input = NULL;
  MPI_Datatype input_mpi_t;
  int i;

  build_mpi_contiguous_type(local_order, &input_mpi_t);
  
  if(my_rank == 0) {
    user_input = malloc(local_order * comm_sz * sizeof(double) );
    
    printf("Enter the first Vector:\n");
    for(i = 0; i < local_order * comm_sz; i++) {
      scanf("%lf", &user_input[i] );
    }
    MPI_Scatter(user_input, 1, input_mpi_t, local_vec_x, 1, input_mpi_t, 0, comm);

    printf("Enter the second Vector:\n");
    for(i = 0; i < local_order * comm_sz; i++) {
      scanf("%lf", &user_input[i] );
    }
    MPI_Scatter(user_input, 1, input_mpi_t, local_vec_y, 1, input_mpi_t, 0, comm);
    
    free(user_input);
  } else {
    MPI_Scatter(user_input, 1, input_mpi_t, local_vec_x, 1, input_mpi_t, 0, comm);
    MPI_Scatter(user_input, 1, input_mpi_t, local_vec_y, 1, input_mpi_t, 0, comm);
  }

  MPI_Type_free(&input_mpi_t);
}

void print_vector(double local_vec[], int local_order, int order, char title[],
		  int my_rank, MPI_Comm comm) {
  double * vector = NULL;
  MPI_Datatype input_mpi_t;
  int i;

  build_mpi_contiguous_type(local_order, &input_mpi_t);

  if(my_rank == 0) {
    vector = malloc(order * sizeof(double) );
    MPI_Gather(local_vec, 1, input_mpi_t, vector, 1, input_mpi_t, 0, comm);
    printf("\n%s\n", title);
    printf("[ ");
    for(i = 0; i < order; i++) {
      printf("%.2lf ", vector[i] );
    }
    printf("]\n");
    free(vector);
  } else {
    MPI_Gather(local_vec, 1, input_mpi_t, vector, 1, input_mpi_t, 0, comm);
  }
}

void add_two_vectors(double local_vec_x[], double local_vec_y[], double local_result[], int local_order) {
  int local_i;
  
  for(local_i = 0; local_i < local_order; local_i++) {
    local_result[local_i] = local_vec_x[local_i] + local_vec_y[local_i];
  }
  
}
