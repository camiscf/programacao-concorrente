#include <stdio.h>
#include <stdlib.h>

// Função para multiplicar matrizes
void multiplyMatrices(float **matA, float **matB, float ***result, int rowsA, int colsA, int colsB) {
    float soma;
    for (int i = 0; i < rowsA; i++) {
        for (int j = 0; j < colsB; j++) {
            soma = 0;
            for (int k = 0; k < colsA; k++) {
                soma = soma + matA[i][k] * matB[k][j];
                // printf("soma: %f\n", soma);
            }
            (*result)[i][j] = soma;
        }
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
    if (argc != 4) {
        printf("Uso: %s <arquivo_matriz1> <arquivo_matriz2> <arquivo_saida>\n", argv[0]);
        return 1;
    }

    // Abrindo arquivos de entrada e saída
    FILE *fileA = fopen(argv[1], "rb");
    FILE *fileB = fopen(argv[2], "rb");
    FILE *fileOutput = fopen(argv[3], "wb");

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

    // Função para imprimir matriz (apenas para depuração)
    void printMatrix(float **matrix, int rows, int cols) {
        for (int i = 0; i < rows; i++) {
            for (int j = 0; j < cols; j++) {
                printf("%.6f\t", matrix[i][j]);
            }
            printf("\n");
        }
    }

    printf("Matriz A:\n");
    printMatrix(matA, rowsA, colsA);

    printf("Matriz B:\n");
    printMatrix(matB, rowsB, colsB);

    // Multiplicando matrizes
    multiplyMatrices(matA, matB, &result, rowsA, colsA, colsB);

    printf("Matriz C:\n");
    printMatrix(result, rowsA, colsB);


    fileOutput = fopen(argv[3], "wb");
    if(!fileOutput) {
        fprintf(stderr, "Erro de abertura do arquivo\n");
        return 3;
    }

    fwrite(&rowsA, sizeof(int), 1, fileOutput);
    fwrite(&colsB, sizeof(int), 1, fileOutput);
    for (int i = 0; i < rowsA; i++) {
        fwrite(result[i], sizeof(float), colsB, fileOutput);
    }
    fclose(fileOutput);

    // Liberando memória e fechando arquivos
    freeMatrices(matA, rowsA);
    freeMatrices(matB, rowsB);
    freeMatrices(result, rowsA);
    fclose(fileA);
    fclose(fileB);

    printf("Multiplicação de matrizes concluída com sucesso!\n");

    return 0;
}
