#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

const int MAX_THREADS = 1024;

void* trap(void* rank);
void get_args(int argc, char* argv[] );
void usage(char* prog_name);

// global vars
long thread_count;
double global_estimate, a, b, h; // a, b - left, right endpoints
long n; // trap count; base length

int main(int argc, char* argv[] ) {
  long thread;
  pthread_t* thread_handles;

  get_args(argc, argv);
  thread_handles = malloc(thread_count * sizeof(pthread_t) );

  h = (b - a) / n;
  
  for(thread = 0; thread < thread_count; thread++) {
    pthread_create(&thread_handles[thread], NULL, trap, (void*) thread);
  }
  for(thread = 0; thread < thread_count; thread++) {
    pthread_join(thread_handles[thread], NULL);
  }

  printf("Trap estimate is:\n%.9lf\n", global_estimate); // process 0 prints final estimate

  // be free
  free(thread_handles);
  
  return 0;
}

/* ********************************************************************* */

void* trap(void* rank) {
  double local_a, local_b, local_estimate, x;
  long local_n, i;
  long my_rank;

  my_rank = (long) rank;
  
  local_n = n / thread_count;
  local_a = a + my_rank * local_n * h;
  local_b = local_a + local_n * h;

  local_estimate = ( (local_a * local_a) + (local_b * local_b) ) / 2.0; // just used x ** 2 for function
  
  for(i = 1; i < local_n; i++) {
    x = local_a + (i * h);
    local_estimate += (x * x); // x squared is f(x)
  }

  // critical section
  global_estimate += local_estimate * h;
  
  return NULL;
}

 void get_args(int argc, char* argv[] ) {
  if (argc != 5) {
    usage(argv[0]);
  }
  
  thread_count = strtol(argv[1], NULL, 10);
  if (thread_count <= 0 || thread_count > MAX_THREADS) { // n
    usage(argv[0]);
  }
  
  a = strtoll(argv[2], NULL, 10);
  if (a < 0) { // only positive #'s
    usage(argv[0]);
  }

  b = strtoll(argv[3], NULL, 10);
  if (b <= a) { // b > a
    usage(argv[0]);
  }

  n = strtoll(argv[4], NULL, 10);
  if (n <= 0) {
    usage(argv[0]);
  }
}

// edit to handle a, b, and n
void usage(char* prog_name) {
  fprintf(stderr, "usage: %s <number of threads> <a> <b> <n>\n", prog_name);
  fprintf(stderr, "   a is the left end-point of terms and should be >= 0\n");
  fprintf(stderr, "   b is the right end-point of terms and should be < a\n");
  fprintf(stderr, "   n is the number of trapezoids and should be >= 1\n");
  fprintf(stderr, "   n should be evenly divisible by the number of threads\n");
  exit(0);
}
