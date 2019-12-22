/* File:     linked_list_multithreading_error.c
 *
 * Purpose:  Implement a multi-threaded sorted linked list of 
 *           ints with ops insert, print, isMember, delete, free list.
 *           This version uses a single mutex
 * 
 * Compile:  gcc -g -Wall -o linked_list_multithreading_error linked_list_multithreading_error.c
 *              my_rand.c -lpthread
 *           needs timer.h and my_rand.h
 *
 * Usage:    ./linked_list_multithreading_error <thread_count>
 * Input:    total number of keys inserted by main thread
 *           total number of ops carried out by each thread (all threads
 *              carry out the same number of ops)
 *           percent of ops that are searches and inserts (remaining ops
 *              are deletes.
 * Output:   Elapsed time to carry out the ops
 *
 * Notes:
 *    1.  Repeated values are not allowed in the list
 *    2.  DEBUG compile flag used.  To get debug output compile with
 *        -DDEBUG command line flag.
 *    3.  The random function is not threadsafe.  So this program
 *        uses a simple linear congruential generator.
 *    4.  -DOUTPUT flag to gcc will show list before and after
 *        threads have worked on it.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "my_rand.h"
#include "timer.h"

pthread_mutex_t mutex;
pthread_mutex_t count_mutex;
/* Random ints are less than MAX_KEY */
const int MAX_KEY = 100000;

/* Struct for list nodes */
struct list_node_s {
   int    data;
   struct list_node_s* next;
   pthread_mutex_t mutex;
};

/* Shared variables */
struct      list_node_s* head = NULL;  
pthread_mutex_t head_mutex;
int         thread_count;
int         total_ops;
double      insert_percent;
double      search_percent;
double      delete_percent;

int         member_total=0, insert_total=0, delete_total=0;

/* Setup and cleanup */
void        Usage(char* prog_name);
void        Get_input(int* inserts_in_main_p);

/* Thread function */
void*       Thread_work(void* rank);

/* List operations */
int         Insert(int value);
void        Print(void);
int         isMember(int value);
int         Delete(int value);
void        Free_list(void);
int         Is_empty(void);

int * numList = NULL;
long keys;

/*-----------------------------------------------------------------*/
int main(int argc, char* argv[]) {
   long i; 
   int key, success, attempts;
   pthread_t* thread_handles;
   pthread_mutex_init(&mutex,NULL);
   pthread_mutex_init(&head_mutex,NULL);
   pthread_mutex_init(&count_mutex,NULL);
   int inserts_in_main;
   unsigned seed = 1;
   double start, finish;

   if (argc != 2) Usage(argv[0]);
   thread_count = strtol(argv[1],NULL,10);

   Get_input(&inserts_in_main);

   int * keyList = malloc(2*inserts_in_main * sizeof(int));
   /* Try to insert inserts_in_main keys, but give up after */
   /* 2*inserts_in_main attempts.                           */
   keys = attempts = 0;
   while ( keys < inserts_in_main && attempts < 2*inserts_in_main ) {
      key = my_rand(&seed) % MAX_KEY;
      success = Insert(key);
      attempts++;
      if (success) {
    	  keyList[keys] = key;
    	  keys++;
      }
   }
   printf("Inserted %ld keys in empty list\n", keys);


   /* Keep these generated key values in the array numList
    * In working thread, we reuse these key values to observe the occurrence of race condition
    */
   numList = malloc(keys * sizeof(int));
   for (int pos = 0; pos < keys; pos++)
	   numList[pos] = keyList[pos];

#  ifdef OUTPUT
   printf("Before starting threads, list = \n");
   Print();
   printf("\n");
#  endif

   thread_handles = malloc(thread_count*sizeof(pthread_t));

   GET_TIME(start);

   for (i = 0; i < thread_count; i++)
      pthread_create(&thread_handles[i], NULL, Thread_work, (void*) i);

   for (i = 0; i < thread_count; i++)
      pthread_join(thread_handles[i], NULL);
   GET_TIME(finish);
   printf("Elapsed time = %e seconds\n", finish - start);
   printf("Total ops = %d\n", total_ops);
   printf("member ops = %d\n", member_total);
   printf("insert ops = %d\n", insert_total);
   printf("delete ops = %d\n", delete_total);

   Print();
   //fflush(stdout);
#  ifdef OUTPUT
   printf("After threads terminate, list = \n");
   Print();
   printf("\n");
#  endif

   Free_list();
   pthread_mutex_destroy(&mutex);
   pthread_mutex_destroy(&count_mutex);
   free(thread_handles);

   return 0;
}  /* main */


/*-----------------------------------------------------------------*/
void Usage(char* prog_name) {
   fprintf(stderr, "usage: %s <thread_count>\n", prog_name);
   exit(0);
}  /* Usage */

/*-----------------------------------------------------------------*/
void Get_input(int* inserts_in_main_p) {

   printf("How many keys should be inserted in the main thread?\n");
   scanf("%d", inserts_in_main_p);
   printf("How many total ops should be executed?\n");
   scanf("%d", &total_ops);
   printf("Percent of ops that should be searches? (between 0 and 1)\n");
   scanf("%lf", &search_percent);
   printf("Percent of ops that should be inserts? (between 0 and 1)\n");
   scanf("%lf", &insert_percent);
   delete_percent = 1.0 - (search_percent + insert_percent);
}  /* Get_input */

/*-----------------------------------------------------------------*/
/* Insert value in correct numerical location into list */
/* If value is not in list, return 1, else return 0 */
int Insert(int value) {
   pthread_mutex_lock(&head_mutex);
   struct list_node_s* curr = head;
   struct list_node_s* pred = NULL;
   //pthread_mutex_lock(&(curr->mutex));
   //pthread_mutex_lock(&(pred->mutex));
   pthread_mutex_unlock(&head_mutex);
   struct list_node_s* temp;
   int rv = 1;
   
   while (curr != NULL && curr->data < value) {
	   pthread_mutex_lock(&(curr->mutex));
      pred = curr;
      curr = curr->next;
      pthread_mutex_unlock(&(pred->mutex));
   }

   if (curr == NULL || curr->data > value) {
      temp = malloc(sizeof(struct list_node_s));
      pthread_mutex_init(&(temp->mutex),NULL);
      pthread_mutex_lock(&(temp->mutex));
      temp->data = value;
      temp->next = curr;

      if (pred == NULL){
    	  pthread_mutex_lock(&head_mutex);
         head = temp;
      	  pthread_mutex_unlock(&(temp->mutex));
      	  pthread_mutex_unlock(&head_mutex);
      }
      else{
    	  pthread_mutex_lock(&pred->mutex);
         pred->next = temp;
         pthread_mutex_unlock(&(pred->mutex));
//         pthread_mutex_unlock(&(curr->mutex));
         pthread_mutex_unlock(&(temp->mutex));
      }
   } else { /* value in list */
	   //pthread_mutex_unlock(&(curr->mutex));
       rv = 0;
   }

   return rv;
}  /* Insert */

/*-----------------------------------------------------------------*/
void Print(void) {
   struct list_node_s* temp;

   printf("list = ");

   temp = head;
   while (temp != (struct list_node_s*) NULL) {
      printf("%d ", temp->data);
      temp = temp->next;
   }
   printf("\n");
}  /* Print */


/*-----------------------------------------------------------------*/
int  isMember(int value) {
   struct list_node_s* temp, *prevTemp;
   //printf("check at here for value: %f",value);
   //pthread_mutex_lock(&head_p_mutex);
   pthread_mutex_lock(&head_mutex);

   temp = head;
   pthread_mutex_lock(&(temp->mutex));
   pthread_mutex_unlock(&head_mutex);

   while (temp != NULL && temp->data < value){
	   if (temp->next!=NULL){
		   pthread_mutex_lock(&(temp->next->mutex));
	   }

	  prevTemp = temp;
      temp = temp->next;
      pthread_mutex_unlock(&(prevTemp->mutex));
   }
   //pthread_mutex_lock(&(temp->mutex));
   if (temp == NULL || temp->data > value) {
#     ifdef DEBUG
      printf("%d is not in the list\n", value);
#     endif
      if (temp!=NULL)
      pthread_mutex_unlock(&(temp->mutex));
      return 0;
   } else {
#     ifdef DEBUG
      printf("%d is in the list\n", value);
#     endif
      pthread_mutex_unlock(&(temp->mutex));
      return 1;
   }
}  /* isMember */
/*-----------------------------------------------------------------*/
/* Deletes value from list */
/* If value is in list, return 1, else return 0 */
int Delete(int value) {
	pthread_mutex_lock(&head_mutex);
   struct list_node_s* curr = head;
   struct list_node_s* pred = NULL;
   pthread_mutex_unlock(&head_mutex);
   int rv = 1;

   /* Find value */
   while (curr != NULL && curr->data < value) {
	   pthread_mutex_lock(&(curr->mutex));
      pred = curr;
      curr = curr->next;
      pthread_mutex_unlock(&(pred->mutex));
   }
   
   if (curr != NULL && curr->data == value) {
      if (pred == NULL) { /* first element in list */
    	  pthread_mutex_lock(&head_mutex);
         head = curr->next;
#        ifdef DEBUG
         printf("Freeing %d\n", value);
#        endif
         pthread_mutex_destroy(&(curr->mutex));
         pthread_mutex_unlock(&head_mutex);
         free(curr);
      } else { 
    	  pthread_mutex_lock(&(curr->mutex));
    	  pthread_mutex_lock(&(pred->mutex));
         pred->next = curr->next;
#        ifdef DEBUG
         printf("Freeing %d\n", value);
#        endif
		  pthread_mutex_unlock(&(pred->mutex));
		  pthread_mutex_unlock(&(curr->mutex));
		  pthread_mutex_destroy(&(curr->mutex));
         free(curr);
      }
   } else { /* Not in list */
      rv = 0;
   }

   return rv;
}  /* Delete */

/*-----------------------------------------------------------------*/
void Free_list(void) {
   struct list_node_s* current;
   struct list_node_s* following;

   if (Is_empty()) return;
   current = head; 
   following = current->next;
   while (following != NULL) {
#     ifdef DEBUG
      printf("Freeing %d\n", current->data);
#     endif
      free(current);
      current = following;
      following = current->next;
   }
#  ifdef DEBUG
   printf("Freeing %d\n", current->data);
#  endif
   free(current);
}  /* Free_list */

/*-----------------------------------------------------------------*/
int  Is_empty(void) {
   if (head == NULL)
      return 1;
   else
      return 0;
}  /* Is_empty */

/*-----------------------------------------------------------------*/
void* Thread_work(void* rank) {
   long my_rank = (long) rank;
   int i, val;
   double which_op;
   unsigned seed = my_rank + 1;
   int my_member=0, my_insert=0, my_delete=0;
   int ops_per_thread = total_ops/thread_count;

   for (i = 0; i < ops_per_thread; i++) {
	  which_op = my_drand(&seed);

	  val = numList[keys - 1]; //my_rand(&seed) % keys]; //MAX_KEY;
	  if (which_op < search_percent) {
			//pthread_mutex_lock(&mutex);
		   //printf("check %f",val);
		   //fflush(stdout);
			isMember(val);
			//pthread_mutex_unlock(&mutex);
			my_member++;
	  } else if (which_op < search_percent + insert_percent) {
			//pthread_mutex_lock(&mutex);
			Insert(val);
			//pthread_mutex_unlock(&mutex);
			my_insert++;
	  } else { /* delete */
		  	//pthread_mutex_lock(&mutex);
			 Delete(val);
			// pthread_mutex_unlock(&mutex);
			 my_delete++;
	  }
   }  /* for */
   pthread_mutex_lock(&count_mutex);
   member_total += my_member;
   insert_total += my_insert;
   delete_total += my_delete;
   pthread_mutex_unlock(&count_mutex);

   return NULL;
}  /* Thread_work */
