#include <stdio.h>
#include <stdlib.h>
#include <time.h>


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
    double *vec, totaltime;
    int derp;
    vec = malloc(64 * sizeof(double));
    for(int i=0; i<64; i++){
        vec[i] = i;
    }
    time_t start, end;
    time(&start);
    derp = sum(vec, 64);
    end = time(&end);
    totaltime = difftime(end,start);
    printf("time: %g\n", totaltime);

    return 0;
}
