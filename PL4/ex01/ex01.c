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

#define NBABIES 8
#define AMOUNT_NUMBERS 200
#define INPUTFILE "Number.txt"
#define OUTPUTFILE "Output.txt"

int babyMaker(int n)
{
    int p, i;
    for (i = 0; i < n; i++)
    {
        p = fork();
        if (p < 0)
            exit(-1); //ERRO
        if (p == 0)
            return i + 1; //FILHO i+1
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

int main(int argc, char *argv[])
{
    int i, id;
    FILE *arqLeitura;
    FILE *arqEscrita;
    int numeros[AMOUNT_NUMBERS];

    sem_t *sem1;
    sem_t *sem2;

    //Cria/abre o semáforo
    //permissões 0644 (owner: read and write; group: read; other: read)
    if ((sem1 = sem_open("semaforo1", O_CREAT | O_EXCL, 0644, 1)) == SEM_FAILED)
    {
        perror("Error at sem_open().\n");
        exit(0);
    }
    if ((sem2 = sem_open("semaforo2", O_CREAT | O_EXCL, 0644, 1)) == SEM_FAILED)
    {
        perror("Error at sem_open().\n");
        exit(0);
    }

    id = babyMaker(NBABIES);

    arqLeitura = fopen(INPUTFILE, "r");
    arqEscrita = fopen(OUTPUTFILE, "a");

    if (id == 0)
    { //PAI
        for (i = 0; i < NBABIES; i++)
        {
            wait(NULL);
        }

        fclose(arqLeitura);
        fclose(arqEscrita);

        arqEscrita = fopen(OUTPUTFILE, "r");
        int numero;

        for (i = 0; i < AMOUNT_NUMBERS * NBABIES; i++)
        {
            fscanf(arqEscrita, "%d", &numero);
            printf("%d\n", numero);
        }

        fclose(arqEscrita);
    }
    else
    {
        down(sem1);

        if (arqLeitura == NULL)
        {
            printf("Erro no ficheiro de leitura!\n");
            return -1;
        }

        for (i = 0; i < AMOUNT_NUMBERS; i++)
        {
            fscanf(arqLeitura, "%d", &numeros[i]);
        }

        fclose(arqLeitura);

        up(sem1);

        down(sem2);

        if (arqEscrita == NULL)
        {
            printf("Erro no ficheiro de escrita!\n");
            return -1;
        }

        for (i = 0; i < AMOUNT_NUMBERS; i++)
        {
            fprintf(arqEscrita, "%d\n", numeros[i]);
        }

        up(sem2);

        exit(0);
    }

    //Fecha o semáforo
    if (sem_close(sem1) == -1)
    {
        perror("Error at sem_close().\n");
        exit(0);
    }

    if (sem_close(sem2) == -1)
    {
        perror("Error at sem_close().\n");
        exit(0);
    }

    //Apaga o semáforo
    if (sem_unlink("semaforo1") == -1)
    {
        perror("Error at sem_unlink().\n");
        printf("Error: %s\n", strerror(errno));
        exit(0);
    }

    if (sem_unlink("semaforo2") == -1)
    {
        perror("Error at sem_unlink().\n");
        printf("Error: %s\n", strerror(errno));
        exit(0);
    }

    return 0;
}