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
	int fd, i;
	time_t t;

	// Cria e verifica se a memória partilhada foi devidamente aberta
	// permissões de read, write, execute/search para user, grupo e owner
	if ((fd = shm_open("/shmHSP", O_CREAT | O_RDWR | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
		perror("Failed to create shared memory");

	// Ajusta o tamanho da memória partilhada
	if (ftruncate(fd, sizeof(numbers)) < 0)
		perror("Failed to adjust memory size");

	// Mapeia a memória partilhada
	n = (numbers *)mmap(NULL, sizeof(numbers), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	// Verifica se a memória partilhada foi devidamente mapeada
	if (n == NULL)
	{
		perror("No mmap()");
		exit(0);
	}

	/* Inicializa RNG (srand():stdlib.h; time(): time.h) */
	srand((unsigned)time(&t));

	for (i = 0; i < ARRAY_SIZE; i++)
	{
		n->array[i] = (rand() % 20) + 1;
	}

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

	return 0;
}