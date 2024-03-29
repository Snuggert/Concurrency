/*
 * simulate.c
 *
 * Implement your (parallel) simulation here!
 *
 *
 * Authors: Abe Wiersma, Bram van den Akker
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
}; 

struct Data data;

struct Args
{
	int begin;
	int size;
	int id;
};

/* Add any functions you may need (like a worker) here. */
void *wave_thread(void *a){
	struct Args *arg = (struct Args*)a;

	/* Constant c that defines the spatial impact. */
	float c = 0.2;

	/* Do all iterations assigned to the worker thread. */
	for(int i = arg->begin; i < (arg->size + arg->begin); i++){
		/* First and last iterations should be 0; */
		if(i <= 0 || i == data.i_max - 2){
			data.next_array[i] = 0;
		}else{
			data.next_array[i] = (2.0 * data.current_array[i]) - data.old_array[i] +
					(c * (data.current_array[i-1] - (2.0 * data.current_array[i]) + 
					data.current_array[i+1]));
		}
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
	int worker_size, workerthreads, i, t;
	workerthreads = num_threads - 1;
	/*
	 * After each timestep, you should swap the buffers around. Watch out none
	 * of the threads actually use the buffers at that time.
	 */
	worker_size = (int)(i_max / num_threads);

	/* Fill up the global struct. */
	data.old_array = old_array;
	data.current_array = current_array;
	data.next_array = next_array;
	data.i_max = i_max;

	pthread_t thread_ids [ num_threads ];
	void * results [ num_threads ];

	/* This is where the master threads args are defined. */
	struct Args master_arg;
	master_arg.begin = workerthreads * worker_size;
	master_arg.size = i_max - (workerthreads) * worker_size;
	master_arg.id = workerthreads;

	/* This is where the args for the worker threads are defined. */
	struct Args args[workerthreads];
	for(i = 0; i < workerthreads; i++){
		struct Args arg;
		arg.begin = worker_size * i;
		arg.size = worker_size;
		arg.id = i;
		args[i] = arg;
	}

	/* This is where the loop for time t starts. */
	for(t = 0; t < t_max; t++){
		for (i =0; i < workerthreads; i++){
			pthread_create ( & thread_ids [i] ,	/* returned thread ids */
					NULL ,						/* default attributes */
					&wave_thread ,				/* start routine */
					&args[i]);					/* argument */
		}

		/* This is where the master_thread starts its own work. */
		wave_thread(&master_arg);

		/*
		 *  This is where the master waits for all the threads to finish work on the 
		 * next_array. */
		for (i =0; i < workerthreads; i ++) {
			if(pthread_join( thread_ids[i], &results[i]) == -1){
				printf("Error joining thread %d\n", i);           
			}

		
}		/* 
		 * This is where you switch current array to old_array and next_array to
		 * current_array. */
		double *tmp = data.old_array;
		data.old_array = data.current_array;
		data.current_array = data.next_array;

		/* Reuse the old array */
		data.next_array = tmp;
	}
	
	/* You should return a pointer to the array with the final results. */
	/* This is also where we free our globals that are not returned. */

	return data.current_array;
}
