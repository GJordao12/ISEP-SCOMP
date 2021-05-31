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

#define BILHETES_DISPONIVEIS 2

int main()
{
    int fd, nbilhete;
    int *numero_ticktes;

    fd = shm_open("/shm_tickets", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
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

    nbilhete = *numero_ticktes;
    if (nbilhete == 0)
    {
        printf("INFO: Já não existem bilhetes disponíveis!\n");
        return 0;
    }

    sem_t *cliente;
    sem_t *vendedor;
    sem_t *aux;

    //Cria/abre o semáforo
    //permissões 0644 (owner: read and write; group: read; other: read)
    if ((cliente = sem_open("sem_cliente", O_CREAT, 0644, 1)) == SEM_FAILED)
    {
        perror("Error at sem_open().\n");
        exit(0);
    }

    if ((vendedor = sem_open("semaforo_vendedor", O_CREAT, 0644, 0)) == SEM_FAILED)
    {
        perror("Error at sem_open().\n");
        exit(0);
    }

    if ((aux = sem_open("semaforo_aux", O_CREAT, 0644, 0)) == SEM_FAILED)
    {
        perror("Error at sem_open().\n");
        exit(0);
    }

    down(cliente); //para mais nenhum cliente poder comprar
    up(vendedor);  //avisa o vendedor que quer comprar bilhete
    down(aux);     //espera que o vendedor vá buscar o bilhete
    nbilhete = *numero_ticktes;
    printf("Ficou com o bilhete: %d\n", nbilhete);
    up(cliente); // dá a vez ao próximo cliente para comprar

    return 0;
}
