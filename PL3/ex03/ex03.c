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

#define NBABIES 1
#define STRUCT_NUMBER 100000

typedef struct{
	int number;
	char string[20];
}Structure;

int babyMaker(int n)
{
	int p, i;
	for(i=0; i<n; i++)
	{
		p = fork();

		if(p<0)
		{
			exit(-1); //ERRO
		}

		if(p==0)
		{
			return i+1; //FILHO i+1
		}
	}
	return 0; //PAI
}

void babyFuneral(int n)
{
	int i, status, exitcode, p;
	for(i=0; i<n; i++)
	{
		p = waitpid(-1,&status,0);

		if(WIFEXITED(status))
		{
			exitcode = WEXITSTATUS(status);
			printf("Baby %d has gone with exit code %d\n", p, exitcode);
		}
	}
}

void fillArrayStruct(Structure *array)
{
	for(int i = 0; i<STRUCT_NUMBER;i++)
	{
		Structure s;
		s.number = i;
		strcpy(s.string,"ISEP - SCOMP 2021");
		array[i] = s;
	}
}

int main() {

	Structure *s;
	Structure array[STRUCT_NUMBER];

	int fd,id;
	int size = sizeof(Structure);
	clock_t start, end;

	/*///////////////////SHARED MEMORY//////////////////*/
	printf("\nSHARED MEMORY: \n");

	if((fd = shm_open("/shmtest", O_CREAT | O_RDWR | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO)) <0)
		perror("Failed to create shared memory");

	if(ftruncate(fd, size)<0)
		perror("Failed to adjust memory size");

	s = (Structure *)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if (s == NULL) {
		perror("No mmap()");
		exit(0);
	}

	start = clock();

	id = babyMaker(NBABIES);

	if(id == -1)
	{
		printf("Error processing child\n");
	}

	else if(id == 0)
	{
		babyFuneral(NBABIES);

		end = clock();

		printf("Shared memory took %f sec \n", ((double) (end - start)) / CLOCKS_PER_SEC);
	}

	else
	{
		fillArrayStruct(array);
		exit(0);
	}

	if(munmap(s, size)<0){
		perror("No munmap()");
		exit(0);
	}

	if(close(fd)<0){
		perror("No close()");
		exit(0);
	}

	if(shm_unlink("/shmtest")<0){
		perror("No unlink()");
		exit(1);
	}


	printf("____________________________________\n");
	printf("\nPIPE: \n");

	/*//////////////////PIPE//////////////////*/

	int fd_pipe[2];
	if(pipe(fd_pipe) == -1)
	{
		perror("Error processing pipe.\n");
		return -1;
	}

	id = babyMaker(NBABIES);

	if(id == -1)
	{
		printf("Error processing child\n");
	}

	else if(id == 0)
	{
		close(fd_pipe[1]);
		read(fd_pipe[0],array,sizeof(array));
		close(fd_pipe[0]);
		babyFuneral(NBABIES);
		end = clock();
		printf("Pipe took %f sec \n", ((double) (end - start)) / CLOCKS_PER_SEC);
	}

	else
	{
		close(fd_pipe[0]);
		write(fd_pipe[1],array,sizeof(array));
		close(fd_pipe[1]);
		exit(0);
	}

	return 0;
}
