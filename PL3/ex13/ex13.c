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
#include <ctype.h>

#define NBABIES 10

int main() {
	FILE *fp;
	int fd,i,j,status;

	struct st{
		char path[20];
		char word[20];
		int occurences;
	};
	
	pid_t pid[NBABIES];
	int size = sizeof(struct st);
	struct st *s;

		/*///////////////////SHARED MEMORY//////////////////*/

	if((fd = shm_open("/shmtest", O_CREAT | O_RDWR | O_TRUNC, S_IRWXU | S_IRWXG | S_IRWXO)) <0)
		perror("Failed to create shared memory");

	if(ftruncate(fd, size)<0)
		perror("Failed to adjust memory size");

	s = (struct st *)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	if (s == NULL) {
		perror("No mmap()");
		exit(0);
	}

		printf("Escolhe um ficheiro: ");
		scanf("%s",s->path);
		fflush(stdin);

		getchar();
		printf("Escolhe uma palavra: ");
        fgets(s->word, 20, stdin);

	    /*Ciclo "for" para a criação dos processos filhos */
    for (j = 0; j < NBABIES; j++)
    {
        pid[j] = fork();

        /*Processo Filho*/
        if (pid[j] == 0)
        {
        
        int oc = 0;

        fp = fopen(s->path, "r");
        if (fp == NULL)
        {
            perror("File not found!");
			return 0;
        }
		char *ch;
        while((ch = fgetc(fp)))
        {
			 
			if(strcmp(s->word,ch)){
				s->occurences = s->occurences + 1;
				oc++;
			}
        }
        fclose(fp);

		printf("Para o filho %d, existiram %d ocorrencias da palavra '%s'\n", j, oc,s->word);
			exit(0);
        }
    }

    /* Ciclo "for" para o pai esperar pelos filhos que ainda não tenham terminado e verificar o seus valores de saída */
    for (i = 0; i < NBABIES; i++)
    {
        waitpid(pid[i], &status, 0);
		
	}


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
