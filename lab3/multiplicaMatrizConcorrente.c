#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "timer.h" // Biblioteca externa para medição de tempo

#define MAX_THREADS 16

// Estrutura para armazenar argumentos das threads
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

// Função executada por cada thread para calcular parte do resultado
void *multiplyPart(void *arg) {
    ThreadArgs *args = (ThreadArgs *)arg;
    float soma;
    for (int i = args->startRow; i < args->endRow; i++) {
        for (int j = 0; j < args->colsB; j++) {
            soma = 0;
            for (int k = 0; k < args->colsA; k++) {
                soma += args->matA[i][k] * args->matB[k][j];
            }
            args->result[i][j] = soma;
        }
    }
    pthread_exit(NULL);
}

// Função para multiplicar matrizes de forma concorrente
void multiplyMatricesConcurrent(float **matA, float **matB, float ***result, int rowsA, int colsA, int colsB, int numThreads) {
    pthread_t threads[numThreads]; // Array de identificadores de threads
    ThreadArgs threadArgs[numThreads]; // Array de argumentos para cada thread
    int chunkSize = (rowsA + numThreads - 1) / numThreads; // Tamanho do bloco de linhas a ser processado por cada thread
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

        pthread_create(&threads[t], NULL, multiplyPart, (void *)&threadArgs[t]); // Criação da thread
    }
    for (int t = 0; t < numThreads; t++) {
        pthread_join(threads[t], NULL); // Espera todas as threads terminarem
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
    double inicio, fim, delta;
    int num_threads;

    GET_TIME(inicio); // Macro para obter tempo inicial

    // Verifica se o número de argumentos é correto
    if (argc != 5) {
        printf("Uso: %s <arquivo_matriz1> <arquivo_matriz2> <arquivo_saida> <num_threads>\n", argv[0]);
        return 1;
    }

    // Abrindo arquivos de entrada e saída
    FILE *fileA = fopen(argv[1], "rb");
    FILE *fileB = fopen(argv[2], "rb");
    FILE *fileOutput = fopen(argv[3], "wb");

    num_threads = atoi(argv[4]); // Converte o número de threads de string para inteiro

    if (fileA == NULL || fileB == NULL || fileOutput == NULL) {
        perror("Erro ao abrir arquivo"); // Exibe mensagem de erro do sistema
        return 1;
    }

    // Lendo dimensões das matrizes
    int rowsA, colsA, rowsB, colsB;
    fread(&rowsA, sizeof(int), 1, fileA);
    fread(&colsA, sizeof(int), 1, fileA);
    fread(&rowsB, sizeof(int), 1, fileB);
    fread(&colsB, sizeof(int), 1, fileB);

    // Verifica se as dimensões são compatíveis para a multiplicação de matrizes
    if (colsA != rowsB) {
        printf("Erro: O número de colunas da matriz A deve ser igual ao número de linhas da matriz B.\n");
        return 1;
    }

    GET_TIME(fim);
    delta = fim - inicio;
    printf("Tempo inicializacao:%lf\n", delta);

    GET_TIME(inicio);    

    // Alocando memória para as matrizes
    float **matA = (float **)malloc(rowsA * sizeof(float *));
    float **matB = (float **)malloc(rowsB * sizeof(float *));
    float **result = (float **)malloc(rowsA * sizeof(float *));

    if (matA == NULL || matB == NULL || result == NULL) {
        printf("Erro ao alocar memória para as matrizes\n");
        return 1;
    }
    
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
    
    GET_TIME(fim);  
    delta = fim - inicio;
    printf("Tempo multiplicacao: %lf\n", delta); 
    
    // Escrevendo matriz resultante no arquivo de saída
    fwrite(&rowsA, sizeof(int), 1, fileOutput);
    fwrite(&colsB, sizeof(int), 1, fileOutput);
    for (int i = 0; i < rowsA; i++) {
        fwrite(result[i], sizeof(float), colsB, fileOutput);
    }

    GET_TIME(inicio);
    // Liberando memória e fechando arquivos
    freeMatrices(matA, rowsA);
    freeMatrices(matB, rowsB);
    freeMatrices(result, rowsA);
    fclose(fileA);
    fclose(fileB);
    fclose(fileOutput);

    printf("Multiplicação de matrizes concluída com sucesso!\n");
    
    GET_TIME(fim);
    delta = fim - inicio;
    printf("Tempo finalizacao:%lf\n", delta);

    return 0;
}
