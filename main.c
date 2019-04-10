#include <pthread.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/syscall.h>

const size_t numprocs = 4;
const int vec_size = 100;
int resultados[4];
int RANGE = 1000;
int *vector;

void *fillVector(void *id)
{
	long ID = (long)id;
	pid_t tid;
	tid = syscall(SYS_gettid);

	// int myPart =
	unsigned int i;
	printf("1ª Threads: %ld(%d) - Process ID: %5d Pai:%5d\n", ID, tid, getpid(), getppid());
	// printf("Thread: %ld\n",ID);
	for (i = ID; i < vec_size; i += numprocs)
	{
		vector[i] = rand() % RANGE;
	}
	pthread_exit(NULL);
}

void *getPartitionMax(void *id)
{
	long ID = (long)id;
	pid_t tid;
	tid = syscall(SYS_gettid);
	printf("2ª Threads: %ld(%d) - Process ID: %5d Pai:%5d\n", ID, tid, getpid(), getppid());

	unsigned int i;
	int max = vector[ID];
	for (i = ID; i < vec_size; i += numprocs)
	{
		if (vector[i] > max)
			max = vector[i];
	}
	resultados[ID] = max;
	pthread_exit(NULL);
}

int main()
{
	pid_t tid;
	tid = syscall(SYS_gettid);
	printf("Thread: (%d) - Process ID: %5d Pai:%5d\n", tid, getpid(), getppid());
	srand(time(NULL)); // Initialization, should only be called once.
	vector = (int *)malloc(vec_size * sizeof(int));

	//Idenficador de cada thread
	pthread_t handles[numprocs];

	/*Carrega os atributos padrões para criação das threads. Dentre os atributos, estão: prioridade no escalonamento e tamanho da pilha.*/
	pthread_attr_t attr;
	pthread_attr_init(&attr);

	//Cria as threads usando os atributos carregados.
	long i;
	for (i = 0; i < numprocs; i++)
	{
		pthread_create(&handles[i], &attr, fillVector, (void *)i);
	}

	// Espera todas as threads terminarem.
	for (i = 0; i != numprocs; ++i)
		pthread_join(handles[i], NULL); //NULL -> parâmetro de retorno

	puts("");
	//Exibe o vetor
	for (i = 0; i < vec_size; ++i)
		printf("%d, ", vector[i]);

	puts("\n");

	// long i;
	// Aqui descobriremos o maior valor de cada thread
	for (i = 0; i < numprocs; i++)
	{
		pthread_create(&handles[i], &attr, getPartitionMax, (void *)i);
	}

	// Espera todas as threads terminarem.
	for (i = 0; i != numprocs; ++i)
		pthread_join(handles[i], NULL); //NULL -> parâmetro de retorno

	puts("");

	/* Compara o resultado de cada thread. */
	int resultado = resultados[0];
	for (i = 0; i < numprocs; i++)
	{
		printf("[%d], ", resultados[i]);
		if (resultados[i] > resultado)
			resultado = resultados[i];
	}
	printf("\n\nO maior numero e %d\n", resultado);
	return 0;
}
