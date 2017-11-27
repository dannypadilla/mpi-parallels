
/* implement task queue using a linked list
 * 
 * 1. main thread generates a random sorted linked list
 * 2. get user input
 * 3. possible tasks on list: member, delete, or insert
 *
 * + each thread will do a number of tasks according to a and b
 * + each task is randomly choosing from member, delete, and insert
 * + must provide the rwlock to protect the list from incorrect status */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

const int MAX_THREADS = 1024;

void* do_task(void* rank);
void create_list(int list_len);
int get_random_task(); // put params
int member(int value);
int insert(int value);
int delete(int value);
void usage(char* prog_name);
void get_args(int argc, char* argv[] );

struct list_node_s {
  int data;
  struct list_node_s* next;
};

// global
struct list_node_s* head = NULL;

int thread_count; // b
int number_of_tasks; // a

int list_len = 20;
int member_val = 0;
int insert_val = 1;
int delete_val = 2;

pthread_rwlock_t rwlock;

/* ********************* MAIN *********************** */

int main(int argc, char* argv[] ) {
  long thread;
  pthread_t* thread_handles;

  pthread_rwlock_init(&rwlock, NULL);
  srand(time(NULL) );

  get_args(argc, argv);
  thread_handles = malloc(thread_count * sizeof(pthread_t) );

  // generate random sorted linked list
  create_list(list_len);

  for(thread = 0; thread < thread_count; thread++) {
    pthread_create(&thread_handles[thread], NULL, do_task, (void*) thread);
  }
  for(thread = 0; thread < thread_count; thread++) {
    pthread_join(thread_handles[thread], NULL);
  }

  pthread_rwlock_destroy(&rwlock);
  free(thread_handles);
  
  return 0;
}

void create_list(int list_len) {
  int i;
  
  for(i = 0; i < list_len; i++) { // create the linked list
    insert(i);
  }
}

int get_random_task() {

  return (rand() % 3);
}

void* do_task(void* rank) {
  long my_rank = (long) rank;
  int i;
  int my_tasks = number_of_tasks / thread_count;
  int first_task = my_rank * my_tasks;
  int last_task = (my_rank + 1) * my_tasks - 1;
  int random_op, random_node;

  for(i = first_task; i < last_task; i++) {
    random_op = (rand() % 3);
    random_node = (rand() % list_len);
    //printf("Random OP - %d\n", random_op);
    //printf("Random NODE - %d\n", random_node);
    
    /* 0 - member, 1 - delete, 2 - insert */
    if(random_op == 0) { // member
      printf("\nPerforming MEMBER task on NODE %d\n", random_node);
      pthread_rwlock_rdlock(&rwlock);
      member(random_node);
      pthread_rwlock_unlock(&rwlock);
      
    } else if(random_op == 1) { // insert
      printf("\nPerforming INSERT task on NODE %d\n", random_node);
      pthread_rwlock_wrlock(&rwlock);
      insert(random_node);
      pthread_rwlock_unlock(&rwlock);
      
    } else { // must be delete
      printf("\nPerforming DELETE task on NODE %d\n", random_node);
      pthread_rwlock_wrlock(&rwlock);
      delete(random_node);
      pthread_rwlock_unlock(&rwlock);
    }
  }

  return NULL;
}

int member(int value) {
   struct list_node_s* temp;

   temp = head;
   while (temp != NULL && temp->data < value)
      temp = temp->next;

   if (temp == NULL || temp->data > value) {
#     ifdef DEBUG
      printf("%d is not in the list\n", value);
#     endif
      return 0;
   } else {
#     ifdef DEBUG
      printf("%d is in the list\n", value);
#     endif
      return 1;
   }
}

int insert(int value) {
   struct list_node_s* curr = head;
   struct list_node_s* pred = NULL;
   struct list_node_s* temp;
   int rv = 1;
   
   while (curr != NULL && curr->data < value) {
      pred = curr;
      curr = curr->next;
   }

   if (curr == NULL || curr->data > value) {
      temp = malloc(sizeof(struct list_node_s));
      temp->data = value;
      temp->next = curr;
      if (pred == NULL)
         head = temp;
      else
         pred->next = temp;
   } else { /* value in list */
      rv = 0;
   }

   return rv;
}  /* Insert */

int delete(int value) {
   struct list_node_s* curr = head;
   struct list_node_s* pred = NULL;
   int rv = 1;

   /* Find value */
   while (curr != NULL && curr->data < value) {
      pred = curr;
      curr = curr->next;
   }
   
   if (curr != NULL && curr->data == value) {
      if (pred == NULL) { /* first element in list */
         head = curr->next;
#        ifdef DEBUG
         printf("Freeing %d\n", value);
#        endif
         free(curr);
      } else { 
         pred->next = curr->next;
#        ifdef DEBUG
         printf("Freeing %d\n", value);
#        endif
         free(curr);
      }
   } else { /* Not in list */
      rv = 0;
   }

   return rv;
}

void get_args(int argc, char* argv[] ) {
  if (argc != 3) {
    usage(argv[0]);
  }
  
  thread_count = strtol(argv[1], NULL, 10); // b - number of threads
  if (thread_count <= 0 || thread_count > MAX_THREADS) {
    usage(argv[0]);
  }
  
  number_of_tasks = strtoll(argv[2], NULL, 10); // number of tasks
  if (number_of_tasks < 0) { // only positive #'s
    usage(argv[0]);
  }
}

void usage(char* prog_name) {
  fprintf(stderr, "usage: %s <thread_count> <number_of_tasks>\n", prog_name); // b then a
  fprintf(stderr, "   a is the number of tasks to perform and should be > 0\n");
  exit(0);
}
