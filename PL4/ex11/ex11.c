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

#define MAX_PESSOAS 200
#define NBABIES 3
#define N_SEMAFOROS 3

int babyMaker(int n)
{
	int p, i;

	for(i=0; i<n; i++)
	{ p = fork();
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

void down(sem_t *sem)
{
    if (sem_wait(sem) == -1)
    {
        perror("Error at sem_wait().");
        exit(0);
    }
}

void up(sem_t *sem)
{
    if (sem_post(sem) == -1)
    {
        perror("Error at sem_post().");
        exit(0);
    }
}



int main()
{
    int cont_pessoas = 0, i;
    pid_t pid;
    char nomeSemaforos[N_SEMAFOROS][10] = {"SEM_01", "SEM_02", "SEM_03"};
    
    sem_t *semaforos[N_SEMAFOROS];

    //Cria/abre os semáforos
    //permissões 0644 (owner: read and write; group: read; other: read)
    for (i = 0; i < N_SEMAFOROS; i++)
    {
        if ((semaforos[i] = sem_open(nomeSemaforos[i], O_CREAT | O_EXCL, 0644, 1)) == SEM_FAILED)
        {
            perror("Error at sem_open()!\n");
            exit(EXIT_FAILURE);
        }
    }

    //tempo que a experiencia vai decorrer
    int tempo = 1000; 
  
    // tempo inicial
    clock_t start_time = clock();

    pid = babyMaker(NBABIES);

    /* Inicializa RNG (srand():stdlib.h; time(): time.h) */
    srand(time(NULL));

    if(pid == 1){
        while (clock()< start_time + tempo){
            // verifica se entra um passageiro (1) ou se sai um passageiro (0)
            int n = (rand() % 2);

            if(n == 1 && cont_pessoas < MAX_PESSOAS){
                down(semaforos[0]);
                cont_pessoas++;
                up(semaforos[0]);
                printf("Entrou 1 pessoa pela porta 1.\n");
                usleep(1000);
            }
            if(n == 1 && cont_pessoas == MAX_PESSOAS){
            up(semaforos[0]);
            printf("Lotação excedida! Não é possivel entrar mais pessoas!\n");
            usleep(1000);
            }
            //verifica se ha pessoas no comboio
            if(n == 0){
            down(semaforos[0]);
                if( cont_pessoas > 0){
                up(semaforos[0]);
                cont_pessoas--;
                    printf("Saiu 1 pessoa pela porta 1.\n");
                    usleep(1000);
                }else{
                     up(semaforos[0]);
                    printf("Não foi possivel sair nenhum passageiro!\n");
                    usleep(1000);
                }
            }
            
        }
        exit(0);
        
    }

    if(pid == 2){
        while (clock()< start_time + tempo){
            // verifica se entra um passageiro (1) ou se sai um passageiro (0)
            int n = (rand() % 2);

            if(n == 1 && cont_pessoas < MAX_PESSOAS){
                down(semaforos[1]);
                cont_pessoas++;
                up(semaforos[1]);
                printf("Entrou 1 pessoa pela porta 2.\n");
                usleep(1000);
            }
            if(n == 1 && cont_pessoas == MAX_PESSOAS){
            up(semaforos[1]);
            printf("Lotação excedida! Não é possivel entrar mais pessoas!\n");
            usleep(1000);
            }
            //verifica se ha pessoas no comboio
            if(n == 0){
            down(semaforos[1]);
                if( cont_pessoas > 0){
                up(semaforos[1]);
                cont_pessoas--;
                    printf("Saiu 1 pessoa pela porta 2.\n");
                    usleep(1000);
                }else{
                    up(semaforos[1]);
                    printf("Não foi possivel sair nenhum passageiro!\n");
                    usleep(1000);
                }
            }
            
        }
       exit(0); 
    }

    if(pid == 3){
        while (clock()< start_time + tempo){
            // verifica se entra um passageiro (1) ou se sai um passageiro (0)
            int n = (rand() % 2);

            if(n == 1 && cont_pessoas < MAX_PESSOAS){
                down(semaforos[2]);
                cont_pessoas++;
                up(semaforos[2]);
                printf("Entrou 1 pessoa pela porta 3.\n");
                usleep(1000);
            }
            if(n == 1 && cont_pessoas == MAX_PESSOAS){
            up(semaforos[2]);
            printf("Lotação excedida! Não é possivel entrar mais pessoas!\n");
            usleep(1000);
            }
            //verifica se ha pessoas no comboio
            if(n == 0){
            down(semaforos[2]);
                if( cont_pessoas > 0){
                up(semaforos[2]);
                cont_pessoas--;
                    printf("Saiu 1 pessoa pela porta 3.\n");
                    usleep(1000);
                }else{
                    up(semaforos[2]);
                    printf("Não foi possivel sair nenhum passageiro!\n");
                    usleep(1000);
                }
            }
            
        }
        exit(0);
    }

    for (i = 0; i < 3; i++)
    {
        wait(NULL);
    }

   // Fecha os Semáforos
    for (i = 0; i < N_SEMAFOROS; i++)
    {
        if (sem_close(semaforos[i]) == -1)
        {
            perror("Error at sem_close()!\n");
            exit(EXIT_FAILURE);
        }
    }

    // Remove os Semáforos
    for (i = 0; i < N_SEMAFOROS; i++)
    {
        if (sem_unlink(nomeSemaforos[i]) == -1)
        {
            perror("Error at sem_unlink()!\n");
            printf("Error: %s\n", strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}
