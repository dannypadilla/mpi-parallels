#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

void get_args(int argc, char* argv[] );
void usage(char* prog_name);

long thread_count;

int main(int argc, char* argv[] ) {
  long thread;
  pthread_t* thread_handles;

  get_args(argc, argv);
  thread_handles = malloc(thread_count * sizeof(p_thread_t) );

  for(thread = 0; thread < thread_count; thread++) {
    pthread_create(&thread_handles[thread], NULL, /* func_here*/, (void*) thread);
  }
  for(thread = 0; thread < thread_count; thread++) {
    pthread_join(thread_handles[thread], NULL);
  }


  // here goes code

  // be free
  free(thread_handles);
  
  return 0;
}

/* ********************************************************************* */

 void get_args(int argc, char* argv[]) {
  if (argc != 3) {
    Usage(argv[0]);
  }
  thread_count = strtol(argv[1], NULL, 10);  
  if (thread_count <= 0 || thread_count > MAX_THREADS) {
    Usage(argv[0]);
  }
  n = strtoll(argv[2], NULL, 10);
  if (n <= 0) {
    Usage(argv[0]);
  }
}

void usage(char* prog_name) {
  fprintf(stderr, "usage: %s <number of threads> <n>\n", prog_name);
  fprintf(stderr, "   n is the number of terms and should be >= 1\n");
  fprintf(stderr, "   n should be evenly divisible by the number of threads\n");
  exit(0);
}
