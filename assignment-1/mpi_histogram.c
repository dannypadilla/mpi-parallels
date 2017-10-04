#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

/*
  1. Use MPI to implement the histogram program discussed in Ch.2.
  * Have process 0 read in the input data and distribute it among the processes.
  * Also have process 0 print out the program.
  */

int find_bin(double data, double* bin_maxes, int bin_count, double min_meas);

void read_input_data();

int main(void) {
  int comm_sz, my_rank;
  MPI_Comm comm;
  
  double data[20] = {
    1.3, 2.9, 0.4, 0.3, 1.3, 4.4, 1.7, 0.4, 3.2, 0.3, 4.9, 2.4, 3.1, 4.4, 3.9, 0.4, 4.2, 4.5, 4.9, 0.9
  }; // array of data_value floats
  double* local_data;
  int data_count, bin_count; // data_count - number of measurements, bin_count = number of bins
  double min_meas, max_meas; // min/max value for the bin containing the smallest/largest values; respectively
  double bin_width; // array of bin_count floats with the upperbound for each bin; width of each bin
  // double bin_maxes[]; --> init down below
  // int bin_counts[]; -- > init down below

  MPI_Init(NULL, NULL);

  comm = MPI_COMM_WORLD;
  MPI_Comm_size(comm, &comm_sz);
  MPI_Comm_rank(comm, &my_rank);

  // ask to initialize number of values (data_count) then prompt for values. Otherwise use constant
  data_count = 20; // tmp
  bin_count = 5; // tmp --> does it depend on the highest measure value in the array??????

  // calculate (find) min_meas/max_meas --- MIGHT BE ABLE TO USE MPI_REDUCE (..MPI_MIN or MPI_MAX)
  /*
    Maybe loop MPI_Scatter every comm_sz elements:
    ( [0, i*comm_sz - 1], [comm_sz, i*comm_sz - 1], ... , [comm_sz * i, data_count])
    ex. ([0, 3], [4, 7], ... ,  [16, 19])
    and MPI_Reduce(MPI_MIN/MAX on each iteration [x, y]..
  */
  min_meas = 0.3; // tmp
  max_meas = 4.9; // tmp

  bin_width = (max_meas - min_meas) / bin_count;

  double bin_maxes[bin_count];
  int bin_counts[bin_count];
  
  int b;
  for(b = 0; b < bin_count; b++) {
    bin_maxes[b] = min_meas + bin_width * (b + 1); // set the max value of each bin (upper value)
  }
  for(b = 0; b < bin_count; b++) {
    bin_counts[b] = 0; // initialize each bin_counts element to 0
  }

  int i;
  int bin;
  for(i = 0; i < data_count; i++) {
    bin = find_bin(data[i], bin_maxes, bin_count, min_meas); // find which bin data[i] belongs to
    bin_counts[bin]++;
  }

  // print
  if(my_rank == 0) {
    int i;
    printf("\n[ ");
    for(i = 0; i < data_count; i++) {
      printf("%.2lf ", data[i] );
    }
    printf("]\n\n");
    for(i = 0; i < bin_count; i++) {
      printf("bin %d: %d\n", i + 1, bin_counts[i] );
    }
    printf("\n");
    for(i = 0; i < bin_count; i++) {
      printf("bin %d max: %.2lf\n", i + 1, bin_maxes[i] );
    }
    printf("\n");
  }
  
  MPI_Finalize();

  return 0;
}

int find_bin(double data, double* bin_maxes, int bin_count, double min_meas) { // search for bin
  int bin;
  int local_min_meas = min_meas;
  for(bin = 0; bin < bin_count; bin++) {
    if( local_min_meas <= data && data <= bin_maxes[bin] ) {
      return bin;
    }
    local_min_meas += local_min_meas * (bin + 1);
  }
  return bin;
}
