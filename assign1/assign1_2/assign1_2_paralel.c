#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <limits.h>
#include <stdbool.h>

typedef struct{
    long *buffer;
    long size;
    long max_size;

    pthread_mutex_t mutex;
    pthread_cond_t full;
    pthread_cond_t empty;
}queue_t;

// Put new value in queue.
void queue_enqueue(queue_t *queue, long *value){
    pthread_mutex_lock(&(queue->mutex));
    while (queue->size == queue->max_size){
        pthread_cond_wait(&(queue->full), &(queue->mutex));
    }

}
 
// Get value from queue.
long queue_dequeue(queue_t * queue){

}

// This is where you check if a number is divisible by the threads first 
// inputted number. If a number is not divisible you send it to the output queue
// which is connected with the input queue of the next thread.
void *check_number(void *a){
    printf("new prime number: %d", a->prime);
}

int main(int argc, char *argv[]){
    long n = 3;
    pthread_t thread_one;
    pthread_create ( &thread_one,           /* returned thread ids */
                    NULL ,                  /* default attributes */
                    &check_number ,          /* start routine */
                    // args to be further specified.);  
    do{

    }while(true)

}