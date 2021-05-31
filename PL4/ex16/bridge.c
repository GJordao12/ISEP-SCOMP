#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <time.h>
#include <signal.h>
#include <semaphore.h>

#define DIREITA 0
#define ESQUERDA 1
#define TEMPO_SIMULACAO 30
#define DELAY_NOVO_CARRO_MIN 10
#define DELAY_NOVO_CARRO_MAX 30

typedef struct ponte
{
    int direcao;
    int qtd_carros;
} ponte;

int randon_generator(int min, int max);

int main()
{
    srand(time(NULL));

    sem_t *sem_mutex = sem_open("sem_mutex", O_CREAT, S_IRUSR | S_IWUSR, 1);
    sem_t *sem_ponte = sem_open("sem_ponte", O_CREAT, S_IRUSR | S_IWUSR, 1);

    int fd = shm_open("/shm_ponte", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    ftruncate(fd, sizeof(ponte));
    ponte *ptr_ponte = (ponte *)mmap(NULL, sizeof(ponte), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    while (1)
    {
        int direcao_carro = randon_generator(DIREITA, ESQUERDA), direcao;
        if (fork() == 0)
        {
            pid_t pid = getpid();
            printf("[%d] Novo Carro na Ponte\n", pid);
            sem_wait(sem_mutex);
            direcao = ptr_ponte->direcao;
            if (direcao != direcao_carro)
            {
                printf("[%d] Direção Diferente, à espera\n", pid);
                sem_post(sem_mutex);
                sem_wait(sem_ponte);
                sem_wait(sem_mutex);
                printf("[%d] Mudando a direção da ponte e atravessando\n", pid);
                ptr_ponte->direcao = direcao_carro;
                (ptr_ponte->qtd_carros)++;
                sem_post(sem_mutex);
            }
            else
            {
                printf("[%d] Direção Igual, Atravessando\n", pid);
                (ptr_ponte->qtd_carros)++;
                sem_post(sem_mutex);
            }

            sleep(TEMPO_SIMULACAO);
            sem_wait(sem_mutex);
            (ptr_ponte->qtd_carros)--;
            if (ptr_ponte->qtd_carros == 0)
                sem_post(sem_ponte);
            sem_post(sem_mutex);
            printf("[%d] Ponte Atravessada\n", pid);
            return 0;
        }
        sleep(randon_generator(DELAY_NOVO_CARRO_MIN, DELAY_NOVO_CARRO_MAX));
    }

    munmap(ptr_ponte, sizeof(ponte));
    sem_unlink("sem_mutex");
    sem_unlink("sem_ponte");

    return 0;
}

int randon_generator(int min, int max)
{
    return (rand() % (max + 1 - min)) + min;
}
