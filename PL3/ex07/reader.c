#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>

#define ARRAY_SIZE 10

typedef struct
{
	int array[ARRAY_SIZE];
} numbers;

int main()
{
	numbers *n;
	int fd, j, sum = 0;

	//Cria e verifica se a memória partilhada foi devidamente aberta
	//permissões de read, write, execute/search para user, grupo e owner
	if ((fd = shm_open("/shmHSP", O_CREAT | O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
		perror("Failed to create shared memory");

	//Ajusta o tamanho da memória partilhada
	if (ftruncate(fd, sizeof(numbers)) < 0)
		perror("Failed to adjust memory size");

	//Mapea a memória partilhada
	n = (numbers *)mmap(NULL, sizeof(numbers), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	//Verifica se a memória partilhada foi devidamente mapeada
	if (n == NULL)
	{
		perror("No mmap()");
		exit(0);
	}

	for (j = 0; j < ARRAY_SIZE; j++)
	{ 
		printf("Posicao %d do array: %d\n", j ,n->array[j]);
		sum = sum + n->array[j];
	}

	printf("Average of the 10 numbers: %d\n", sum / ARRAY_SIZE);

	//Desfaz o mapeamento
	if (munmap(n, sizeof(numbers)) < 0)
	{
		perror("No munmap()");
		exit(0);
	}

	//Fecha o descritor
	if (close(fd) < 0)
	{
		perror("No close()");
		exit(0);
	}

	//Apaga a memoria partilhada do sistema
	if (shm_unlink("/shmHSP") < 0)
	{
		perror("No unlink()");
		exit(1);
	}

	return 0;
}