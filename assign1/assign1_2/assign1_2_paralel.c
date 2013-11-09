#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include <stdbool.h>

long max_queue_size = 10000;

typedef struct{
    long *buffer;
    long size;
    long max_size;

    long index;
    long outdex;

    pthread_mutex_t *mutex;
    pthread_cond_t  *full;
    pthread_cond_t  *empty;
}queue_t;

/* Put new value in queue. */
void queue_enqueue(queue_t *queue, long value){
    pthread_mutex_lock(queue->mutex);
    while (queue->size == queue->max_size){
        pthread_cond_wait(queue->full, queue->mutex);
    }
    queue->buffer[queue->index] = value;
    queue->size ++;
    queue->index ++;
    queue->index %= queue->max_size;
    pthread_mutex_unlock(queue->mutex);
    pthread_cond_broadcast(queue->empty);
}
 
/* Get value from queue. */
long queue_dequeue(queue_t * queue){
    pthread_mutex_lock(queue->mutex);
    while (queue->size == 0){
        pthread_cond_wait(queue->empty, queue->mutex);
    }
    long value = queue->buffer[queue->outdex];
    queue->size --;
    queue->outdex ++;
    queue->outdex %= queue->max_size;
    pthread_mutex_unlock(queue->mutex);
    pthread_cond_broadcast(queue->full);
    return value;
}

queue_t *init_queue(long max_size){
    queue_t *queue;
    queue = malloc(sizeof(queue_t));

    queue->buffer = malloc(sizeof(long) * max_size);
    queue->size = 0;
    queue->max_size = max_size;

    queue->index = 0;
    queue->outdex = 0;

    queue->mutex = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(queue->mutex, NULL);

    queue->full = malloc(sizeof(pthread_cond_t));
    pthread_cond_init(queue->full, NULL);

    queue->empty = malloc(sizeof(pthread_cond_t));
    pthread_cond_init(queue->empty, NULL);

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
    printf("new prime number: %ld\n", prime);

    long nat_number = 0;
    int newThread = 0;

    /* Search for the first prime to initizalize child with. */
    while(!newThread){
        nat_number = queue_dequeue(inbound_queue);
        if(nat_number % prime != 0){
            newThread = 1; 
            outbound_queue = init_queue(max_queue_size);
            queue_enqueue(outbound_queue, nat_number);
        }
    }

    /* Init child filter. */
    pthread_t thread;
    pthread_create ( &thread,               /* returned thread ids */
                    NULL ,                  /* default attributes */
                    &check_number ,         /* start routine */
                    outbound_queue);       /* Que argument */

    /* Keep filtering all the numbers from the previous queue. */
    while(1){
        nat_number = queue_dequeue(inbound_queue);
        if(nat_number % prime != 0){
            queue_enqueue(outbound_queue, nat_number);
        }        
    }

}

int main(int argc, char *argv[]){
    long n = 3;
    pthread_t thread_one;
    queue_t *outbound_queue = init_queue(max_queue_size);
    queue_enqueue(outbound_queue, n);

    pthread_create (&thread_one,           /* returned thread ids */
                    NULL ,                  /* default attributes */
                    &check_number ,         /* start routine */
                    outbound_queue);       /* args to be further specified. */
    
    do{
        n += 2;
        queue_enqueue(outbound_queue, n);
    }while(true);
}