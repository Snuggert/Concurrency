/*
 * assign3_1.c
 *
 * Contains code for setting up and finishing the simulation.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <mpi.h>


#include "file.h"
#include "timer.h"
#include "simulate.h"

typedef double (*func_t)(double x);

/*
 * Simple gauss with mu=0, sigma^1=1
 */
double gauss(double x)
{
    return exp((-1 * x * x) / 2);
}


/*
 * Fills a given array with samples of a given function. This is used to fill
 * the initial arrays with some starting data, to run the simulation on.
 *
 * The first sample is placed at array index `offset'. `range' samples are
 * taken, so your array should be able to store at least offset+range doubles.
 * The function `f' is sampled `range' times between `sample_start' and
 * `sample_end'.
 */
void fill(double *array, int offset, int range, double sample_start,
        double sample_end, func_t f)
{
    int i;
    float dx;

    dx = (sample_end - sample_start) / range;
    for (i = 0; i < range; i++) {
        array[i + offset] = f(sample_start + i * dx);
    }
}


int main(int argc, char *argv[])
{
    int myid, numprocs;
    MPI_Status status;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD,&myid);
    MPI_Comm_size(MPI_COMM_WORLD,&numprocs);
    MPI_Request reqs[numprocs - 1];

    double *part_old, *part_current, *part_result, *result, time;
    int part_size, t_max, i_max;
    // serial
    if(myid == 0){
        double *old, *current;

        /* Parse commandline args */
        if (argc < 3) {
            printf("Usage: %s i_max t_max num_threads [initial_data]\n", argv[0]);
            printf(" - i_max: number of discrete amplitude points, should be >2\n");
            printf(" - t_max: number of discrete timesteps, should be >=1\n");
            printf(" - initial_data: select what data should be used for the first "
                    "two generation.\n");
            printf("   Available options are:\n");
            printf("    * sin: one period of the sinus function at the start.\n");
            printf("    * sinfull: entire data is filled with the sinus.\n");
            printf("    * gauss: a single gauss-function at the start.\n");
            printf("    * file <2 filenames>: allows you to specify a file with on "
                    "each line a float for both generations.\n");

            return EXIT_FAILURE;
        }

        i_max = atoi(argv[1]);
        t_max = atoi(argv[2]);

        result = calloc(i_max, sizeof(double));

        if (i_max < 3) {
            printf("argument error: i_max should be >2.\n");
            return EXIT_FAILURE;
        }
        if (t_max < 1) {
            printf("argument error: t_max should be >=1.\n");
            return EXIT_FAILURE;
        }

        /* Allocate and initialize buffers. */
        old = malloc(i_max * sizeof(double));
        current = malloc(i_max * sizeof(double));

        if (old == NULL || current == NULL) {
            fprintf(stderr, "Could not allocate enough memory, aborting.\n");
            return EXIT_FAILURE;
        }

        memset(old, 0, i_max * sizeof(double));
        memset(current, 0, i_max * sizeof(double));

        /* How should we will our first two generations? This is determined by the
         * optional further commandline arguments.
         * */
        if (argc > 3) {
            if (strcmp(argv[3], "sin") == 0) {
                fill(old, 1, i_max/4, 0, 2*3.14, sin);
                fill(current, 2, i_max/4, 0, 2*3.14, sin);
            } else if (strcmp(argv[3], "sinfull") == 0) {
                fill(old, 1, i_max-2, 0, 10*3.14, sin);
                fill(current, 2, i_max-3, 0, 10*3.14, sin);
            } else if (strcmp(argv[3], "gauss") == 0) {
                fill(old, 1, i_max/4, -3, 3, gauss);
                fill(current, 2, i_max/4, -3, 3, gauss);
            } else if (strcmp(argv[3], "file") == 0) {
                if (argc < 6) {
                    printf("No files specified!\n");
                    return EXIT_FAILURE;
                }
                file_read_double_array(argv[4], old, i_max);
                file_read_double_array(argv[5], current, i_max);
            } else {
                printf("Unknown initial mode: %s.\n", argv[3]);
                return EXIT_FAILURE;
            }
        } else {
            /* Default to sinus. */
            fill(old, 1, i_max/4, 0, 2*3.14, sin);
            fill(current, 2, i_max/4, 0, 2*3.14, sin);
        }

        int index = 0;
        for(int i = 0; i < numprocs - 1; i++){
            part_size = ((i_max*i)+i_max) / numprocs - (i_max*i) 
                    / numprocs;

            /* send sizes and the part of old array */
            MPI_Send(&part_size, 1, MPI_INT, i + 1, 0, MPI_COMM_WORLD);
            MPI_Send(&t_max, 1, MPI_INT, i + 1, 1, MPI_COMM_WORLD);
            MPI_Send(&old[index], part_size, MPI_DOUBLE, i + 1, 2, 
                    MPI_COMM_WORLD);
            /* send part of current array */
            MPI_Send(&current[index], part_size, MPI_DOUBLE, i + 1, 3, 
                    MPI_COMM_WORLD);
            
            /* master receives results into result */
            MPI_Irecv(&result[index], part_size, MPI_DOUBLE, i + 1, 0,
                MPI_COMM_WORLD, &reqs[i]);
            index += part_size;
        }

        /* give master the last parts*/
        part_size = ((i_max*numprocs)+i_max) / numprocs - (i_max*numprocs) 
                    / numprocs;
        part_old = (double *)calloc(part_size, sizeof(double));
        memcpy(part_old, old + index, part_size);
        part_current = (double *)calloc(part_size, sizeof(double));
        memcpy(part_current, current + index, part_size);

        free(old);
        free(current);
        timer_start();
    }
    else{
        /* receive t and part_size */
        MPI_Recv(&part_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, &status);
        MPI_Recv(&t_max, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &status);

        part_old = calloc(part_size, sizeof(double));
        part_current = calloc(part_size, sizeof(double));
        /* receive parts for processing */
        MPI_Recv(&part_old[0], part_size, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD, 
                &status);
        MPI_Recv(&part_current[0], part_size, MPI_DOUBLE, 0, 3, MPI_COMM_WORLD, 
                &status);
    }

    int left_neighbor, right_neighbor;

    left_neighbor = myid - 1;
    if(left_neighbor < 0)
        left_neighbor += numprocs;
    
    right_neighbor = (myid + 1) % numprocs;

    // parallel
    /* Call the actual simulation that should be implemented in simulate.c. */
    part_result = (double *)calloc(part_size, sizeof(double));
    part_result = simulate(part_size, t_max, part_old, part_current,
            part_result, left_neighbor, right_neighbor, status);

    // serial
    if(myid == 0){
        /* copy results of masters work into result */
        memcpy(result + i_max - part_size, part_result, part_size);
        /* wait for all processes to complete writing to results */
        MPI_Waitall(numprocs - 1, &reqs[0], &status);
        
        time = timer_end();
        printf("Took %g seconds\n", time);
        printf("Normalized: %g seconds\n", time / (1. * i_max * t_max));

        file_write_double_array("result.txt", result, i_max);

    }
    else{
        /* send results to master */
        MPI_Send(&part_result[0], part_size, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);        
    }

    MPI_Finalize();
    return EXIT_SUCCESS;
}
