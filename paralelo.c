#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <assert.h>
#include <omp.h>
#include <sys/time.h>


int main(int argc, char **argv) {


//declaração de variáveis
double **x, **y, **z;
int N = 300; //tamanho da matriz quadrada
int qt = 0, qtS = 0;
int numVezes = 1;

if (argc == 2) {
  N = atoi (argv[1]);
  //encontrar bugs quando a aplicação ainda está em desenvolvimento
  assert (N > 0);
}

int i,j,k,mul=5;
int col_sum = N * (N-1) / 2;

//alocando memória
x = (double **)malloc (N * sizeof(double *));
y = (double **)malloc (N * sizeof(double *));
z = (double **)malloc (N * sizeof(double *));

for (i=0; i<N; i++) {
  x[i] = (double *)malloc (N * sizeof(double));
  y[i] = (double *)malloc (N * sizeof(double));
  z[i] = (double *)malloc (N * sizeof(double));
}

printf("\n Tamanho Matriz - %d (s)\n", N);
printf(" Quantidade de vezes - %d (s)\n", numVezes);


//Retorna um valor em segundos do tempo decorrido desde algum ponto. 
double inicioS = omp_get_wtime(); 


printf ("\n ..................... INICIO SEQUENCIAL ..................... \n");
//quantidade de vezes que ocorrerá a multiplicação sequencialmente
for (qtS=0; qtS<numVezes;qtS++) { 

//código sequencial - Inicio

for (i=0; i<N; i++)
  for (j=0; j<N; j++) {
    x[i][j] = i*mul;
    y[i][j] = i;
    z[i][j] = 0; 
}

for (i=0; i<N; i++)
  for (j=0; j<N; j++)
     for (k=0; k<N; k++)
	z[i][j] += x[i][k] * y[k][j];
}
//código sequencial -Fim

//Retorna um valor em segundos do tempo decorrido desde algum ponto. 
double fimS = omp_get_wtime();

printf(" Tempo total -> %.20f (s)\n", fimS - inicioS); 



//Retorna que um valor em segundos do tempo decorrido desde algum ponto. 
double inicio = omp_get_wtime(); 

//código paralelo - inicio

printf ("\n..................... INICIO PARALELO ..................... \n\n");
//quantidade de vezes que ocorrerá a multiplicação paralelamente
for (qt=0; qt<numVezes;qt++) { 

/* Região paralelismo para multiplicar matriz 
shared: Especifica que uma ou mais variáveis devem ser compartilhadas entre todos os threads
private: cada segmento deve ter sua própria instância de uma variável */    
#pragma omp parallel shared (x,y,z) private (i,j,k) num_threads(4)
{

//int itr = omp_get_thread_num();
//printf("Hello from thread %d\n", itr);

//printf ("INICIO DA INICIALIZAÇÃO DAS MATRIZES.\n");

//SCHEDULE permite uma variedade de opções por especificar quais iterações dos laços são executadas por quais threads
//DYNAMIC divide o espaço de iteração em pedaços de tamanho chunksize , e os atribui para as threads com uma política first-come-first-served
#pragma omp for schedule (dynamic) 
for (i=0; i<N; i++)
  for (j=0; j<N; j++) {
    x[i][j] = i*mul;
    y[i][j] = i;
    z[i][j] = 0; 
  }
//printf ("FIM DA INICIALIZAÇÃO DAS MATRIZES.\n\n");

/*
for (i=0; i<N; i++){
  for (j=0; j<N; j++) {
    printf ("%p", &x[ i ][ j ]);
    printf (" ");
  }  
printf ("\n");
}*/



//printf ("INICIO DA MULTIPLICAÇÃO DA MATRIZ x PELA MATRIZ y.\n");

#pragma omp for schedule (dynamic)
for (i=0; i<N; i++)
  for (j=0; j<N; j++)
     for (k=0; k<N; k++)
	z[i][j] += x[i][k] * y[k][j];

//printf ("FIM DA MULTIPLICAÇÃO DA MATRIZ x PELA MATRIZ y.\n\n");

#pragma omp for schedule (dynamic)
for (i=0; i<N; i++)
  for (j=0; j<N; j++)
    assert ( z[i][j] == i*mul * col_sum);  


} 
/* Fim da Região paralelismo para multiplicar matriz */    
//código paralelo - fim

} // for - quantidade de vezes que ocorrerá a multiplicação

//Retorna que um valor em segundos do tempo decorrido desde algum ponto. 
double fim = omp_get_wtime();

printf(" Tempo total -> %.20f (s)\n", fim - inicio); 

//printf("%d",qt);
//printf(" Prl: %.16g (s)\n", fim - inicio);


//printf ("DESALOCANDO VARIÁVEIS DA MEMÓRIA.\n\n");
free(x);
free(y);
free(z);

} //main
