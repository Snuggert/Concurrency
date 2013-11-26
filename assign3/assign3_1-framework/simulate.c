/*
 * simulate.c
 *
 * Implement your (parallel) simulation here!
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpi.h>

#include "simulate.h"

/*
 * Executes the entire simulation.
 *
 * Implement your code here.
 *
 * i_max: how many data points are on a single wave
 * t_max: how many iterations the simulation should run
 * old_array: array of size i_max filled with data for t-1
 * current_array: array of size i_max filled with data for t
 * next_array: array of size i_max. You should fill this with t+1
 */
double *simulate(const int i_max, const int t_max, double *old_array,
        double *current_array, double *next_array, int left_neighbor, 
        int right_neighbor, MPI_Status status){
    /*
     * Your implementation should go here.
     */
    float c = 0.2;
    int left_index, right_index;
    current_array = realloc(current_array, i_max + 2);
    for(int t = 0; t < t_max; t++){
        /* send to left_neighbor receive from right */
        MPI_Send(&current_array[0], 1, MPI_DOUBLE, left_neighbor, 1,
                MPI_COMM_WORLD);
        MPI_Recv(&current_array[i_max + 1], 1, MPI_DOUBLE, right_neighbor, 1,
                MPI_COMM_WORLD, &status);

        /* send to right_neighbor receive from left */
        MPI_Send(&current_array[i_max - 1], 1, MPI_DOUBLE, right_neighbor, 2,
                MPI_COMM_WORLD);
        MPI_Recv(&current_array[i_max], 1, MPI_DOUBLE, left_neighbor, 2,
                MPI_COMM_WORLD, &status);

        for(int i = 0; i < i_max; i++){
            if(i - 1 < 0){
                left_index = i_max;
            }else{
                left_index = i - 1;
            }
            if(i + 1 == i_max){
                right_index = i_max + 1;
            }else{
                right_index = i + 1;
            }

            next_array[i] = (2.0 * current_array[i]) - old_array[i] +
                    (c * (current_array[left_index] - (2.0 * current_array[i]) + 
                    current_array[right_index]));
        }
        memcpy(old_array, current_array, i_max);
        memcpy(current_array, next_array, i_max);
    }
    memcpy(next_array, current_array, i_max);
    free(current_array);
    free(old_array);
    return next_array;    
}
