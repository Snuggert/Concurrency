#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <iostream>

#include "file.h"
#include "timer.h"

using namespace std;

typedef double(*func_t)(double x);

/*
 * Simple gauss with mu=0, sigma^1=1
 */
double gauss(double x)
{
    return exp((-1 * x * x) / 2);
}

void fill(float *array, int offset, int range, float sample_start,
        float sample_end, func_t f)
{
    int i;
    float dx;

    dx = (sample_end - sample_start) / range;
    for (i = 0; i < range; i++) {
        array[i + offset] = f(sample_start + i * dx);
    }
}

/* Utility function, use to do error checking.

   Use this function like this:

   checkCudaCall(cudaMalloc((void **) &deviceRGB, imgS * sizeof(color_t)));

   And to check the result of a kernel invocation:

   checkCudaCall(cudaGetLastError());
*/
static void checkCudaCall(cudaError_t result) {
    if (result != cudaSuccess) {
        cerr << "cuda error: " << cudaGetErrorString(result) << endl;
        exit(1);
    }
}


__global__ void vectorAddKernel(float* deviceA, float* deviceB, float* deviceResult) {
    unsigned index = blockIdx.x * blockDim.x + threadIdx.x;
    deviceResult[index] = deviceA[index] + deviceB[index];
}

__global__ void waveStep(int N, float* old, float* current, float* next){
    unsigned i = blockIdx.x * blockDim.x + threadIdx.x + 1;
    if (i < (N - 1)){
        next[i] = (2.0 * current[i]) - old[i] +
            (0.2 * (current[i-1] - (2.0 * current[i]) + 
            current[i+1]));
    }
}

void simulateCuda(int n, int max_t, float* old, float* current, float* next){
    int threadBlockSize = 512;

    // allocate the vectors on the GPU
    float* deviceOld = NULL;
    checkCudaCall(cudaMalloc((void **) &deviceOld, n * sizeof(float)));
    if (deviceOld == NULL) {
        cout << "could not allocate memory!" << endl;
        return;
    }
    float* deviceCurrent = NULL;
    checkCudaCall(cudaMalloc((void **) &deviceCurrent, n * sizeof(float)));
    if (deviceCurrent == NULL) {
        checkCudaCall(cudaFree(deviceOld));
        cout << "could not allocate memory!" << endl;
        return;
    }
    float* deviceNext = NULL;
    checkCudaCall(cudaMalloc((void **) &deviceNext, n * sizeof(float)));
    if (deviceNext == NULL) {
        checkCudaCall(cudaFree(deviceOld));
        checkCudaCall(cudaFree(deviceCurrent));
        cout << "could not allocate memory!" << endl;
        return;
    }

    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    for(int i = 0; i < max_t; i++){

        // copy the original vectors to the GPU
        checkCudaCall(cudaMemcpy(deviceOld, old, n*sizeof(float), cudaMemcpyHostToDevice));
        checkCudaCall(cudaMemcpy(deviceCurrent, current, n*sizeof(float), cudaMemcpyHostToDevice));

        // execute kernel
        cudaEventRecord(start, 0);
        vectorAddKernel<<<n/threadBlockSize, threadBlockSize>>>(deviceOld, deviceCurrent, deviceNext);
        cudaEventRecord(stop, 0);

        // check whether the kernel invocation was successful
        checkCudaCall(cudaGetLastError());

        /* Copy results to old and current */
        checkCudaCall(cudaMemcpy(deviceOld, deviceCurrent, n*sizeof(float), cudaMemcpyDeviceToDevice));
        checkCudaCall(cudaMemcpy(deviceCurrent, deviceNext, n*sizeof(float), cudaMemcpyDeviceToDevice));
    }
    // copy result back
    checkCudaCall(cudaMemcpy(next, deviceNext, n * sizeof(float), cudaMemcpyDeviceToHost));

    checkCudaCall(cudaFree(deviceOld));
    checkCudaCall(cudaFree(deviceCurrent));
    checkCudaCall(cudaFree(deviceNext));

    // print the time the kernel invocation took, without the copies!
    float elapsedTime;
    cudaEventElapsedTime(&elapsedTime, start, stop);
    
    cout << "kernel invocation took " << elapsedTime << " milliseconds" << endl;

}


void vectorAddCuda(int n, float* a, float* b, float* result) {
    int threadBlockSize = 512;

    // allocate the vectors on the GPU
    float* deviceA = NULL;
    checkCudaCall(cudaMalloc((void **) &deviceA, n * sizeof(float)));
    if (deviceA == NULL) {
        cout << "could not allocate memory!" << endl;
        return;
    }
    float* deviceB = NULL;
    checkCudaCall(cudaMalloc((void **) &deviceB, n * sizeof(float)));
    if (deviceB == NULL) {
        checkCudaCall(cudaFree(deviceA));
        cout << "could not allocate memory!" << endl;
        return;
    }
    float* deviceResult = NULL;
    checkCudaCall(cudaMalloc((void **) &deviceResult, n * sizeof(float)));
    if (deviceResult == NULL) {
        checkCudaCall(cudaFree(deviceA));
        checkCudaCall(cudaFree(deviceB));
        cout << "could not allocate memory!" << endl;
        return;
    }

    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);

    // copy the original vectors to the GPU
    checkCudaCall(cudaMemcpy(deviceA, a, n*sizeof(float), cudaMemcpyHostToDevice));
    checkCudaCall(cudaMemcpy(deviceB, b, n*sizeof(float), cudaMemcpyHostToDevice));

    // execute kernel
    cudaEventRecord(start, 0);
    vectorAddKernel<<<n/threadBlockSize, threadBlockSize>>>(deviceA, deviceB, deviceResult);
    cudaEventRecord(stop, 0);

    // check whether the kernel invocation was successful
    checkCudaCall(cudaGetLastError());

    // copy result back
    checkCudaCall(cudaMemcpy(result, deviceResult, n * sizeof(float), cudaMemcpyDeviceToHost));

    checkCudaCall(cudaFree(deviceA));
    checkCudaCall(cudaFree(deviceB));
    checkCudaCall(cudaFree(deviceResult));

    // print the time the kernel invocation took, without the copies!
    float elapsedTime;
    cudaEventElapsedTime(&elapsedTime, start, stop);
    
    cout << "kernel invocation took " << elapsedTime << " milliseconds" << endl;
}


int main(int argc, char* argv[]) {
    float *old, *current, *next;
    timer vectorAddTimer("vector add timer");
    int t_max, i_max, num_threads;

    /* Parse commandline args: i_max t_max num_threads */
    if (argc < 4) {
        printf("Usage: %s i_max t_max num_threads [initial_data]\n", argv[0]);
        printf(" - i_max: number of discrete amplitude points, should be >2\n");
        printf(" - t_max: number of discrete timesteps, should be >=1\n");
        printf(" - num_threads: number of threads to use for simulation, "
                "should be >=1\n");
        printf(" - initial_data: select what data should be used for the first "
                "two generation.\n");
        printf("   Available options are:\n");
        printf("    * sin: one period of the sinus function at the start.\n");
        printf("    * sinfull: entire data is filled with the sinus.\n");
        printf("    * gauss: a single gauss-function at the start.\n");
        printf("    * file <2 filenames>: allows you to specify a file with on "
                "each line a float for both generations.\n");

        exit(1);
    }

    i_max = atoi(argv[1]);
    t_max = atoi(argv[2]);
    num_threads = atoi(argv[3]);

    if (i_max < 3) {
        printf("argument error: i_max should be >2.\n");
        exit(1);
    }
    if (t_max < 1) {
        printf("argument error: t_max should be >=1.\n");
        exit(1);
    }
    if (num_threads < 1) {
        printf("argument error: num_threads should be >=1.\n");
        exit(1);
    }

    /* Allocate and initialize buffers. */
    old = new float[i_max];
    current = new float[i_max];
    next = new float[i_max];

    if (old == NULL || current == NULL || next == NULL) {
        fprintf(stderr, "Could not allocate enough memory, aborting.\n");
        exit(1);
    }

    memset(old, 0, i_max * sizeof(float));
    memset(current, 0, i_max * sizeof(float));
    memset(next, 0, i_max * sizeof(float));

    /* How should we will our first two generations? */
    if (argc > 4) {
        if (strcmp(argv[4], "sin") == 0) {
            fill(old, 1, i_max/4, 0, 2*3.14, sin);
            fill(current, 2, i_max/4, 0, 2*3.14, sin);
        } else if (strcmp(argv[4], "sinfull") == 0) {
            fill(old, 1, i_max-2, 0, 10*3.14, sin);
            fill(current, 2, i_max-3, 0, 10*3.14, sin);
        } else if (strcmp(argv[4], "gauss") == 0) {
            fill(old, 1, i_max/4, -3, 3, gauss);
            fill(current, 2, i_max/4, -3, 3, gauss);
        } else if (strcmp(argv[4], "file") == 0) {
            if (argc < 7) {
                printf("No files specified!\n");
                exit(1);
            }
            file_read_float_array(argv[5], old, i_max);
            file_read_float_array(argv[6], current, i_max);
        } else {
            printf("Unknown initial mode: %s.\n", argv[4]);
            exit(1);
        }
    } else {
        /* Default to sinus. */
        fill(old, 1, i_max/4, 0, 2*3.14, sin);
        fill(current, 2, i_max/4, 0, 2*3.14, sin);
    }

    vectorAddTimer.start();
    vectorAddTimer.stop();

    cout << vectorAddTimer;
    cout << "results OK!" << endl;
            
    delete[] old;
    delete[] current;
    delete[] next;
    
    return 0;
}
