#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>

#define DESCRIPTION_SIZE 50

typedef struct
{
	int code;
	char desc[DESCRIPTION_SIZE];
	int price;
} product;

int main()
{
	product *p;
	int fd;

	// Cria e verifica se a memória partilhada foi devidamente aberta
	// permissões de read, write, execute/search para user, grupo e owner
	if ((fd = shm_open("/shmHSP", O_CREAT | O_RDWR | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO)) < 0)
		perror("Failed to create shared memory");

	// Ajusta o tamanho da memória partilhada
	if (ftruncate(fd, sizeof(product)) < 0)
		perror("Failed to adjust memory size");

	// Mapeia a memória partilhada
	p = (product *)mmap(NULL, sizeof(product), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	// Verifica se a memória partilhada foi devidamente mapeada
	if (p == NULL)
	{
		perror("No mmap()");
		exit(0);
	}

	printf("\nProduct Code: ");
	scanf("%d", &(p->code));
	fflush(stdin);
	getchar();

	printf("Product Description: ");
	fgets(p->desc, DESCRIPTION_SIZE, stdin);
	//Limpa o buffer
	fflush(stdin);

	printf("Product Price: ");
	scanf("%d", &(p->price));
	//Limpa o buffer
	fflush(stdin);
	getchar();

	//Desfaz o mapeamento
	if (munmap(p, sizeof(product)) < 0)
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