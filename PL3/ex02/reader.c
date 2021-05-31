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

typedef struct {
	int code;
    char desc[DESCRIPTION_SIZE];
	int price;
} product;

int main() {
	product *p;
	int fd;

	//Cria e verifica se a memória partilhada foi devidamente aberta 
	//permissões de read, write, execute/search para user, grupo e owner 
	if((fd = shm_open("/shmHSP", O_CREAT | O_RDWR, S_IRWXU | S_IRWXG | S_IRWXO)) <0)
		perror("Failed to create shared memory");

	//Ajusta o tamanho da memória partilhada
	if(ftruncate(fd, sizeof(product))<0)
        perror("Failed to adjust memory size");

	//Mapea a memória partilhada 
	p = (product *)mmap(NULL, sizeof(product), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	//Verifica se a memória partilhada foi devidamente mapeada 
    if (p == NULL) {
        perror("No mmap()");
        exit(0);
    }

	printf("\nProduct Code: %d", p->code);
	printf("\nProduct Description: %s", p->desc);
    printf("Product Price: %d\n", p->price);

	//Desfaz o mapeamento 
	if(munmap(p, sizeof(product))<0){ 
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