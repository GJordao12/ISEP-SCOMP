#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>

#define NBABIES 1
#define ARRAY_SIZE 10
#define NR_LOOPS 3

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

struct informacao
{
	int numeros[ARRAY_SIZE];
	int flag;
};

int main()
{
	int i, j, fd, id;
	struct informacao *data;

	// Cria e verifica se a memória partilhada foi devidamente aberta
	// permissões de read, write, execute/search para user, grupo e owner
	if ((fd = shm_open("/shmHSP", O_CREAT | O_RDWR | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
		perror("Failed to create shared memory");

	// Ajusta o tamanho da memória partilhada
	if (ftruncate(fd, sizeof(struct informacao)) < 0)
		perror("Failed to adjust memory size");

	// Mapea a memória partilhada
	data = (struct informacao *)mmap(NULL, sizeof(struct informacao), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	// Verifica se a memória partilhada foi devidamente mapeada
	if (data == NULL)
	{
		perror("No mmap()");
		exit(0);
	}

	id = babyMaker(NBABIES);

	if (id == 0)
	{ //PAI

		data->flag = 1;

		for (i = 0; i < NR_LOOPS; i++)
		{

			while (data->flag != 1)
				;

			for (j = 0; j < ARRAY_SIZE; j++)
			{
				data->numeros[j] = ((i * 10)) + j;
			}

			data->flag = 0;
		}

		wait(NULL);
	}
	else
	{ //FILHO
		int i, j, fd2;
		struct informacao *data2;

		//Cria e verifica se a memória partilhada foi devidamente aberta
		//permissões de read, write, execute/search para user, grupo e owner
		if ((fd2 = shm_open("/shmHSP2", O_CREAT | O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
			perror("Failed to create shared memory");

		//Ajusta o tamanho da memória partilhada
		if (ftruncate(fd2, sizeof(struct informacao)) < 0)
			perror("Failed to adjust memory size");

		//Mapea a memória partilhada
		data2 = (struct informacao *)mmap(NULL, sizeof(struct informacao), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		//Verifica se a memória partilhada foi devidamente mapeada
		if (data2 == NULL)
		{
			perror("No mmap()");
			exit(0);
		}

		for (i = 0; i < NR_LOOPS; i++)
		{
			while (data2->flag != 0)
				;
			for (j = 0; j < ARRAY_SIZE; j++)
			{
				printf("Número Recebido: %d\n", data2->numeros[j]);
			}
			data2->flag = 1;
		}

		//Desfaz o mapeamento
		if (munmap(data2, sizeof(struct informacao)) < 0)
		{
			perror("No munmap()");
			exit(0);
		}

		//Fecha o descritor
		if (close(fd2) < 0)
		{
			perror("No close()");
			exit(0);
		}

		//Apaga a memoria partilhada do sistema
		if (shm_unlink("/shmHSP2") < 0)
		{
			perror("No unlink()");
			exit(1);
		}

		exit(0);
	}

	//Desfaz o mapeamento
	if (munmap(data, sizeof(struct informacao)) < 0)
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