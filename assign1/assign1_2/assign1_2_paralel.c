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
    pthread_mutex_t mutex;
}queue_t;

// Put new value in queue.
void queue_enqueue(queue_t *queue, void *value){

}
 
// Get value from queue.
long queue_dequeue(queue_t * queue){

}

// This is where you check if a number is divisible by the threads first 
// inputted number. If a number is not divisible you send it to the output queue
// which is connected with the input queue of the next thread.
void *check_number(void *a){

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