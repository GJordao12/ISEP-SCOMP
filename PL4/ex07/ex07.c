#include <errno.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define NBABIES 3
#define NSEM 3

int babyMaker(int n){
	int p, i;
	for(i=0; i<n; i++){
		p = fork();
		if(p<0) exit(-1); //ERRO
		if(p==0) return i+1; //FILHO i+1
	}
	return 0; //PAI
}

void down(sem_t *sem) {
    if (sem_wait(sem) == -1) {
        perror("Error at sem_wait().");
        exit(0);
    }
}

void up(sem_t *sem) {
    if (sem_post(sem) == -1) {
        perror("Error at sem_post().");
        exit(0);
    }
}

void printMessage(char *message) {
    printf("%s", message);
    fflush(stdout);
}

int main(int argc, char *agrv[]) {
    sem_t *sem[NSEM];
    char semName[NSEM][10] = {"SEM01", "SEM02", "SEM03"};
    int semValues[NSEM] = {1, 0, 0};

    //Cria/abre semáforos
	//permissões 0644 (owner: read and write; group: read; other: read) 
    int i;
    for (i = 0; i < NSEM; i++) {
        if ((sem[i] = sem_open(semName[i], O_CREAT | O_EXCL, 0644, semValues[i])) == SEM_FAILED) {
            perror("Error at sem_open().\n");
            exit(0);
        }
    }

    int id = babyMaker(NBABIES);

    if (id == 1) {
        down(sem[0]);
        printMessage("Sistemas ");
        up(sem[1]);

        down(sem[0]);
        printMessage("a ");
        up(sem[1]);

        exit(0);
    }

    if (id == 2) {
        down(sem[1]);
        printMessage("de ");
        up(sem[2]);

        down(sem[1]);
        printMessage("melhor ");
        up(sem[2]);

        exit(0);
    }

    if (id == 3) {
        down(sem[2]);
        printMessage("Computadores - ");
        up(sem[0]);

        down(sem[2]);
        printMessage("disciplina!\n");

        exit(0);
    }

    for (i = 0; i < NBABIES; i++) {
        wait(NULL);
    }

    //Fecha os semáforos
    for (i = 0; i < NSEM; i++) {
        if (sem_close(sem[i]) == -1) {
            perror("Error at sem_close().\n");
            exit(0);
        }
    }

    //Apaga os semáforos
    for (i = 0; i < NSEM; i++) {
        if (sem_unlink(semName[i]) == -1) {
            perror("Error at sem_unlink().\n");
            printf("Error: %s\n", strerror(errno));
            exit(0);
        }
    }

    return 0;
}