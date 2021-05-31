#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <time.h>
#include <string.h>

#define NBABIES 10
#define TOTAL 1000
#define L_ARRAY 10
#define CHILD_ARRAY 100

int main() {

	int fd,i,j,k,status,max = -1,maxTotal = -1;

	typedef struct{
		int array[L_ARRAY];
	}st;
	
	pid_t pid[NBABIES];
	time_t t;

	int size = sizeof(st), numbers[TOTAL];
	st * s;

	/* Inicializa RNG (srand():stdlib.h; time(): time.h) */
    srand((unsigned)time(&t));

	for (i = 0; i < TOTAL; i++)
	{
		numbers[i] = (rand() % 1001);
	}

		/*///////////////////SHARED MEMORY//////////////////*/

	if((fd = shm_open("/shmtest", O_CREAT | O_RDWR | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO)) <0)
		perror("Failed to create shared memory");

	if(ftruncate(fd, size)<0)
		perror("Failed to adjust memory size");

	s = (st *)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if (s == NULL) {
		perror("No mmap()");
		exit(0);
	}

	    /*Ciclo "for" para a criação dos processos filhos */
    for (j = 0; j < NBABIES; j++)
    {
        pid[j] = fork();

        /*Processo Filho*/
        if (pid[j] == 0)
        {
            /* Ciclo "for" para a pesquisa dos 100 números de cada filho */
            for (k = (j * CHILD_ARRAY); k < (j * CHILD_ARRAY) + CHILD_ARRAY; k++)
            {
                if (numbers[k] > max )
                {
                    max = numbers[k];
                }
            }
			s->array[j] = max;
			exit(i);
        }
    }

    /* Ciclo "for" para o pai esperar pelos filhos que ainda não tenham terminado e verificar o seus valores de saída */
    for (i = 0; i < NBABIES; i++)
    {
        waitpid(pid[i], &status, 0);
	}

	for(i = 0; i < L_ARRAY; i++){
		if (s->array[i] > maxTotal )
                {
                    maxTotal = s->array[i];
                }
	}

printf("O número máximo foi: %d\n", maxTotal);

//Desfaz o mapeamento
	if(munmap(s, size)<0){
		perror("No munmap()");
		exit(0);
	}
//Fecha o descritor
	if(close(fd)<0){
		perror("No close()");
		exit(0);
	}
//Apaga a memoria partilhada do sistema
	if(shm_unlink("/shmtest")<0){
		perror("No unlink()");
		exit(1);
	}
	
	return 0;
}
