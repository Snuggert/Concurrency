#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <iostream>
#include <time.h>

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

void fillRandom(float *array, int n)
{
    int i;
    srand(time(NULL));

    for (i = 0; i < n; i++) {
        array[i] = rand();
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

__global__ void cudaMax(int size, int gap, float* values){
    unsigned i = blockIdx.x * blockDim.x + threadIdx.x;
    if( ( (gap + i) < size) &&  values[gap + i] > values[i]){
	values[i] = values[gap + i];
    }
}

float max_value(int size, float* values){
    int max = 0; 
    for(int i = 0; i < size; i++){
	if(values[i] > max){
	    max = values[i];
        }
    }

    return max;
}


void calculateMax(int size, float* values){
    int threadBlockSize = 512;
    int gap = 1;
    float max = 0;
    int n = size; 

    // allocate the vectors on the GPU
    float* deviceValues = NULL;
    checkCudaCall(cudaMalloc((void **) &deviceValues, n * sizeof(float)));
    if (deviceValues == NULL) {
        cout << "could not allocate memory!" << endl;
        return;
    }

    cudaEvent_t start, stop;
    cudaEventCreate(&start);
    cudaEventCreate(&stop);
    checkCudaCall(cudaMemcpy(deviceValues, values, n*sizeof(float), cudaMemcpyHostToDevice));
    
    while(n != 1){
        n = n/2;

        // copy the original vectors to the GPU

        // execute kernel
        cudaEventRecord(start, 0);
        cudaMax<<<(int) ceilf((float) n/threadBlockSize), threadBlockSize>>>(size, gap, deviceValues);
        cudaEventRecord(stop, 0);
        // check whether the kernel invocation was successful
        checkCudaCall(cudaGetLastError());

        gap = gap * 2; 
    }	
    // copy result back
    checkCudaCall(cudaMemcpy(values, deviceValues, n * sizeof(float), cudaMemcpyDeviceToHost));
    max = values[0];
    cout << "max cuda value: " <<  max << endl;

    checkCudaCall(cudaFree(deviceValues));

    // print the time the kernel invocation took, without the copies!
    float elapsedTime;
    cudaEventElapsedTime(&elapsedTime, start, stop);
    
    cout << "kernel invocation took " << elapsedTime << " milliseconds" << endl;

}

int main(int argc, char* argv[]) {
    float* values;
    timer vectorAddTimer("vector add timer");
    int n = 300;

    values = new float[n];
    memset(values, 0, n * sizeof(float));
    fillRandom(values, n);
    cout << "max serial: " << max_value(n, values) << endl;

    vectorAddTimer.start();
    calculateMax(n, values);
    vectorAddTimer.stop();
	
    cout << "max cuda: " << values[0] << endl;
    for(int i = 0; i < n; i++){
        //cout << values[i] << " " << endl;
    }
    cout << vectorAddTimer;
    cout << "results OK!" << endl;
            
    delete[] values;
    
    return 0;
}
