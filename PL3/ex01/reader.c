#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <time.h>

#define TAMANHO_NOME 50

typedef struct {
	char nome[TAMANHO_NOME];
	int numero;
} Aluno;

int main() {
	Aluno *a;
	int fd, tamanho=sizeof(Aluno);

	//Cria e verifica se a memória partilhada foi devidamente aberta 
	//permissões de read, write, execute/search para user, grupo e owner 
	if((fd = shm_open("/shmHSP", O_CREAT | O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO)) <0)
		perror("Failed to create shared memory");

	//Ajusta o tamanho da memória partilhada
	if(ftruncate(fd, tamanho)<0)
        perror("Failed to adjust memory size");

	//Mapea a memória partilhada 
	a = (Aluno *)mmap(NULL, tamanho, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	//Verifica se a memória partilhada foi devidamente mapeada 
    if (a == NULL) {
        perror("No mmap()");
        exit(0);
    }

	//São impressos: nome e número do aluno lidos da memória partilhada 
	printf("\nName = %s", a->nome);
	printf("\nNumber = %d", a->numero);
	printf("\n");

	//Desfaz o mapeamento 
	if(munmap(a, tamanho)<0){ 
        perror("No munmap()");
        exit(0);
    }

	//Fecha o descritor 
	if(close(fd)<0){ 
        perror("No close()");
        exit(0);
    }

	//Apaga a memoria partilhada do sistema 
    if(shm_unlink("/shmHSP")<0){ 
        perror("No unlink()");
        exit(1);
    }

	return 0;
}
