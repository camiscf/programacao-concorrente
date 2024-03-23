/* Disciplina: Programacao Concorrente */
/* Prof.: Silvana Rossetto */
/* Laboratório: 1 */
/* Codigo: "Hello World" usando threads em C */

#include <stdio.h>
#include <stdlib.h> 
#include <pthread.h>

//--funcao executada pelas threads
void *PrintHello (void *arg) {

  printf("Hello World\n");

  pthread_exit(NULL);
}

//--funcao principal do programa
int main(int argc, char* argv[]) {
  int nthreads; //qtde de threads que serao criadas (recebida na linha de comando)
   
  //verifica se o argumento 'qtde de threads' foi passado e armazena seu valor
  if(argc<2) { 
      printf("--ERRO: informe a qtde de threads <%s> <nthreads>\n", argv[0]); exit(-1);
  }
  nthreads = atoi(argv[1]);

  pthread_t tid_sistema[nthreads]; //identificadores das threads no sistema
  
  //cria as threads e atribui a tarefa de cada thread
  for(int i=0; i<nthreads; i++) {
    printf("--Cria a thread %d\n", i);
    if (pthread_create(&tid_sistema[i], NULL, PrintHello, NULL)) {
      printf("--ERRO: pthread_create()\n"); exit(-2);
    }
  }

  printf("--Thread principal terminou\n");

  pthread_exit(NULL);
}
