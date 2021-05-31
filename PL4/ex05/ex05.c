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

#define NBABIES 1

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

int main() {
	sem_t *sem;
	
	//Cria/abre o semáforo
	//permissões 0644 (owner: read and write; group: read; other: read) 
	if ((sem = sem_open("semaforo", O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED) {
		perror("Error at sem_open().\n");
		exit(0);
	}

	int id = babyMaker(NBABIES);

    if (id == 1) { //Filho
		printf("I'm the child.\n");
		up(sem); //Abre o semáforo para o pai correr
		exit(0);
	}
	else{  //Pai
		down(sem); //bloqueia
		printf("I'm the father.\n");
		up(sem);
	}

	//Fecha o semáforo
	if (sem_close(sem) == -1) {
		perror("Error at sem_close().\n");
		exit(0);
	}

	//Apaga o semáforo
	if (sem_unlink("semaforo") == -1) {
		perror("Error at sem_unlink().\n");
		printf("Error: %s\n", strerror(errno));
		exit(0);
	}
	
	return 0;
}
