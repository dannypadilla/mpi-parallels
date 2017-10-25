#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

/* 3.1
   Suppose we toss darts randomly at a square dartboard, whose bullseye is at the
   origin, and whose sides are 2 feet in length. Suppose also that there's a circle
   inscribed in the square dartboard. The radius of the circle is 1 foot, and it's area
   is PI square feet. If the points that are hit by the darts are uniformly distributed
   (and we always hit the square), then the number of darts that hit inside the circle
   should approximately satisfy the equation:

      number in circle         PI
   ---------------------- =  -----
   total number of tosses      4

   since the ratio of the area of the circle to the area of the square is PI/4.
   We can use this formula to ESTIMATE THE VALUE OF PI with a random number
   generator
*/



int main(void) {

  int comm_sz, my_rank;
  MPI_Comm comm;

  MPI_Init(NULL, NULL);

  comm = MPI_COMM_WORLD;
  MPI_Comm_size(comm, &comm_sz);
  MPI_Comm_rank(comm, &my_rank);

  
  
  MPI_Finalize();
  return 0;
  
}
   
int random_number_generator(int number_of_tosses) {
  int toss, number_of_tosses, number_in_circle;
  double distance_squared;
  number_in_circle = 0;
  for(toss = 0; toss < number_of_tosses; toss++) {
    // x = random double between -1 and 1;
    // y = random double between -1 and 1;
    distance_squared = x * x + y * y;
    if(distance_squared <= 1) {
      number_in_circle++;
    }
  }
  pi_estimate = 4 * number_in_circle / ((double) number_of_tosses);
}
