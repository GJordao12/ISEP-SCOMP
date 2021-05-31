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
#include <wait.h>

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

#define BILHETES_DISPONIVEIS 2

int main()
{
    int fd, qtd_bilhetes = 1;
    int *numero_ticktes;

    fd = shm_open("/shm_tickets", O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
        printf("ERROR with SHM_OPEN\n");
    }
    if (ftruncate(fd, sizeof(int)) == -1)
    {
        printf("ERROR TRUNCATING\n");
    }
    numero_ticktes = mmap(NULL, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (numero_ticktes == MAP_FAILED)
    {
        printf("ERROR MAPPING\n");
    }

    *numero_ticktes = 1;

    sem_t *vendedor;
    sem_t *aux;

    //Cria/abre o semáforo
    //permissões 0644 (owner: read and write; group: read; other: read)
    if ((vendedor = sem_open("semaforo_vendedor", O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED)
    {
        perror("Error at sem_open().\n");
        exit(0);
    }

    if ((aux = sem_open("semaforo_aux", O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED)
    {
        perror("Error at sem_open().\n");
        exit(0);
    }

    do
    {
        down(vendedor); //espera que um cliente chegue
        *numero_ticktes = qtd_bilhetes;
        srand(time(NULL));
        int r = (rand() % 10) + 1;
        sleep(r);
        printf("Já foram vendidos: %d\n", qtd_bilhetes);
        qtd_bilhetes++;
        up(aux); //entrega o bilhete ao cliente
    } while (qtd_bilhetes <= BILHETES_DISPONIVEIS);

    close_sem(vendedor);
    delete_sem("semaforo_vendedor");

    close_sem(aux);
    delete_sem("semaforo_aux");

    delete_sem("sem_cliente");

    //Desfaz o mapeamento
    if (munmap(numero_ticktes, sizeof(int)) < 0)
    {
        perror("No munmap()");
        exit(0);
    }

    //Fecha o descritor
    if (close(fd) < 0)
    {
        perror("No close()");
        exit(0);
    }

    //Apaga a memoria partilhada do sistema
    if (shm_unlink("/shm_tickets") < 0)
    {
        perror("No unlink()");
        exit(1);
    }

    return 0;
}
