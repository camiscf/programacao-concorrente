#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void gerarArquivoBinario(const char *nomeArquivo, int N) {
    FILE *arquivo = fopen(nomeArquivo, "wb");
    if (!arquivo) {
        perror("Erro ao abrir arquivo");
        exit(EXIT_FAILURE);
    }

    srand(time(NULL));
    for (int i = 0; i < N; i++) {
        int valor = rand() % 1000000 + 1;  // Gera números entre 1 e 1000000
        fwrite(&valor, sizeof(int), 1, arquivo);
    }

    fclose(arquivo);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <nome_do_arquivo> <N>\n", argv[0]);
        return EXIT_FAILURE;
    }

    const char *nomeArquivo = argv[1];
    int N = atoi(argv[2]);

    if (N <= 0) {
        fprintf(stderr, "N deve ser um número positivo.\n");
        return EXIT_FAILURE;
    }

    gerarArquivoBinario(nomeArquivo, N);
    return 0;
}
