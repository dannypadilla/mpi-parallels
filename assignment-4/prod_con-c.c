#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>

/* 4.3 c.
 * Generalize this so that each thread is both a consumer and a producer.
 * ex. Suppose that thread q sends a message to thread (q + 1) mod t
 *     and receives a message from thread (q - 1 + t) mod t,
 *     where t is the total number of threads.
 */

const int MAX_THREADS = 1024;
const int MSG_MAX = 100;

void* send_msg(void* rank);
void get_args(int argc, char* argv[]);
void usage(char* prog_name);

// GLOBAL VARIABLES
long thread_count;
int message_available; // flag
char** messages;
pthread_mutex_t mutex;

int main(int argc, char* argv[] ) {
  long thread;
  pthread_t* thread_handles;

  get_args(argc, argv);
  // allocate resources
  thread_handles = malloc(thread_count * sizeof(pthread_t) );
  messages = malloc(thread_count * sizeof(char*) );
  for(thread = 0; thread < thread_count; thread++) {
    messages[thread] = NULL;
  }

  pthread_mutex_init(&mutex, NULL); // init mutex
  message_available = 0;

  for(thread = 0; thread < thread_count; thread++) {
    pthread_create(&thread_handles[thread], NULL, send_msg, (void*) thread);
  }
  for(thread = 0; thread < thread_count; thread++) {
    pthread_join(thread_handles[thread], NULL);
  }

  // free resources
  pthread_mutex_destroy(&mutex); // destroy mutex
  for(thread = 0; thread < thread_count; thread++) {
    free(messages[thread]); // free up each threads message
  }
  free(messages);
  free(thread_handles);

  return 0;
}

/* ***************************************************************** */

// have producer and consumer SHARE a mutex
// a FLAG variable that's initialized to false (0) by the main thread
void* send_msg(void* rank) {
  long my_rank = (long) rank;
  long dest = (my_rank + 1) % thread_count;
  //char* my_msg = malloc(MSG_MAX * sizeof(char) );

  while(1) {
    pthread_mutex_lock(&mutex); // shared mutex
    if(message_available == 0) { // consumers
      //printf("Consumer thread %ld message: %s\n", my_rank, messages[my_rank] ); // print message
      printf("Hello from CONSUMER thread %ld\n", my_rank);
      message_available = 1; // msg ready
      pthread_mutex_unlock(&mutex);
      break;
    } else { // producer
      //sprintf(my_msg, "Hello to %ld from producer thread %ld", dest, my_rank); // create message
      printf("Hello to %ld from PRODUCER thread %ld\n", dest, my_rank);
      //messages[dest] = my_msg;
      message_available = 0; // reset
      pthread_mutex_unlock(&mutex);
      break;
    }
  }
  return NULL;
}

void get_args(int argc, char* argv[]) {
  if (argc != 2) { // changed from 3 to 2
    usage(argv[0]);
  }
  thread_count = strtol(argv[1], NULL, 10);  
  if (thread_count <= 0 || thread_count > MAX_THREADS) {
    usage(argv[0]);
  }
}

void usage(char* prog_name) {
  fprintf(stderr, "usage: %s <number of threads>\n", prog_name);
  exit(0);
}
