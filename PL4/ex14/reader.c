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

typedef struct
{
    int pid;
    char string[100];
    int nr_readers;
    int nr_writers;
} shared_memory;

int main()
{
    int fd;
    shared_memory *sm;

    fd = shm_open("/shm_ex14", O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
        printf("ERROR with SHM_OPEN\n");
    }
    if (ftruncate(fd, sizeof(shared_memory)) == -1)
    {
        printf("ERROR TRUNCATING\n");
    }
    sm = mmap(NULL, sizeof(shared_memory), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (sm == MAP_FAILED)
    {
        printf("ERROR MAPPING\n");
    }

    return 0;
}
