#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define MAX_THREADS 16

typedef struct {
    float **matA;
    float **matB;
    float **result;
    int rowsA;
    int colsA;
    int colsB;
    int startRow;
    int endRow;
} ThreadArgs;

// Função para multiplicar uma parte das matrizes
void *multiplyPart(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;
    float soma;
    for (int i = args->startRow; i < args->endRow; i++) {
        for (int j = 0; j < args->colsB; j++) {
            soma = 0;
            for (int k = 0; k < args->colsA; k++) {
                soma += args->matA[i][k] * args->matB[k][j];
                printf("celula[%d][%d] = %.6f\n", i,j,soma);
            }
            args->result[i][j] = soma;
        }
    }
    pthread_exit(NULL);
}

// Função para multiplicar matrizes de forma concorrente
void multiplyMatricesConcurrent(float **matA, float **matB, float ***result, int rowsA, int colsA, int colsB, int numThreads) {
    pthread_t threads[numThreads];
    ThreadArgs threadArgs[numThreads];
    int chunkSize = (rowsA + numThreads - 1) / numThreads;
    for (int t = 0; t < numThreads; t++) {
        threadArgs[t].matA = matA;
        threadArgs[t].matB = matB;
        threadArgs[t].result = *result;
        threadArgs[t].rowsA = rowsA;
        threadArgs[t].colsA = colsA;
        threadArgs[t].colsB = colsB;
        threadArgs[t].startRow = t * chunkSize;
        threadArgs[t].endRow = (t + 1) * chunkSize;

        if (threadArgs[t].endRow > rowsA) {
            threadArgs[t].endRow = rowsA;
        }

        printf("--- THREAD %d ---\n", t);
        pthread_create(&threads[t], NULL, multiplyPart, (void *)&threadArgs[t]);
    }
    for (int t = 0; t < numThreads; t++) {
        pthread_join(threads[t], NULL);
    }
}

// Função para liberar a memória alocada para as matrizes
void freeMatrices(float **matrix, int rows) {
    for (int i = 0; i < rows; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

int main(int argc, char *argv[]) {
    int num_threads;

    if (argc != 5) {
        printf("Uso: %s <arquivo_matriz1> <arquivo_matriz2> <arquivo_saida> <num_threads>\n", argv[0]);
        return 1;
    }

    // Abrindo arquivos de entrada e saída
    FILE *fileA = fopen(argv[1], "rb");
    FILE *fileB = fopen(argv[2], "rb");
    FILE *fileOutput = fopen(argv[3], "wb");

    num_threads = atoi(argv[4]);

    if (fileA == NULL || fileB == NULL || fileOutput == NULL) {
        perror("Erro ao abrir arquivo");
        return 1;
    }

    // Lendo dimensões das matrizes
    int rowsA, colsA, rowsB, colsB;
    fread(&rowsA, sizeof(int), 1, fileA);
    fread(&colsA, sizeof(int), 1, fileA);
    fread(&rowsB, sizeof(int), 1, fileB);
    fread(&colsB, sizeof(int), 1, fileB);

    if (colsA != rowsB) {
        printf("Erro: O número de colunas da matriz A deve ser igual ao número de linhas da matriz B.\n");
        return 1;
    }

    // Alocando memória para as matrizes
    float **matA = (float **)malloc(rowsA * sizeof(float *));
    float **matB = (float **)malloc(rowsB * sizeof(float *));
    float **result = (float **)malloc(rowsA * sizeof(float *));
    for (int i = 0; i < rowsA; i++) {
        matA[i] = (float *)malloc(colsA * sizeof(float));
        result[i] = (float *)malloc(colsB * sizeof(float));
    }
    for (int i = 0; i < rowsB; i++) {
        matB[i] = (float *)malloc(colsB * sizeof(float));
    }

    // Lendo matrizes do arquivo
    for (int i = 0; i < rowsA; i++) {
        fread(matA[i], sizeof(float), colsA, fileA);
    }
    for (int i = 0; i < rowsB; i++) {
        fread(matB[i], sizeof(float), colsB, fileB);
    }

    // Multiplicando matrizes de forma concorrente
    multiplyMatricesConcurrent(matA, matB, &result, rowsA, colsA, colsB, num_threads);

    // Escrevendo matriz resultante no arquivo de saída
    fwrite(&rowsA, sizeof(int), 1, fileOutput);
    fwrite(&colsB, sizeof(int), 1, fileOutput);
    for (int i = 0; i < rowsA; i++) {
        fwrite(result[i], sizeof(float), colsB, fileOutput);
    }

    // Liberando memória e fechando arquivos
    freeMatrices(matA, rowsA);
    freeMatrices(matB, rowsB);
    freeMatrices(result, rowsA);
    fclose(fileA);
    fclose(fileB);
    fclose(fileOutput);

    printf("Multiplicação de matrizes concluída com sucesso!\n");

    return 0;
}
