// CAMILA CALEONES DE FIGUEIREDO - PROGRAMAÇÃO CONCORRENTE - 2024.1 - UFRJ

// Escreva um programa de M threads para elevar ao quadrado cada elemento de um vetor de N elementos do tipo inteiro

/* 
ROTEIRO
1. Cada thread vai executar N/M elementos
    1.1 os argumentos passados para cada thread: o vetor, qntd de elementos e elementos
2. Implementar funções para inicializar e carregar o vetor e outra se o resultado final está correto
3. Na main:
    - Inicializar o vetor
    - criar as threads
    - acabar as threas
    - verificar se os valores estão corretos
*/

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>

int *vetor;
int N, M;

void inicializarVetor() {
    for (int i = 0; i < N; i++) {
        vetor[i] = rand() % 10; 
        printf("valor na posicao %d do vetor: %d\n\n", i, vetor[i]);
    }
}

void verificarResultado() {
    for (int i = 0; i < N; i++) {
        printf("%d ", vetor[i]);
    }
    printf("\n");
}

void *elevarAoQuadrado(void *arg) {

    /* Foi escolhido dividir o vetor 
    para cada thread pegar um pedaço igual do vetor, 
    ou seja, cada thread ganha um trabalho igual*/

    int thread_id = *((int *)arg);
    int inicio = thread_id * (N / M); 
    int fim = (thread_id + 1) * (N / M);

    for (int i = inicio; i < fim; i++) {
        vetor[i] = vetor[i] * vetor[i];
    }

    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        printf("Passar os argumentos: %s <N> elementos do vetor e <M> threads \n", argv[0]);
        return 1;
    }

    N = atoi(argv[1]); // Número de elementos do vetor
    M = atoi(argv[2]); // Número de threads

    vetor = (int *)malloc(N * sizeof(int));

    pthread_t threads[M];
    int thread_args[M];

    // Inicializa o vetor com valores aleatórios
    inicializarVetor();

    // Cria as threads
    for (int i = 0; i < M; i++) {
        thread_args[i] = i;
        printf("-- criando a thread %d --\n", i);
        pthread_create(&threads[i], NULL, elevarAoQuadrado, (void *)&thread_args[i]);
    }

    // Aguarda o término das threads para terminar o programa
    for (int i = 0; i < M; i++) {
        pthread_join(threads[i], NULL);
    }

    // Verifica se o vetor foi elevado ao quadrado
    verificarResultado();

    free(vetor);

    return 0;
}