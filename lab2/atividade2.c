/*
CAMILA CALEONES DE FIGUEIREDO
*/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define TESTE

float *vetor;

typedef struct {
    int id;
    int tamanho_bloco;
} tArgs;

typedef struct
{
    float valor;
}soma;


void *soma_vetor(void *arg) {
    tArgs *args = (tArgs *)arg;

    int inicio = args->id * args->tamanho_bloco;
    int fim = inicio + args->tamanho_bloco;

    soma *soma_local;
    soma_local = (soma *) malloc(sizeof(soma));
    if (soma_local == NULL) {
        printf("--ERRO: malloc()\n"); 
        pthread_exit(NULL);
    }

    soma_local->valor = 0;

    for(int i = inicio; i < fim; i++){
        soma_local->valor += vetor[i];
    }

    pthread_exit((void *) soma_local);

}

int main(int argc, char *argv[]) {
    float soma_geral = 0;
    soma *soma_local;
    long int quantidade_threads;
    long int tamanho_bloco;
    long int dim; 


    if (argc != 2) {
        printf("Uso: %s <quantidade_threads>\n", argv[0]);
        return 0;
    }

    quantidade_threads = atoi(argv[1]);
    
    pthread_t *threads;
    threads = (pthread_t *) malloc(sizeof(pthread_t) * quantidade_threads);
    if (threads == NULL) {
        printf("--ERRO: malloc()\n"); 
        return 3;
    }

    scanf("%ld", &dim);
    printf("Dimensão do vetor: %ld\n", dim);

    //aloca o vetor de entrada
    vetor = (float*) malloc(sizeof(float)*dim);
    if(vetor == NULL) {
        fprintf(stderr, "ERRO--malloc\n");
        return 2;
    }

    //preenche o vetor de entrada
    for(long int i=0; i<dim; i++)
    scanf("%f", &vetor[i]);

    for (int i = 0; i< quantidade_threads; i++){

        tArgs *args = (tArgs*) malloc(sizeof(tArgs));
        
        if (args == NULL) {
            printf("--ERRO: malloc()\n"); 
            pthread_exit(NULL);
        }
        args->id = i; 

        printf("------ thread %d ------\n ",i);

        tamanho_bloco = dim/quantidade_threads;
        args->tamanho_bloco = tamanho_bloco;

        if(pthread_create(threads + i, NULL, soma_vetor, (void*) args)){
          fprintf(stderr, "ERRO--pthread_create\n");
          return 5;
       }
    }

    for(int i = 0; i < quantidade_threads; i++){
        pthread_join(threads[i], (void **) &soma_local);
        soma_geral += soma_local->valor;
    }

    if(tamanho_bloco%quantidade_threads){
        for (long int i = (tamanho_bloco%quantidade_threads)*quantidade_threads; i < tamanho_bloco; i++){
            soma_geral += soma_local->valor;
        }
    }


    printf("SOMA FINAL: %f\n", soma_geral);
    free(vetor);
    free(threads);

 #ifdef TESTE
        float valor_final = 0;
        scanf("%f", &valor_final);
        if(abs(soma_geral - valor_final) < 0.001){
            printf("O resultado está correto!.\n");
        }        
        else{
            printf("O resultado está errado.\n");
        }
#endif

    return 0;

     
}
