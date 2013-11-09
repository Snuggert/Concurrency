#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <limits.h>
#include <stdbool.h>

long max_queue_size = 10000;

typedef struct{
    long *buffer;
    long size;
    long max_size;

    pthread_mutex_t mutex;
    pthread_cond_t full;
    pthread_cond_t empty;
}queue_t;

/* Put new value in queue. */
void queue_enqueue(queue_t *queue, long *value){
    pthread_mutex_lock(&(queue->mutex));
    while (queue->size == queue->max_size){
        pthread_cond_wait(&(queue->full), &(queue->mutex));
    }
}
 
/* Get value from queue. */
long queue_dequeue(queue_t * queue){
    return 1;
}

queue_t *init_queue(long max_size){
    queue_t *queue = malloc(sizeof(struct queue_t *));

    queue->buffer = malloc(sizeof(long) * max_size);
    queue->size = 0;
    queue->max_size = max_size;

    return queue;
}

/* 
 * This is where you check if a number is divisible by the threads first 
 * inputted number. If a number is not divisible you send it to the output queue
 * which is connected with the input queue of the next thread.
 */
void *check_number(void *a){
    queue_t *inbound_queue = (queue_t*)a;

    queue_t *outbound_queue;

    long prime = queue_dequeue(inbound_queue);
    printf("new prime number: %ld", prime);

    long nat_number = 0;
    int newThread = 0;

    /* Search for the first prime to initizalize child with. */
    while(!newThread){
        nat_number = queue_dequeue(inbound_queue);
        if(nat_number % prime != 0){
            newThread = 1; 
            outbound_queue = init_queue(max_queue_size);
            queue_enqueue(outbound_queue, &nat_number);
        }
    }

    /* Init child filter. */
    pthread_t thread;
    pthread_create ( &thread,               /* returned thread ids */
                    NULL ,                  /* default attributes */
                    &check_number ,         /* start routine */
                    &outbound_queue);       /* Que argument */

    /* Keep filtering all the numbers from the previous queue. */
    while(1){
        nat_number = queue_dequeue(inbound_queue);
        if(nat_number % prime != 0){
            queue_enqueue(outbound_queue, &nat_number);
        }        
    }

}

int main(int argc, char *argv[]){
    long n = 3;
    pthread_t thread_one;
    queue_t *outbound_queue = init_queue(max_queue_size);
    queue_enqueue(outbound_queue, &n);


    pthread_create ( &thread_one,           /* returned thread ids */
                    NULL ,                  /* default attributes */
                    &check_number ,         /* start routine */
                    &outbound_queue);       /* args to be further specified. */
    
    do{
        n += 2;
        queue_enqueue(outbound_queue, &n);
    }while(1);
    queue_t *queue = init_queue(10);

}