#include <stdio.h>
#include <stdlib.h>

double sum(double* vec, int len){
    int i; 
    double accu;
    #pragma omp parallel for critical(accu) num_threads(8) firstprivate(vec)
    for(i=0; i<len; i++){
        accu = accu + vec[i];
    }
    return accu;
}

double reduce(double(fun)(double, double), double* vec, int len){
    int i;
    double accu;
    #pragma omp parallel for critical(accu) num_threads(8) firstprivate(vec, fun)
    for(i=0; i<len; i++){
        accu = fun(accu , vec[i]);
    }
    return accu;
}