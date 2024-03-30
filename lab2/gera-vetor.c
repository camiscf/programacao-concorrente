#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MAX_ELEMENTOS 1000 // máximo de elementos que você deseja no vetor
#define MIN_VALOR 1        // valor mínimo do elemento
#define MAX_VALOR 100000000      // valor máximo do elemento

int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Use: %s <quantidade_elementos>\n", argv[0]);
        return 1;
    }

    int quantidade_elementos = atoi(argv[1]);

    // Verificando se a quantidade de elementos está dentro dos limites
    if (quantidade_elementos <= 0) {
        printf("Quantidade de elementos deve ser maior que 0 \n");
        return 1;
    }

    // Gerando um vetor de entrada com valores aleatórios
    srand(time(NULL));
    printf("%d\n", quantidade_elementos);
    for (int i = 0; i < quantidade_elementos; i++) {
        printf("%.2f ", (float)(rand() % (MAX_VALOR - MIN_VALOR + 1) + MIN_VALOR));
    }
    printf("\n");

    return 0;
}
