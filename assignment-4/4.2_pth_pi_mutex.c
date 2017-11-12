#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>
#include "timer.h"

const int MAX_THREADS = 1024;

void* Thread_sum(void* rank);
void Get_args(int argc, char* argv[]); /* Only executed by main thread */
void Usage(char* prog_name);
double Serial_pi(long long n);

// GLOBAL VARIABLES
long thread_count;
long long n;
double sum, sum_serial;
int i;

sem_t* sem;

int main(int argc, char* argv[]) {
  long       thread;  /* Use long in case of a 64-bit system */
  pthread_t* thread_handles;
  double start, finish, elapsed;
  
  Get_args(argc, argv); /* Get number of threads from command line */
  thread_handles = malloc(thread_count * sizeof(pthread_t) );
  sem = malloc(thread_count * sizeof(sem_t) );
  
  for(thread = 0; thread < thread_count; thread++) {
    sem_init(&sem[thread], 0, 0); // init to locked
  }
  
  sum = 0.0;
  // parallel run
  GET_TIME(start);
  for(thread = 0; thread < thread_count; thread++) {
    pthread_create(&thread_handles[thread], NULL, Thread_sum, (void*) thread);
  }
  for(thread = 0; thread < thread_count; thread++) {
    pthread_join(thread_handles[thread], NULL);
  }
  GET_TIME(finish);
  
  printf("\nSemaphore pi estimation: %.12lf\n", sum * 4);
  elapsed = finish - start;
  printf("The code to be timed took %e seconds\n", elapsed);

  // serial run
  GET_TIME(start);
  sum_serial = Serial_pi(n);
  GET_TIME(finish);
  
  printf("\nSerial pi estimation:  %.12lf\n", sum_serial);
  elapsed = finish - start;
  printf("The code to be timed took %e seconds\n\n", elapsed);

  for(thread = 0; thread < thread_count; thread++) {
    sem_destroy(&sem[thread]); // free each semaphore separately
  }
  free(thread_handles);
  free(sem);

  return 0;
}  /* main */

/*------------------------------------------------------------------*/
void* Thread_sum(void* rank) {
  long my_rank = (long) rank;

  long long i;
  double factor = 1.0;
  long my_sum;
  long long my_n = n / thread_count;
  long long my_first_i = my_n * my_rank;
  long long my_last_i = my_first_i + my_n;

  my_sum = 0.0;
  if(my_first_i % 2 == 0) {
    factor = 1.0;
  } else {
    factor = -1.0;
  }

  for (i = my_first_i; i < my_last_i; i++, factor = -factor) {
    sem_post(&sem[my_rank]); // before
    sum += factor / (2 * i + 1);
    sem_wait(&sem[my_rank]); // after
  }

  /* sem_post(&sem[my_rank]); // before */
  /* sum += my_sum; */
  /* sem_wait(&sem[my_rank]); // after */
  
  return NULL;
}  /* Thread_sum */

/*------------------------------------------------------------------
 * Function:   Serial_pi
 * Purpose:    Estimate pi using 1 thread
 * In arg:     n
 * Return val: Estimate of pi using n terms of Maclaurin series
 */
double Serial_pi(long long n) {
   double sum = 0.0;
   long long i;
   double factor = 1.0;

   for (i = 0; i < n; i++, factor = -factor) {
      sum += factor/(2*i+1);
   }
   return 4.0*sum;

}  /* Serial_pi */

/*------------------------------------------------------------------
 * Function:    Get_args
 * Purpose:     Get the command line args
 * In args:     argc, argv
 * Globals out: thread_count, n
 */
void Get_args(int argc, char* argv[]) {
   if (argc != 3) Usage(argv[0]);
   thread_count = strtol(argv[1], NULL, 10);  
   if (thread_count <= 0 || thread_count > MAX_THREADS) Usage(argv[0]);
   n = strtoll(argv[2], NULL, 10);
   if (n <= 0) Usage(argv[0]);
}  /* Get_args */


/*------------------------------------------------------------------
 * Function:  Usage
 * Purpose:   Print a message explaining how to run the program
 * In arg:    prog_name
 */
void Usage(char* prog_name) {
   fprintf(stderr, "usage: %s <number of threads> <n>\n", prog_name);
   fprintf(stderr, "   n is the number of terms and should be >= 1\n");
   fprintf(stderr, "   n should be evenly divisible by the number of threads\n");
   exit(0);
}  /* Usage */
