#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <limits.h>
#include <stdbool.h>

int main(int argc, char *argv[])
{
    long prime_size = 16;
    long number_of_primes = 1;
    long number = 3;
    long i;
    bool no_prime;

    long* primes = malloc(12 * sizeof(long));
    primes[0] = 2;
    printf("%ld\n", 2);
    do{
        for(i = 0; i < number_of_primes; i++){
            if(number % primes[i] == 0){
                break;
                no_prime = true;
            }
        }
        if(no_prime == false){
            printf("%ld,", number);
            primes[number_of_primes] = number;
            number_of_primes ++;
            if(number_of_primes == prime_size){
                primes = realloc(primes, 2*prime_size*sizeof(long));
                prime_size = prime_size * 2;
            }
        }
        no_prime = false;
        number++;
    }while(true);
}
