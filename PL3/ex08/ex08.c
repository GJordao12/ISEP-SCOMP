#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>

#define RANGE 10
#define NBABIES 1
#define NLOOPS 1000000

int babyMaker(int n)
{
	int p, i;
	for (i = 0; i < n; i++)
	{
		p = fork();
		if (p < 0)
			exit(-1); //ERRO
		if (p == 0)
			return i + 1; //FILHO i+1
	}
	return 0; //PAI
}

typedef struct
{
	int number;
} data;

int main()
{
	data *shmData;
	int fd, i, id;

	// Cria e verifica se a memória partilhada foi devidamente aberta
	// permissões de read, write, execute/search para user, grupo e owner
	if ((fd = shm_open("/shmHSP", O_CREAT | O_RDWR | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
		perror("Failed to create shared memory");

	// Ajusta o tamanho da memória partilhada
	if (ftruncate(fd, sizeof(data)) < 0)
		perror("Failed to adjust memory size");

	// Mapea a memória partilhada
	shmData = (data *)mmap(NULL, sizeof(data), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	// Verifica se a memória partilhada foi devidamente mapeada
	if (shmData == NULL)
	{
		perror("No mmap()");
		exit(0);
	}

	id = babyMaker(NBABIES);

	shmData->number = 100;

	if (id == 0)
	{ //PAI
		for (i = 1; i <= NLOOPS; i++)
		{
			shmData->number++;
		}
		for (i = 1; i <= NLOOPS; i++)
		{
			shmData->number--;
		}
		printf("Number: %d\n", shmData->number);
		wait(NULL);
	}
	else
	{ //FILHO
		for (i = 1; i <= NLOOPS; i++)
		{
			shmData->number++;
		}
		for (i = 1; i <= NLOOPS; i++)
		{
			shmData->number--;
		}
		printf("Number: %d\n", shmData->number);
		exit(0);
	}

	//Desfaz o mapeamento
	if (munmap(shmData, sizeof(data)) < 0)
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

	printf("\nOs resultados vão ser sempre diferentes, porque os dois processos \nvão estar a mexer no mesmo valor concorrentemente.\n");

	return 0;
}