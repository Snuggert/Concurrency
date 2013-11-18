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

	float c = 0.2;

	/* This is where the loop for time t starts. */
	for(int t = 0; t < t_max; t++){

		/* Do all iterations assigned to the worker thread. */
		#pragma omp parallel for firstprivate ( next_array , current_array , \
		 		old_array) lastprivate ( next_array , current_array , \
		 		old_array) schedule ( static, 1 ) num_threads(num_threads)
		for(int i = 0; i < i_max; i++){
			/* First and last iterations should be 0; */
			if(i <= 0 || i == i_max - 2){
				next_array[i] = 0;
			}else{
				next_array[i] = (2.0 * current_array[i]) - old_array[i] +
						(c * (current_array[i-1] - (2.0 * current_array[i]) + 
						current_array[i+1]));
			}
		}	

		/* 
		 * This is where you switch current array to old_array and next_array to
		 * current_array. */
		double *tmp = old_array;
		old_array = current_array;
		current_array = next_array;

		/* Reuse the old array */
		next_array = tmp;
	}
	
	/* You should return a pointer to the array with the final results. */
	/* This is also where we free our globals that are not returned. */

	return current_array;
}
