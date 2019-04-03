#include <pthread.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>

const size_t numprocs = 4;
const int vec_size = 100;
double resultados[4];

int* vector;

void* fillVector(void * id){
	long ID=(long)id;
	pid_t tid;
	tid = syscall(SYS_gettid);
	
	// int myPart = 
	unsigned int i;
	printf("Thread: %ld(%d) - Process ID: %5d Pai:%5d\n",ID,tid,getpid(),getppid());
	// printf("Thread: %ld\n",ID);
	for(i = ID; i < vec_size; i+=numprocs){
		vector[i] = rand();
	}
	pthread_exit(NULL);
}

int main(){	
	pid_t tid;
	tid = syscall(SYS_gettid);
	printf("Thread: (%d) - Process ID: %5d Pai:%5d\n",tid,getpid(),getppid());
	srand(time(NULL)); // Initialization, should only be called once.
	vector = (int *) malloc(vec_size * sizeof(int));

	//Idenficador de cada thread
	pthread_t handles[numprocs]; 

	/*Carrega os atributos padrões para criação das threads. Dentre os atributos, estão: prioridade no escalonamento e tamanho da pilha.*/
	pthread_attr_t attr; 
	pthread_attr_init(&attr);

	//Cria as threads usando os atributos carregados.
	long i;
	for (i = 0; i < numprocs; i++){
		pthread_create(&handles[i], &attr, fillVector, (void*)i);
	}
	
	// Espera todas as threads terminarem.
	for (i = 0; i != numprocs; ++i)
		pthread_join(handles[i], NULL); //NULL -> parâmetro de retorno

	long i;
	for (i = 0; i < numprocs; i++){
		pthread_create(&handles[i], &attr, fillVector, (void*)i);
	}
	
	// Espera todas as threads terminarem.
	for (i = 0; i != numprocs; ++i)
		pthread_join(handles[i], NULL); //NULL -> parâmetro de retorno
	
	/* Soma o resultado de cada thread. Observe que cada thread escreve em uma posição do vetor de resultados (o que evita inconsistência).*/
	double pi=0.0;
	for (i = 0; i != numprocs; ++i)		
		pi=pi+resultados[i]; 

	for (i = 0; i < vec_size; ++i)		
		printf("%d, ", vector[i]); 
	
	puts("");
	return 0;
}
