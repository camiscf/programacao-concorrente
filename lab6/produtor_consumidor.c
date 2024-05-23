#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <semaphore.h>

#define TRUE 1
#define FALSE 0

// Buffer compartilhado entre a thread produtora e as threads consumidoras
int *buffer;
int in = 0, out = 0; // Índices para inserção e remoção no buffer
int buffer_size; // Tamanho do buffer
int producao_terminada = 0; // Flag para indicar que a produção terminou

sem_t empty_slots; // Semáforo para contar espaços vazios no buffer
sem_t full_slots;  // Semáforo para contar espaços ocupados no buffer
sem_t mutex;       // Semáforo para exclusão mútua

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
        sem_wait(&empty_slots); // Esperar por um espaço vazio no buffer
        sem_wait(&mutex);       // Entrar na seção crítica

        buffer[in] = numero;
        in = (in + 1) % buffer_size;

        sem_post(&mutex);       // Sair da seção crítica
        sem_post(&full_slots);  // Incrementar o número de espaços ocupados
    }

    fclose(arquivo);

    // Sinalizar que a produção acabou
    sem_wait(&mutex);
    producao_terminada = 1;
    sem_post(&mutex);
    sem_post(&full_slots); // Desbloquear qualquer consumidor potencialmente bloqueado

    pthread_exit(0);
}

// Função das threads consumidoras
void *consumidora(void *param) {
    ThreadData *data = (ThreadData *) param; // Dados da thread
    data->primosEncontrados = 0;

    while (TRUE) {
        sem_wait(&full_slots); // Esperar por um item no buffer
        sem_wait(&mutex);      // Entrar na seção crítica

        if (producao_terminada && buffer[in] == buffer[out]) {
            sem_post(&mutex);
            sem_post(&full_slots); // Desbloquear outras threads potencialmente bloqueadas
            break;
        }

        int numero = buffer[out];
        out = (out + 1) % buffer_size;

        sem_post(&mutex);      // Sair da seção crítica
        sem_post(&empty_slots); // Incrementar o número de espaços vazios

        // Verificar se o número é primo
        if (ehPrimo(numero)) {
            data->primosEncontrados++;
        }
    }

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

    // Inicializar os semáforos
    sem_init(&empty_slots, 0, buffer_size);
    sem_init(&full_slots, 0, 0);
    sem_init(&mutex, 0, 1);

    pthread_t prodThread;
    pthread_t consThreads[numThreads];
    ThreadData threadData[numThreads];

    // Criar a thread produtora
    pthread_create(&prodThread, NULL, produtora, (void *) nomeArquivo);

    // Criar as threads consumidoras
    for (int i = 0; i < numThreads; i++) {
        threadData[i].id = i;
        pthread_create(&consThreads[i], NULL, consumidora, (void *) &threadData[i]);
    }

    // Esperar pela thread produtora
    pthread_join(prodThread, NULL);

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

    // Destruir os semáforos
    sem_destroy(&empty_slots);
    sem_destroy(&full_slots);
    sem_destroy(&mutex);

    return 0;
}
