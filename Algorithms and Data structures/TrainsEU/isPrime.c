#include <stdlib.h>
#include <stdio.h>
#include <limits.h>
#include <stdbool.h>

bool isPrime(int n){
    if(n < 2){
        return false;
    }
    if(n == 2){
        return true;
    }
    if(n%2 == 0){
        return false;
    }
    for (int i = 3; i < n/2; i = i + 2){
        if(n%i == 0){
            return false;
        }
    }

    return true;
}

int *primes(int from, int to){
    int range = to - from;
    int *primes = (int*)malloc((range)*sizeof(int));
    int k = 0;
    for (int i = from; i <= to; i++){
        if(isPrime(i)){
            primes[k] = i;
            k++;
        }
    }

    if(k == 0){
        int *zero = {0};
        return zero;
    }

    int *smaller = (int*)malloc((k+1)*sizeof(int));
    for (int i = 0; i < k; i++){
        smaller[i] = primes[i];
    }
    smaller[k] = 0;
    return smaller;
}

int main(){
    int *a = (int*)malloc((100)*sizeof(int));
    a = primes(100, 200);
    int i = 0;
    while(a[i] != 0 && i<100){
        printf("%d, ", a[i]);
        i++;
    }
    return 0;
}
