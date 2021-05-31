#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>

#define ARRAY_SIZE 1000000
#define RANGE 10
#define NBABIES 1

void populateArray(int n, int *vec){
    int i;
    time_t t;
    srand ((unsigned) time (&t));

    for (i = 0; i < n; i++)
        vec[i] = rand () % RANGE;
}

void showArray(int begin, int end, int *vec){
    int i;
    printf("Vec: ");
	for(int i=begin; i<end; i++)
		printf("%d | ", vec[i]);
    printf("\n");
}

int babyMaker(int n){
	int p, i;
	for(i=0; i<n; i++){
		p = fork();
		if(p<0) exit(-1); //ERRO
		if(p==0) return i+1; //FILHO i+1
	}
	return 0; //PAI
}

typedef struct {
	int numbers[ARRAY_SIZE];
	int flag;
} ShmDataType;

int main() {
	ShmDataType *shmData;
	int vec[ARRAY_SIZE];
	int fd, i, id;
	clock_t start, end;
	
    //////////////////////////// SHM ////////////////////////////
	
	// Cria e verifica se a memória partilhada foi devidamente aberta 
	// permissões de read, write, execute/search para user, grupo e owner 
	if((fd = shm_open("/shmHSP", O_CREAT | O_RDWR | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO)) <0)
		perror("Failed to create shared memory");

	// Ajusta o tamanho da memória partilhada
	if(ftruncate(fd, sizeof(ShmDataType))<0)
        perror("Failed to adjust memory size");

	// Mapea a memória partilhada 
	shmData = (ShmDataType *)mmap(NULL, sizeof(ShmDataType), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	// Verifica se a memória partilhada foi devidamente mapeada 
    if (shmData == NULL) {
        perror("No mmap()");
        exit(0);
    }

	populateArray(ARRAY_SIZE, vec);
	shmData->flag=0;
	
	id = babyMaker(NBABIES);
	
	start = clock();
	
	if(id == 0){ //PAI
		while(!shmData->flag); 
		//showArray(0, ARRAY_SIZE, shmData->numbers);
		end = clock();
		printf("Shared memory took %f sec \n", ((double) (end - start)) / CLOCKS_PER_SEC);
	}
	else{ //FILHO
		for (i = 0; i < ARRAY_SIZE; i++)
			shmData->numbers[i] = vec[i];
		shmData->flag=1;
		exit(0);
	}

    //Desfaz o mapeamento 
	if(munmap(shmData, sizeof(ShmDataType))<0){ 
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


    //////////////////////////// PIPE ////////////////////////////
	ShmDataType shmData_pipe;
	int fd_pipe[2];

	if (pipe(fd_pipe) == -1) {
        perror("Pipe failed");
        return 1;
    }

	id = babyMaker(NBABIES);

	start = clock();

	if(id == 0){ //PAI
        close(fd_pipe[1]); 
		for (i = 0; i < ARRAY_SIZE; i++)
        	read(fd_pipe[0], &shmData_pipe.numbers[i], sizeof(shmData_pipe.numbers[i]));
        close(fd_pipe[0]);
		//showArray(0, ARRAY_SIZE, shmData_pipe.numbers);
		end = clock();
		printf("Pipe took %f sec \n", ((double) (end - start)) / CLOCKS_PER_SEC);
	}
	else{ //FILHOS
		close(fd_pipe[0]);
		for (i = 0; i < ARRAY_SIZE; i++)
			write(fd_pipe[1], &vec[i], sizeof(vec[i]));
		close(fd_pipe[1]);
		exit(0);
	}

	return 0;
}