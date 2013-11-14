#include <stdio.h>
#include <stdlib.h>

double sum(double* vec, int len){
    int i; 
    double accu = 0;
    #pragma omp parallel for reduction(+:accu) num_threads(8) firstprivate(vec)
    for(i=0; i<len; i++){
        accu = accu + vec[i];
    }
    return accu;
}

double reduce(double(fun)(double, double), double* vec, int len){
    int i;
    double accu = 0;
    #pragma omp parallel for reduction(+:accu) num_threads(8) firstprivate(vec, fun)
    for(i=0; i<len; i++){
        accu = fun(accu , vec[i]);
    }
    return accu;
}

double reduce_function(double total, double reduction){
    return total - reduction;
}

int main(int argc, char *argv[])
{
    double *vec;
    vec = malloc(64 * sizeof(double));
    for(int i=0; i<64; i++){
        vec[i] = i;
    }
    printf("%g\n", sum(vec, 64));

    return 0;
}