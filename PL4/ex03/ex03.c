#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>

#define MEM_AREA "/shm"
#define NUM_PROCESS 4
#define STRING "I'm the Father - with PID "
#define SEMAPHORE "/sema"
#define NUM_STRINGS 50
#define NUM_CHARS 80

typedef struct{
    int index;
    char line[NUM_STRINGS][NUM_CHARS];
}shared_data_type;

time_t t;

int main(void)
{

    int i = 0, fd, data_size = sizeof(shared_data_type);
    sem_t *sem;
    shared_data_type *s;

    /* Inicializa RNG (srand():stdlib.h; time(): time.h) */
    srand((unsigned)time(&t));

    // Create memory area
    fd = shm_open(MEM_AREA, O_CREAT | O_EXCL | O_RDWR, S_IRUSR | S_IWUSR);
    ftruncate(fd, data_size);
    s = (shared_data_type *)mmap(NULL, data_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    // Create semaphore
    if ((sem = sem_open(SEMAPHORE, O_CREAT, 0644, 1)) == SEM_FAILED)
    {
        perror("Error in sem_open()");
        exit(1);
    }

    for (i = 0; i < NUM_PROCESS; i++)
    {
        if (fork() == 0) /* Child process */
        {
            // Block semaphore
            sem_wait(sem);

            // Writing in memory area
            sprintf(s->line[s->index], "%s%d", STRING, getpid());

            s->index++;
            if (s->index >= NUM_STRINGS)
            {
                s->index = 0;
            }

            // Raise semaphore
            sem_post(sem);

            int n = (rand() % 5) + 1;
            
            sleep(n);

            // Close semaphore
            sem_close(sem);

            exit(0);
        }
        else
        {
            wait(NULL);
        }
    }

    /* Parent process */

    // Print strings in memory area
    for (i = 0; i < s->index; i++)
    {
        printf("%s\n", s->line[i]);
    }

    // Unlink shm
    if (shm_unlink(MEM_AREA) < 0)
    {
        printf("Error at shm_unlink()!\n");
        exit(EXIT_FAILURE);
    }
    // Disconnect map
    if (munmap(s, data_size) < 0)
    {
        printf("Error  munmap()!\n");
        exit(EXIT_FAILURE);
    }
    // Close fd
    if (close(fd) < 0)
    {
        printf("Error  close()!\n");
        exit(EXIT_FAILURE);
    }

    // Unlink semaphore
    sem_unlink(SEMAPHORE);

    return 0;
}