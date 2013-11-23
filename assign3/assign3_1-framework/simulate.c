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
    printf("R:%d L:%d\n", right_neighbor, left_neighbor);
    float c = 0.2;
    double *new_array;
    new_array = calloc(i_max + 2, sizeof(double));
    memcpy(new_array + 1, current_array, i_max);
    free(current_array);
    current_array = new_array;
    for(int t = 0; t < t_max; t++){
        printf("ALL IS GOOD1\n");
        /* send to left_neighbor receive from right */
        if(MPI_SUCCESS == MPI_Send(&current_array[1], 1, MPI_DOUBLE, left_neighbor, t,
                MPI_COMM_WORLD)){
            printf("sending succes\n");
        }
        MPI_Recv(&current_array[i_max], 1, MPI_DOUBLE, right_neighbor, t,
                MPI_COMM_WORLD, &status);

        /* send to right_neighbor receive from left */
        if(MPI_SUCCESS == MPI_Send(&current_array[i_max - 1], 1, MPI_DOUBLE, right_neighbor, t+t_max,
                MPI_COMM_WORLD)){
            printf("sending succes\n");
        }
        MPI_Recv(&current_array[0], 1, MPI_DOUBLE, left_neighbor, t+t_max,
                MPI_COMM_WORLD, &status);

        printf("ALL IS GOOD2\n");

        for(int i = 0; i < i_max; i++){
            /* First and last iterations should be 0; */
            next_array[i] = (2.0 * current_array[i+1]) - old_array[i] +
                    (c * (current_array[i] - (2.0 * current_array[i+1]) + 
                    current_array[i+2]));
        }
        memcpy(old_array, current_array + 1, i_max);
        memcpy(current_array + 1, next_array, i_max);
    }
    memcpy(next_array, current_array + 1, i_max);
    free(current_array);
    free(old_array);
    return next_array;    
}
