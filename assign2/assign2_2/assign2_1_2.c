#include <stdio.h>
#include <stdlib.h>
#include <time.h>

double reduce_function(double total, double reduction);
double sum(double* vec, int len);
double reduce(double(fun)(double, double), double* vec, int len);

double sum(double* vec, int len){
    int i; 
    double accu = 0;
    #pragma omp parallel for reduction(+:accu) num_threads(4) firstprivate(vec)
    for(i=0; i<len; i++){
        accu = accu + vec[i];
    }
    return accu;
}

double reduce(double(fun)(double, double), double* vec, int len){
    int i;
    double accu = 0;
    #pragma omp parallel for reduction(+:accu) num_threads(1) firstprivate(vec, fun)
    for(i=0; i<len; i++){
        accu = fun(accu , vec[i]);
    }
    return accu;
}

double reduce_function(double total, double reduction){
    return total - reduction;
}

struct timespec diff(struct timespec start, struct timespec end)
{
    struct timespec temp;
    if ((end.tv_nsec-start.tv_nsec)<0) {
        temp.tv_sec = end.tv_sec-start.tv_sec-1;
        temp.tv_nsec = 1000000000+end.tv_nsec-start.tv_nsec;
    } else {
        temp.tv_sec = end.tv_sec-start.tv_sec;
        temp.tv_nsec = end.tv_nsec-start.tv_nsec;
    }
    return temp;
}

int main(int argc, char *argv[])
{
    double *vec, totaltime;
    struct timespec time1, time2;
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time1);
    int vecsize;
    for(vecsize = 1000; vecsize <= 10000000; vecsize = vecsize * 10){
        vec = malloc(vecsize * sizeof(double));
        for(int i=0; i<vecsize; i++){
            vec[i] = i;
        }
        sum(vec, vecsize);
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &time2);
        printf("%g\n", difftime(time2.tv_sec, time1.tv_sec) +
            (time2.tv_nsec - time1.tv_nsec) / 1000000000.);
    }

    return 0;
}
