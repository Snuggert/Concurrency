/*
 * simulate.c
 *
 * Implement your (parallel) simulation here!
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "simulate.h"


/* Add any global variables you may need. */
struct Data
{
    double *old_array;
    double *current_array;
    double *next_array;
    int i_max;
    int worker_size;
}; 

struct Data data;

/* Add any functions you may need (like a worker) here. */
void *wave_thread(void *a){
	int thread_id =  (unsigned int)pthread_self();

	printf("thread id %d\n", thread_id);

	int prev;
	int next;

	for(int i = begin; i < end; i++){
		if(i < 0){
			prev = data.i_max;
		}else{
			prev = i;
		}

		if(i >= data.i_max){
			next = 0;
		}else{
			next = i;
		}

		data->next_array[i] = (2.0F * data->current_array[i]) -
				data->old_array[i] + 0.2F * (data->current_array[prev] - 
				(2.0F * data->current_array[i], data->current_array[next]));
	}


	return NULL;
}

/*
 * Executes the entire simulation.
 *
 * Implement your code here.
 *
 * i_max: how many data points are on a single wave
 * t_max: how many iterations the simulation should run
 * num_threads: how many threads to use (excluding the main threads)
 * old_array: array of size i_max filled with data for t-1
 * current_array: array of size i_max filled with data for t
 * next_array: array of size i_max. You should fill this with t+1
 */
double *simulate(const int i_max, const int t_max, const int num_threads,
        double *old_array, double *current_array, double *next_array)
{


    int master_size;
    int worker_size;
    /*
     * After each timestep, you should swap the buffers around. Watch out none
     * of the threads actually use the buffers at that time.
     */
    worker_size = (int)(i_max / num_threads);
    master_size = i_max - (num_threads - 1) * worker_size;

    data.old_array = old_array;
    data.current_array = current_array;
    data.next_array = next_array;
    data.i_max = i_max;
    data.worker_size = worker_size;

	pthread_t thread_ids [ num_threads ];
	void * results [ num_threads ];

	int i;
	for (i =0; i < num_threads ; i ++) {
		printf("created %d\n", i);
		pthread_create ( & thread_ids [i] , /* returned thread ids */
				NULL , 						/* default attributes */
				&wave_thread , 				/* start routine */
				&i ); 					/* argument */
	}

	for (i =0; i < num_threads ; i ++) {
		pthread_join ( thread_ids [i], & results [i ]);
		printf("joined %d\n", i);
	}

    printf("worker_size: %d\n", data.worker_size);




    /* You should return a pointer to the array with the final results. */
    return current_array;
}
