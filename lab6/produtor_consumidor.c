#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>

#define TRUE 1
#define FALSE 0

// Buffer compartilhado entre a thread produtora e as threads consumidoras
int *buffer;
int in = 0, out = 0; // Índices para inserção e remoção no buffer
int count = 0; // Contador de itens no buffer
int buffer_size; // Tamanho do buffer
int producao_terminada = 0; // Flag para indicar que a produção terminou

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // Mutex para sincronização
pthread_cond_t cond_producer = PTHREAD_COND_INITIALIZER; // Condição para a thread produtora
pthread_cond_t cond_consumer = PTHREAD_COND_INITIALIZER; // Condição para as threads consumidoras

// Estrutura para armazenar dados de cada thread consumidora
typedef struct {
    int id;
    int primosEncontrados;
} ThreadData;

// Função para verificar se um número é primo
int ehPrimo(long long int n) {
    if (n <= 1) return FALSE;
    if (n == 2) return TRUE;
    if (n % 2 == 0) return FALSE;
    for (int i = 3; i <= sqrt(n); i += 2) {
        if (n % i == 0) return FALSE;
    }
    return TRUE;
}

// Função da thread produtora
void *produtora(void *param) {
    const char *nomeArquivo = (const char *) param; // Nome do arquivo de entrada
    FILE *arquivo = fopen(nomeArquivo, "rb");
    if (!arquivo) {
        perror("Erro ao abrir arquivo");
        exit(EXIT_FAILURE);
    }

    int numero;
    // Ler números do arquivo e colocá-los no buffer
    while (fread(&numero, sizeof(int), 1, arquivo) == 1) {
        pthread_mutex_lock(&mutex);

        // Esperar se o buffer estiver cheio
        while (count == buffer_size) {
            pthread_cond_wait(&cond_producer, &mutex);
        }

        buffer[in] = numero;
        in = (in + 1) % buffer_size;
        count++;

        // Sinalizar uma thread consumidora que um novo item está disponível
        pthread_cond_signal(&cond_consumer);
        pthread_mutex_unlock(&mutex);
    }

    fclose(arquivo);

    // Sinalizar que a produção acabou
    pthread_mutex_lock(&mutex);
    producao_terminada = 1;
    pthread_cond_broadcast(&cond_consumer); // Notificar todas as threads consumidoras
    pthread_mutex_unlock(&mutex);

    pthread_exit(0);
}

// Função das threads consumidoras
void *consumidora(void *param) {
    ThreadData *data = (ThreadData *) param; // Dados da thread
    data->primosEncontrados = 0;

    while (TRUE) {
        pthread_mutex_lock(&mutex);

        // Esperar se o buffer estiver vazio e a produção não tiver terminado
        while (count == 0 && !producao_terminada) {
            pthread_cond_wait(&cond_consumer, &mutex);
        }

        // Sair do loop se a produção terminou e o buffer está vazio
        if (count == 0 && producao_terminada) {
            pthread_mutex_unlock(&mutex);
            break;
        }

        int numero = buffer[out];
        out = (out + 1) % buffer_size;
        count--;

        // Sinalizar a thread produtora que há espaço disponível no buffer
        pthread_cond_signal(&cond_producer);
        pthread_mutex_unlock(&mutex);

        // Verificar se o número é primo
        if (ehPrimo(numero)) {
            data->primosEncontrados++;
        }
    }

    // printf("Thread consumidora %d finalizada. Primos encontrados: %d\n", data->id, data->primosEncontrados);
    pthread_exit(0);
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Uso: %s <numero_de_threads> <tamanho_do_buffer> <arquivo_de_entrada>\n", argv[0]);
        return EXIT_FAILURE;
    }


    int numThreads = atoi(argv[1]); // Número de threads consumidoras
    buffer_size = atoi(argv[2]); // Tamanho do buffer
    const char *nomeArquivo = argv[3]; // Nome do arquivo de entrada

    // Alocar memória para o buffer
    buffer = (int *) malloc(buffer_size * sizeof(int));
    if (buffer == NULL) {
        perror("Erro ao alocar memória para o buffer");
        return EXIT_FAILURE;
    }

    // printf("Buffer alocado com sucesso. Iniciando threads...\n");

    pthread_t prodThread;
    pthread_t consThreads[numThreads];
    ThreadData threadData[numThreads];

    // Criar a thread produtora
    pthread_create(&prodThread, NULL, produtora, (void *) nomeArquivo);
    // printf("Thread produtora criada.\n");

    // Criar as threads consumidoras
    for (int i = 0; i < numThreads; i++) {
        threadData[i].id = i;
        pthread_create(&consThreads[i], NULL, consumidora, (void *) &threadData[i]);
        // printf("Thread consumidora %d criada.\n", i);
    }

    // Esperar pela thread produtora
    pthread_join(prodThread, NULL);
    // printf("Thread produtora finalizada.\n");

    int totalPrimos = 0;
    int vencedora = 0;
    int maxPrimos = 0;

    // Esperar pelas threads consumidoras e calcular o total de primos encontrados
    for (int i = 0; i < numThreads; i++) {
        pthread_join(consThreads[i], NULL);
        totalPrimos += threadData[i].primosEncontrados;
        if (threadData[i].primosEncontrados > maxPrimos) {
            maxPrimos = threadData[i].primosEncontrados;
            vencedora = i;
        }
    }

    printf("Total de primos encontrados: %d\n", totalPrimos);
    printf("Thread consumidora vencedora: %d\n", vencedora);

    // Liberar a memória do buffer
    free(buffer);

    return 0;
}
