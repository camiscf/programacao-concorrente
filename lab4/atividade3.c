#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include "timer.h"


typedef struct {
    long long start;
    long long end;
    int *count;
} ThreadArgs;

int ehPrimo(long long int n) {
    int i;

    if (n <= 1) return 0;
    if (n == 2) return 1;
    if (n % 2 == 0) return 0;
    for (i = 3; i < sqrt(n) + 1; i += 2) {
        if (n % i == 0) return 0;
    }
    return 1;
}

void *verificaPrimos(void *args) {
    ThreadArgs *threadArgs = (ThreadArgs *) args;
    long long start = threadArgs->start;
    long long end = threadArgs->end;
    int *count = threadArgs->count;

    for (long long i = start; i <= end; i++) {
        if (ehPrimo(i)) {
            (*count)++;
        }
    }


    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Parametros: %s <N> <threads>\n", argv[0]);
        exit(1); // Exit with an error status
    }

    long long N = atoll(argv[1]);
    int numThreads = atoi(argv[2]);

    pthread_t *threads = malloc(numThreads * sizeof(pthread_t));
    ThreadArgs *threadArgs = malloc(numThreads * sizeof(ThreadArgs));

    int totalCount = 0;
    long long blockSize = (N + numThreads - 1) / numThreads;

    double inicio, fim, delta;
    GET_TIME(inicio);

    for (int i = 0; i < numThreads; i++) {
        threadArgs[i].start = i * blockSize + 1;
        threadArgs[i].end = (i + 1) * blockSize;

        if (threadArgs[i].end > N) {
            threadArgs[i].end = N;
        }
        threadArgs[i].count = &totalCount;
        pthread_create(&threads[i], NULL, verificaPrimos, (void *) &threadArgs[i]);
    }

    for (int i = 0; i < numThreads; i++) {
        pthread_join(threads[i], NULL);
    }

    GET_TIME(fim);
    delta = fim - inicio;
    printf("Tempo verificando primo:%lf\n", delta);
    printf("total de primos %d\n", totalCount);
    return 0; // Return success status
}
