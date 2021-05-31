#include <errno.h>
#include <unistd.h>
#include <wait.h>
#include <string.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

//#################### FORKS #####################
#define NBABIES 0

int babyMaker(int n)
{
    int p, i;
    for (i = 0; i < n; i++)
    {
        p = fork();
        if (p < 0)
            exit(-1);
        if (p == 0)
            return i + 1;
    }
    return 0;
}
//#################################################

//################ SEMÁFOROS ######################
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

void close_sem(sem_t *sem)
{
    if (sem_close(sem) == -1)
    {
        perror("Error at sem_close().\n");
        exit(0);
    }
}

void delete_sem(char *sem)
{
    if (sem_unlink(sem) == -1)
    {
        perror("Error at sem_unlink().\n");
        printf("Error: %s\n", strerror(errno));
        exit(0);
    }
}
//#################################################

//############ MEMÓRIA PARTILHADA #################
#define MEMORIA_PARTILHADA "/shm_exemplo"

struct exemplo
{
    int numero;
    char nome[20];
    int disciplinas[20];
};
//#################################################

int main()
{

    //############################ ABRIR MEMÓRIA PARTILHADA ##################################
    int fd;
    struct exemplo *ptr_exemplo;

    fd = shm_open(MEMORIA_PARTILHADA, O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
        printf("ERROR with SHM_OPEN\n");
    }

    if (ftruncate(fd, sizeof(struct exemplo)) == -1)
    {
        printf("ERROR TRUNCATING\n");
    }

    ptr_exemplo = (struct exemplo *)mmap(NULL, sizeof(struct exemplo), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (ptr_exemplo == MAP_FAILED)
    {
        printf("ERROR MAPPING\n");
    }
    //########################################################################################

    //######################### LEITURA DE INFORMAÇÃO PARA ESTRUTURA #########################
    printf("\nNúmero do Aluno: ");
    scanf("%d", &(ptr_exemplo->numero));
    fflush(stdin);
    getchar();

    printf("Nome do Aluno: ");
    fgets(ptr_exemplo->nome, 200, stdin);
    //Limpa o buffer
    fflush(stdin);

    printf("Nota da Disciplina: ");
    scanf("%d", &(ptr_exemplo->disciplinas[0]));
    fflush(stdin);
    getchar();

    //FAZER SLEEP
    srand(time(NULL));
    int r = (rand() % 10) + 1;
    sleep(r);
    //########################################################################################

    //###################################### SEMÁFOROS #######################################
    sem_t *sem_exemplo;

    if ((sem_exemplo = sem_open("sem_exemplo", O_CREAT | O_EXCL, 0644, 1)) == SEM_FAILED)
    {
        perror("Error at sem_open().\n");
        exit(0);
    }

    down(sem_exemplo);
    up(sem_exemplo);

    close_sem(sem_exemplo);
    delete_sem("sem_exemplo");
    //#########################################################################################

    //###################################### FORKS ############################################
    int id;

    id = babyMaker(NBABIES);
    if (id == 0)
    { //PAI
        wait(NULL);
    }
    else
    { //FILHO NR: ID
        exit(0);
    }
    //#########################################################################################

    //########################### FECHAR MEMÓRIA PARTILHADA ###################################
    if (munmap(ptr_exemplo, sizeof(struct exemplo)) < 0)
    {
        perror("No munmap()");
        exit(0);
    }

    if (close(fd) < 0)
    {
        perror("No close()");
        exit(0);
    }

    if (shm_unlink(MEMORIA_PARTILHADA) < 0)
    {
        perror("No unlink()");
        exit(1);
    }
    //##########################################################################################

    return 0;
}
