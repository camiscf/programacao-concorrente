#include <stdio.h>

long int multiplica_matriz_sequencial(float a[linhas][elementos], float b[elementos][colunas], float c[linhas][colunas]){
    for (int i = 0; i < linhas; i++){
        for(int j = 0; j < colunas; j++){
            for(int k = 0; k < elementos; k++){
                c[i][j] += a[i][k]*b[k][j];
            }
        }
    }
}

int main() {
    float *matriz;
    int linhas

}