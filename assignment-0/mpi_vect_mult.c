#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h> 

void Read_n(int* n_p, int* local_n_p, int my_rank, int comm_sz, 
      MPI_Comm comm);
void Check_for_error(int local_ok, char fname[], char message[], 
      MPI_Comm comm);
void Read_data(double local_vec1[], double local_vec2[], double* scalar_p,
      int local_n, int my_rank, int comm_sz, MPI_Comm comm);
void Print_vector(double local_vec[], int local_n, int n, char title[], 
      int my_rank, MPI_Comm comm);
double Par_dot_product(double local_vec1[], double local_vec2[], 
      int local_n, MPI_Comm comm);
void Par_vector_scalar_mult(double local_vec[], double scalar, 
      double local_result[], int local_n);

int main(void) {
   int n, local_n;
   double *local_vec1, *local_vec2;
   double scalar;
   double *local_scalar_mult1, *local_scalar_mult2;
   double dot_product;
   int comm_sz, my_rank;
   MPI_Comm comm;

   MPI_Init(NULL, NULL);
   
   comm = MPI_COMM_WORLD;
   MPI_Comm_size(comm, &comm_sz);
   MPI_Comm_rank(comm, &my_rank);
   
   // first read in n: length of vector
   Read_n(&n, &local_n, my_rank, comm_sz, comm);
   
   // initialize vector1,2 and scalar1,2 to read-in in next function
   local_vec1 = malloc(local_n * sizeof(double) );
   local_vec2 = malloc(local_n * sizeof(double) );
   local_scalar_mult1 = malloc(local_n * sizeof(double) );
   local_scalar_mult2 = malloc(local_n * sizeof(double) );

   // read in data
   Read_data(local_vec1, local_vec2, &scalar, local_n, my_rank, comm_sz, comm);
   
   /* Print input data */
   Print_vector(local_vec1, local_n, n, "Input Vector 1: ", my_rank, comm);
   Print_vector(local_vec2, local_n, n, "Input Vector 2: ", my_rank, comm);
   if(my_rank == 0) {
     printf("\nInput Scalar:\n%.2lf\n", scalar);
   }
   
   /* Print results */
   // what?
   
   /* Compute and print dot product */
   dot_product = Par_dot_product(local_vec1, local_vec2, local_n, comm); // returns double
   if(my_rank == 0) {
     printf("\nDot Product of Vector 1 and 2:\n%.2lf\n", dot_product);
   }
   
   /* Compute scalar multiplication and print out result */
   Par_vector_scalar_mult(local_vec1, scalar, local_scalar_mult1, local_n);
   Par_vector_scalar_mult(local_vec2, scalar, local_scalar_mult2, local_n);

   Print_vector(local_scalar_mult1, local_n, n, "Scalar multiplication with Vector 1: ", my_rank, comm);
   Print_vector(local_scalar_mult2, local_n, n, "Scalar multiplication with Vector 2: ", my_rank, comm);
   printf("\n");
   
   free(local_scalar_mult2);
   free(local_scalar_mult1);
   free(local_vec2);
   free(local_vec1);
   
   MPI_Finalize();
   return 0;
}

/*-------------------------------------------------------------------*/
void Check_for_error(
                int       local_ok   /* in */, 
                char      fname[]    /* in */,
                char      message[]  /* in */, 
                MPI_Comm  comm       /* in */) {
   int ok;
   
   MPI_Allreduce(&local_ok, &ok, 1, MPI_INT, MPI_MIN, comm);
   if (ok == 0) {
      int my_rank;
      MPI_Comm_rank(comm, &my_rank);
      if (my_rank == 0) {
         fprintf(stderr, "Proc %d > In %s, %s\n", my_rank, fname, 
               message);
         fflush(stderr);
      }
      MPI_Finalize();
      exit(-1);
   }
}  /* Check_for_error */


/*-------------------------------------------------------------------*/
// n_p - out
// local_n_p - out
void Read_n(int* n_p, int* local_n_p, int my_rank, int comm_sz, 
      MPI_Comm comm) {
  if(my_rank == 0) {
    printf("\nEnter n: \n");
    scanf("%d", n_p);
  }
  MPI_Bcast(n_p, 1, MPI_INT, 0, comm); // broadcast n to all processes

  *local_n_p = *n_p / comm_sz; // calculate how much each process gets
}  /* Read_n */

/*-------------------------------------------------------------------*/
// scalar_p, local_vect1, local_vect2 - out
void Read_data(double local_vec1[], double local_vec2[], double* scalar_p,
      int local_n, int my_rank, int comm_sz, MPI_Comm comm) {
   double* a = NULL;
   int i;
   if (my_rank == 0) {
      printf("What is the scalar?\n");
      scanf("%lf", scalar_p);
   }
   MPI_Bcast(scalar_p, 1, MPI_DOUBLE, 0, comm); // broadcast scalar value to all processes
   
   if (my_rank == 0) {
      a = malloc(local_n * comm_sz * sizeof(double) );
      
      printf("Enter the first vector\n");
      for (i = 0; i < local_n * comm_sz; i++) {
         scanf("%lf", &a[i] );
      }
      MPI_Scatter(a, local_n, MPI_DOUBLE, local_vec1, local_n, MPI_DOUBLE, 0, comm); // p0 scatter first n/comm_sz vector
      
      printf("Enter the second vector\n");
      for (i = 0; i < local_n * comm_sz; i++)  {
         scanf("%lf", &a[i] );
      }
      MPI_Scatter(a, local_n, MPI_DOUBLE, local_vec2, local_n, MPI_DOUBLE, 0, comm); // p0 scatter second n/comm_sz vector
      free(a);
      
   } else {
     MPI_Scatter(a, local_n, MPI_DOUBLE, local_vec1, local_n, MPI_DOUBLE, 0, comm); // pcomm receive first scattered vector
     MPI_Scatter(a, local_n, MPI_DOUBLE, local_vec2, local_n, MPI_DOUBLE, 0, comm); // pcomm receive second scattered vector
   }
}  /* Read_data */

/*-------------------------------------------------------------------*/
// local_vec, a - out
void Print_vector(double local_vec[], int local_n, int n, char title[], 
      int my_rank, MPI_Comm comm) {
   double* a = NULL;
   int i;
   
   if (my_rank == 0) {
      a = malloc(n * sizeof(double) );
      MPI_Gather(local_vec, local_n, MPI_DOUBLE, a, local_n, MPI_DOUBLE, 0, comm); // gather every processes local vector array
      printf("\n%s\n", title);
      printf("[ "); // formatting
      for(i = 0; i < n; i++) {
	printf("%.2lf ", a[i] ); // then print it out
      }
      printf("]\n"); // end formatting
      free(a);
   } else {
     MPI_Gather(local_vec, local_n, MPI_DOUBLE, a, local_n, MPI_DOUBLE, 0, comm); // send local vector array to p0
   }

}  /* Print_vector */


/*-------------------------------------------------------------------*/
double Par_dot_product(double local_vec1[], double local_vec2[], 
      int local_n, MPI_Comm comm) {
  int local_i;
  double y, local_y;
  
  local_y = 0;
  for(local_i = 0; local_i < local_n; local_i++) {
    local_y += local_vec1[local_i] * local_vec2[local_i];
  }
  
  y = 0;
  MPI_Reduce(&local_y, &y, local_n, MPI_DOUBLE, MPI_SUM, 0, comm); // local_y in; y out
  
  return y;
}  /* Par_dot_product */


/*-------------------------------------------------------------------*/
void Par_vector_scalar_mult(double local_vec[], double scalar, 
      double local_result[], int local_n) {
  int local_i;

  for(local_i = 0; local_i < local_n; local_i++) {
    local_result[local_i] = local_vec[local_i] * scalar;
  }
}  /* Par_vector_scalar_mult */
