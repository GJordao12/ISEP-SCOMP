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
#define NLOOPS 1000000
#define STR_SIZE 50
#define NR_DISC 10

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

struct aluno
{
	int numero;
	char nome[STR_SIZE];
	int disciplinas[NR_DISC];
};

int main()
{
	int i, fd, id;
	struct aluno *data;

	// Cria e verifica se a memória partilhada foi devidamente aberta
	// permissões de read, write, execute/search para user, grupo e owner
	if ((fd = shm_open("/shmHSP", O_CREAT | O_RDWR | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
		perror("Failed to create shared memory");

	// Ajusta o tamanho da memória partilhada
	if (ftruncate(fd, sizeof(struct aluno)) < 0)
		perror("Failed to adjust memory size");

	// Mapea a memória partilhada
	data = (struct aluno *)mmap(NULL, sizeof(struct aluno), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	// Verifica se a memória partilhada foi devidamente mapeada
	if (data == NULL)
	{
		perror("No mmap()");
		exit(0);
	}

	printf("\nNúmero do Aluno: ");
	scanf("%d", &(data->numero));
	fflush(stdin);
	getchar();

	printf("Nome do Aluno: ");
	fgets(data->nome, STR_SIZE, stdin);
	//Limpa o buffer
	fflush(stdin);

	for (i = 1; i <= NR_DISC; i++)
	{
		printf("Nota da Disciplina %d: ", i);
		scanf("%d", &(data->disciplinas[i - 1]));
		fflush(stdin);
		getchar();
	}

	id = babyMaker(NBABIES);

	if (id == 0)
	{ //PAI

		wait(NULL);
	}
	else
	{ //FILHO
		int i, fd2, notaBaixa, notaAlta, media;
		struct aluno *data2;

		//Cria e verifica se a memória partilhada foi devidamente aberta
		//permissões de read, write, execute/search para user, grupo e owner
		if ((fd2 = shm_open("/shmHSP", O_CREAT | O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
			perror("Failed to create shared memory");

		//Ajusta o tamanho da memória partilhada
		if (ftruncate(fd2, sizeof(struct aluno)) < 0)
			perror("Failed to adjust memory size");

		//Mapea a memória partilhada
		data2 = (struct aluno *)mmap(NULL, sizeof(struct aluno), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		//Verifica se a memória partilhada foi devidamente mapeada
		if (data2 == NULL)
		{
			perror("No mmap()");
			exit(0);
		}

		notaBaixa = data->disciplinas[0];
		notaAlta = data->disciplinas[0];
		media = data->disciplinas[0];

		for (i = 1; i < NR_DISC; i++)
		{
			if (data->disciplinas[i] < notaBaixa)
			{
				notaBaixa = data->disciplinas[i];
			}
			if (data->disciplinas[i] > notaAlta)
			{
				notaAlta = data->disciplinas[i];
			}
			media = media + data->disciplinas[i];
		}

		printf("Nota mais alta = %d\n", notaAlta);
		printf("Nota mais baixa = %d\n", notaBaixa);
		printf("Média das notas = %d\n", media / NR_DISC);

		//Desfaz o mapeamento
		if (munmap(data2, sizeof(struct aluno)) < 0)
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

		exit(0);
	}

	//Desfaz o mapeamento
	if (munmap(data, sizeof(struct aluno)) < 0)
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