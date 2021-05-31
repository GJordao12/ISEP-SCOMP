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

    sem_t *sem_array[8];
    char sem_nome[8][20];

    //Cria/abre o semáforo
    //permissões 0644 (owner: read and write; group: read; other: read)
    for (i = 0; i < NBABIES; i++)
    {
        sprintf(sem_nome[i], "sem_%d", i + 1);
        if (i == 0)
        {
            if ((sem_array[i] = sem_open(sem_nome[i], O_CREAT | O_EXCL, 0644, 1)) == SEM_FAILED)
            {
                perror("Error at sem_open().\n");
                exit(0);
            }
        }
        else
        {
            if ((sem_array[i] = sem_open(sem_nome[i], O_CREAT | O_EXCL, 0644, 0)) == SEM_FAILED)
            {
                perror("Error at sem_open().\n");
                exit(0);
            }
        }
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
        down(sem_array[id - 1]);

        printf("Sou o filho: %d\n", id);

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

        if (arqEscrita == NULL)
        {
            printf("Erro no ficheiro de escrita!\n");
            return -1;
        }

        for (i = 0; i < AMOUNT_NUMBERS; i++)
        {
            fprintf(arqEscrita, "%d\n", numeros[i]);
        }

        if (id < NBABIES)
        {
            up(sem_array[id]);
        }

        exit(0);
    }

    //Fecha o semáforo
    for (i = 0; i < NBABIES; i++)
    {
        if (sem_close(sem_array[i]) == -1)
        {
            perror("Error at sem_close().\n");
            exit(0);
        }
    }

    //Apaga o semáforo
    for (i = 0; i < NBABIES; i++)
    {
        if (sem_unlink(sem_nome[i]) == -1)
        {
            perror("Error at sem_unlink().\n");
            printf("Error: %s\n", strerror(errno));
            exit(0);
        }
    }

    return 0;
}