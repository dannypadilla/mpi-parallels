#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>

const int MAX_THREADS = 1024;
const int MSG_MAX = 50;

void* send_msg(void* rank);
void get_args(int argc, char* argv[]);
void usage(char* prog_name);

// GLOBAL VARIABLES
long thread_count;
long long n;
int message_available, consumer; // flags
char** messages;
pthread_mutex_t mutex;

int main(int argc, char* argv[] ) {
  long thread;
  pthread_t* thread_handles;

  get_args(argc, argv);
  //thread_count = 2;
  thread_handles = malloc(thread_count * sizeof(pthread_t) );
  messages = malloc(thread_count * sizeof(char*) );

  pthread_mutex_init(&mutex, NULL);
  message_available = 0; // false
  consumer = 0; // thread 0 is the consumer

  for(thread = 0; thread < thread_count; thread++) {
    pthread_create(&thread_handles[thread], NULL, send_msg, (void*) thread);
  }
  for(thread = 0; thread < thread_count; thread++) {
    pthread_join(thread_handles[thread], NULL);
  }

  // free resources
  pthread_mutex_destroy(&mutex); // destroy mutex
  free(messages);
  free(thread_handles);

  return 0;
}

/* ***************************************************************** */

// have producer and consumer SHARE A MUTEX
// a FLAG variable that's initialized to false (0) by the main thread
void* send_msg(void* rank) {
  long my_rank = (long) rank;
  long dest = (my_rank + 1) % thread_count;
  char* my_msg = malloc(MSG_MAX * sizeof(char) );

  while(1) {
    pthread_mutex_lock(&mutex); // shared mutex
    if(my_rank == consumer) { // thread 0 is the consumer
      if(message_available) {
	printf("CONSUMER thread %ld message: \"%s\"\n", my_rank, messages[my_rank] ); // print message
	//printf("COMSUMER thread %ld here\n", my_rank);
	pthread_mutex_unlock(&mutex);
	break;
      }
    } else {
      sprintf(my_msg, "Hello to consumer thread %ld from PRODUCER thread %ld", dest, my_rank); // create message
      //printf("Message from PRODUCER thread %ld\n", my_rank);
      messages[dest] = my_msg;
      message_available = 1;
      pthread_mutex_unlock(&mutex);
      break;
    }
    pthread_mutex_unlock(&mutex);
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
