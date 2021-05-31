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

//############################### FORKS ######################################
#define NBABIES 3

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
//#############################################################################

//############################### SEMÁFOROS ###################################
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
//##############################################################################

//################################ VARIAVEIS ###################################
#define QTD_NUMEROS 10000
#define MAX_NUM_RANDOM 50
//##############################################################################

//######################### MEMÓRIA PARTILHADA #################################
#define MEMORIA_PARTILHADA "/shm_exame"

struct exame
{
    int numeros[QTD_NUMEROS / 2];
    int max;
};
//###############################################################################

int main()
{

    //####################################### ABRIR/CRIAR MEMÓRIA PARTILHADA #######################################
    int fd;
    struct exame *ptr_exame;

    fd = shm_open(MEMORIA_PARTILHADA, O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
        printf("ERROR with SHM_OPEN\n");
    }

    if (ftruncate(fd, sizeof(struct exame)) == -1)
    {
        printf("ERROR TRUNCATING\n");
    }

    ptr_exame = (struct exame *)mmap(NULL, sizeof(struct exame), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (ptr_exame == MAP_FAILED)
    {
        printf("ERROR MAPPING\n");
    }
    //################################################################################################################

    //############################################ ABRIR/CRIAR SEMÁFOROS #############################################
    sem_t *sem_um;
    sem_t *sem_dois;
    sem_t *sem_tres;

    if ((sem_um = sem_open("sem_um", O_CREAT | O_EXCL, 0644, 1)) == SEM_FAILED) // valor a 1, porque vai ser o semáforo do processo P1 que é o primeiro a fazer algo
    {
        perror("Error at sem_open().\n");
        exit(0);
    }

    if ((sem_dois = sem_open("sem_dois", O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED) // valor a 0, porque vai ter que esperar que o processo P1 dê permissões (up) iniciais
    {
        perror("Error at sem_open().\n");
        exit(0);
    }

    if ((sem_tres = sem_open("sem_tres", O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED) // valor a 0, porque vai ter que esperar que o processo P1 dê permissões (up) iniciais
    {
        perror("Error at sem_open().\n");
        exit(0);
    }
    //#################################################################################################################

    //####################################### PREENCHIMENTO COM NÚMEROS RANDOM ########################################
    int i;
    int num_random[QTD_NUMEROS];
    srand(time(NULL));

    for (i = 0; i < QTD_NUMEROS; i++)
    {
        num_random[i] = rand() % MAX_NUM_RANDOM + 1;
    }
    //##################################################################################################################

    //##################################################### FORKS ######################################################
    int id;

    id = babyMaker(NBABIES);

    if (id == 0)
    { //PAI
        int j;

        for (j = 0; j < NBABIES; j++) // espera que todos os filhos acabem
        {
            wait(NULL);
        }

        for (j = 0; j < QTD_NUMEROS / 2; j++)
        {
            printf("Posição %d: %d\n", j, ptr_exame->numeros[j]); // print dos números do vetor
        }
        printf("O maior valor é: %d\n", ptr_exame->max); // print do maior número
    }
    else
    { //FILHO NR: ID
        int pos, k;
        if (id == 1) // Processo P1
        {
            pos = 0; // começa a 0, porque é o primeiro a preencher o vetor
            for (k = 0; k < QTD_NUMEROS; k = k + 4)
            {
                down(sem_um);                                                // fica à espera que o processo P2 (não no início, porque o valor do semáforo é 1) lhe dê permissão para fazer o cálculo e guardar o valor na memória partilhada
                ptr_exame->numeros[pos] = num_random[k] * num_random[k + 1]; // multiplicação de pares consecutivos (k e k+1)
                pos = pos + 2;                                               // anda duas posições porque é alternado com o processo P2
                up(sem_dois);                                                // dá permissão ao processo P2 para fazer a sua multiplicação e guardar o seu valor
                up(sem_tres);                                                // dá permissão ao processo P3 para verificar se o número que ele (P1) inseriu passou a ser o maior valor
            }
        }
        if (id == 2) // Processo P2
        {
            pos = 1; // começa a 1, porque é o segundo a preencher o vetor
            for (k = 2; k < QTD_NUMEROS; k = k + 4)
            {
                down(sem_dois);                                              // fica à espera que o processo P1 lhe dê permissão para fazer o cálculo e guardar o valor na memória partilhada
                ptr_exame->numeros[pos] = num_random[k] * num_random[k + 1]; // multiplicação de pares consecutivos (k e k+1)
                pos = pos + 2;                                               // anda duas posições porque é alternado com o processo P1
                up(sem_um);                                                  // dá permissão ao processo P1 para fazer a sua multiplicação e guardar o seu valor
                up(sem_tres);                                                // dá permissão ao processo P3 para verificar se o número que ele (P2) inseriu passou a ser o maior valor
            }
        }
        if (id == 3) // Processo P3
        {
            int num_max = 0, num_memoria;
            for (k = 0; k < QTD_NUMEROS / 2; k++)
            {
                down(sem_tres); // fica à espera que tenha permissão do processo P1 ou P2 para verificar se um número inserido passou a ser o maior valor
                num_memoria = ptr_exame->numeros[k];
                if (num_memoria > num_max)
                {
                    num_max = num_memoria; // se o número inserido pelo processo P1 ou P2 for maior do que o que era o maior de momento esse passa a ser o maior agora
                }
            }
            ptr_exame->max = num_max; // depois de verificar todos os números guarda na memória partilhada o maior valor
        }
        exit(0);
    }
    //################################################################################################

    //########################### FECHAR/APAGAR SEMÁFOROS ############################################
    close_sem(sem_um);
    delete_sem("sem_um");

    close_sem(sem_dois);
    delete_sem("sem_dois");

    close_sem(sem_tres);
    delete_sem("sem_tres");
    //################################################################################################

    //########################### FECHAR/APAGAR MEMÓRIA PARTILHADA ###################################
    if (munmap(ptr_exame, sizeof(struct exame)) < 0)
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
    //#################################################################################################

    return 0;
}
