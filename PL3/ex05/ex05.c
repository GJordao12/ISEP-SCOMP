#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>

#define TIMES 1000000
#define NBABIES 1

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
	int numero1;
	int numero2;
} numeros;

int main()
{
	numeros *n;
	int fd, tamanho = sizeof(numeros);
	pid_t pid;

	// Cria e verifica se a memória partilhada foi devidamente aberta
	// permissões de read, write, execute/search para user, grupo e owner
	if ((fd = shm_open("/shmHSP", O_CREAT | O_RDWR | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
		perror("Failed to create shared memory");

	// Ajusta o tamanho da memória partilhada
	if (ftruncate(fd, tamanho) < 0)
		perror("Failed to adjust memory size");

	// Mapea a memória partilhada
	n = (numeros *)mmap(NULL, tamanho, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	// Verifica se a memória partilhada foi devidamente mapeada
	if (n == NULL)
	{
		perror("No mmap()");
		exit(0);
	}

	n->numero1 = 8000;
	n->numero2 = 200;

	pid = babyMaker(NBABIES);

	if (pid < 0)
	{
		perror("Fork Failed");
	}

	if (pid > 0)
	{ // Processo pai
		int j = 0;
		while (j != TIMES)
		{
			n->numero1++;
			n->numero2--;
			j++;
		}
		printf("Numero 1: %d\n",n->numero1);
		printf("Numero 2: %d\n",n->numero2);
		wait(NULL);
	}
	if (pid == 0)
	{ // Processo filho
		int k = 0;
		while (k != TIMES)
		{
			n->numero1--;
			n->numero2++;
			k++;
		}
		printf("Numero 1: %d\n",n->numero1);
		printf("Numero 2: %d\n",n->numero2);
		exit(0);
	}

	//Desfaz o mapeamento
	if (munmap(n, tamanho) < 0)
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
}
