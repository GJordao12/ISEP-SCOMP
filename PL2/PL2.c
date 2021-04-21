#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <ctype.h>
#include <stdbool.h>

void pl2_ex01()
{
    int fd[2];
    pid_t pid;
    int test;

    /* cria o pipe */
    if (pipe(fd) == -1)
    {
        perror("Pipe failed");
    }
    pid = fork();
    if (pid > 0)
    {
        printf("Pid do pai: %d\n", getpid());
        /* fecha a extremidade não usada */
        close(fd[0]);
        /* escreve no pipe */

        test = getpid();
        write(fd[1], &test, sizeof(int));
        /* fecha a extremidade de escrita */
        close(fd[1]);
    }
    else
    {
        /* fecha a extremidade não usada */
        close(fd[1]);
        /* lê dados do pipe */
        int valor;
        read(fd[0], &valor, sizeof(int));
        printf("Filho leu: %d\n", valor);
        /* fecha a extremidade de leitura */
        close(fd[0]);
    }
}

void pl2_ex02a()
{
    int fd[2];
    pid_t pid;
    int value;
    char string[800];

    /* cria o pipe */
    if (pipe(fd) == -1)
    {
        perror("Pipe failed");
    }

    pid = fork();

    if (pid < 0)
    {
        perror("Fork failed");
    }

    if (pid > 0)
    {
        /* fecha a extremidade não usada */
        close(fd[0]);

        printf("Escolhe uma frase: ");
        fgets(string, 800, stdin);
        printf("Escolhe um valor: ");
        scanf("%d", &value);

        /* escreve no pipe */
        write(fd[1], &value, sizeof(int));
        write(fd[1], string, sizeof(string));

        /* fecha a extremidade de escrita */
        close(fd[1]);
    }
    else
    {
        /* fecha a extremidade não usada */
        close(fd[1]);

        /* lê dados do pipe */
        read(fd[0], &value, sizeof(int));
        read(fd[0], string, sizeof(string));
        printf("Filho leu a frase: %s", string);
        printf("Filho leu o valor: %d\n", value);

        /* fecha a extremidade de leitura */
        close(fd[0]);
    }
}

void pl2_ex02b()
{
    typedef struct
    {
        int num;
        char string[800];

    } data;

    int fd[2];
    pid_t pid;
    data informacao;

    if (pipe(fd) < 0)
    {
        perror("Pide failed");
    }

    pid = fork();

    if (pid < 0)
    {
        perror("Fork failed");
    }

    if (pid > 0)
    {
        /* fecha a extremidade não usada */
        close(fd[0]);

        printf("Escolhe uma frase: ");
        fgets(informacao.string, 800, stdin);
        printf("Escolhe um valor: ");
        scanf("%d", &informacao.num);

        /* escreve no pipe */
        write(fd[1], &informacao, sizeof(informacao));

        /* fecha a extremidade de escrita */
        close(fd[1]);

        exit(0);
    }
    else
    {
        /* fecha a extremidade não usada */
        close(fd[1]);

        /* lê dados do pipe */
        read(fd[0], &informacao, sizeof(data));

        printf("Filho leu a frase: %s", informacao.string);
        printf("Filho leu o valor: %d\n", informacao.num);

        /* fecha a extremidade de leitura */
        close(fd[0]);
        exit(0);
    }
}

#define BUFFERSZ 80
void pl2_ex03()
{
    int fd[2];
    pid_t pid;
    char string1[BUFFERSZ] = "Hello World";
    char string2[BUFFERSZ] = "Goodbye!";
    int status;

    /* cria o pipe */
    if (pipe(fd) == -1)
    {
        perror("Pipe failed");
    }

    pid = fork();

    if (pid < 0)
    {
        perror("Fork failed");
    }

    if (pid > 0)
    {
        /* fecha a extremidade não usada */
        close(fd[0]);
        /* escreve no pipe */
        write(fd[1], string1, strlen(string1) + 1);
        write(fd[1], string2, strlen(string2) + 1);

        /* fecha a extremidade de escrita */
        close(fd[1]);
        wait(&status);
        if (WIFEXITED(status))
        {
            printf("PID: %d\nExit Value: %d\n", pid, WEXITSTATUS(status));
        }
    }
    else
    {
        /* fecha a extremidade não usada */
        close(fd[1]);

        /* lê dados do pipe */
        read(fd[0], string1, BUFFERSZ);
        printf("Filho leu a frase: %s\n", string1);
        read(fd[0], string2, BUFFERSZ);
        printf("Filho leu a frase: %s\n", string2);

        /* fecha a extremidade de leitura */
        close(fd[0]);
        exit(0);
    }
}

#define BUFFERSZ4 5000
void pl2_ex04()
{
    char string[BUFFERSZ4];
    int fd[2];
    pid_t pid;

    /* cria o pipe */
    if (pipe(fd) == -1)
    {
        perror("Pipe failed");
    }

    pid = fork();

    if (pid < 0)
    {
        perror("Fork failed");
    }

    if (pid > 0)
    {
        FILE *fp;
        char ch;
        int index = 0;

        fp = fopen("File.txt", "r");
        if (fp == NULL)
        {
            perror("File not found!");
        }
        while ((ch = fgetc(fp)) != EOF)
        {
            string[index] = ch;
            index++;
        }
        fclose(fp);

        string[index] = '\0';

        close(fd[0]);
        write(fd[1], string, strlen(string) + 1);
        close(fd[1]);

        wait(NULL);
    }
    else
    {
        close(fd[1]);
        read(fd[0], string, BUFFERSZ4);
        printf("\nO filho leu: %s\n", string);
        close(fd[0]);
        exit(0);
    }
}

#define BUFFERSZ5 256
void pl2_ex05()
{
    int up[2], down[2], i = 0, status;
    pid_t pid;
    char string1[BUFFERSZ4];
    char ch;

    /* cria os pipes */
    if (pipe(up) < 0 || pipe(down) < 0)
    {
        perror("Pipe failed");
    }

    pid = fork();

    if (pid < 0)
    {
        perror("Fork failed");
    }

    if (pid > 0)
    {
        /* fecha a extremidade não usada */
        close(up[1]);
        /* lê no pipe */
        read(up[0], string1, BUFFERSZ5);

        /* fecha a extremidade de leitura */
        close(up[0]);

        while (string1[i] != '\0')
        {
            ch = string1[i];
            if (ch >= 65 && ch <= 90)
            {
                string1[i] = ch + 32;
            }
            else if (ch >= 97 && ch <= 122)
            {
                string1[i] = ch - 32;
            }
            i++;
        }

        close(down[0]);

        write(down[1], string1, strlen(string1) + 1);

        close(down[1]);
    }
    else
    {
        /* fecha a extremidade não usada */
        close(up[0]);

        printf("Escolhe uma frase: ");
        fgets(string1, BUFFERSZ, stdin);
        /* lê dados do pipe */
        write(up[1], string1, strlen(string1) + 1);

        /* fecha a extremidade de leitura */
        close(up[1]);

        waitpid(getppid(), &status, 0);

        close(down[1]);
        read(down[0], string1, BUFFERSZ5);

        close(down[1]);
        printf("O filho leu: %s", string1);
        exit(0);
    }
}

#define CHILD 5
#define SIZE 1000
void pl2_ex06()
{
    int fd[2];
    pid_t pid[CHILD];
    int vec1[SIZE], vec2[SIZE];
    int status, tmp = 0;
    int total;
    int i, j;

    if (pipe(fd) < 0)
    {
        perror("Pipe Failed");
    }

    for (i = 0; i < SIZE; i++)
    { //Criacao dos Vetores
        vec1[i] = 1;
        vec2[i] = 1;
    }

    for (i = 0; i < CHILD; i++)
    {
        pid[i] = fork();
        if (pid[i] == 0)
        { //Child process
            for (j = i * 200; j < (i + 1) * 200; j++)
            {
                tmp += vec1[j] + vec2[j];
            }
            close(fd[0]);
            write(fd[1], &tmp, sizeof(tmp));
            close(fd[1]);
            exit(0);
        }
    }
    close(fd[1]);
    for (i = 0; i < CHILD; i++)
    {
        waitpid(pid[i], &status, 0);
        if (WIFEXITED(status))
        {
            read(fd[0], &total, sizeof(total));
            tmp += total;
        }
    }

    close(fd[0]);
    printf("Soma total: %d\n", tmp);
}

#define CHILD7 5
#define SIZE7 1000
#define INDIVIDUAL_AMOUNT 200
void pl2_ex07()
{
    int vec1[SIZE7], vec2[SIZE7], results[SIZE7];
    int i, j;
    int status[CHILD7];
    pid_t pid[CHILD7];
    int fd[CHILD7][2];

    for (i = 0; i < CHILD7; i++)
    {
        pipe(fd[i]);
    }

    for (i = 0; i < SIZE7; i++)
    {
        vec1[i] = i;
        vec2[i] = i;
    }

    for (i = 0; i < CHILD7; i++)
    {
        pid[i] = fork();
        if (pid[i] == 0)
        {
            close(fd[i][0]);
            for (j = i * INDIVIDUAL_AMOUNT; j < ((i * INDIVIDUAL_AMOUNT) + INDIVIDUAL_AMOUNT); j++)
            {
                results[j] = vec1[j] + vec2[j];
            }
            write(fd[i][1], &(results[i * INDIVIDUAL_AMOUNT]), sizeof(int) * INDIVIDUAL_AMOUNT);
            close(fd[i][1]);
            exit(0);
        }
    }

    for (i = 0; i < CHILD7; i++)
    {
        waitpid(pid[i], &status[i], 0);
        close(fd[i][1]);
        read(fd[i][0], &(results[i * 200]), sizeof(int) * 200);
        close(fd[i][0]);
    }

    for (i = 0; i < SIZE7; i++)
    {
        printf("Valor da soma de %d com %d: %d\n", vec1[i], vec2[i], results[i]);
    }
}

#define CHILD8 10
void pl2_ex08()
{
    typedef struct
    {
        int round;
        char msg[4];

    } data;

    pid_t pid[CHILD8];
    int fd[2], i = 0, j = 1;
    int status[CHILD8];
    data informacao;
    strcpy(informacao.msg, "Win");

    if (pipe(fd) < 0)
    {
        perror("Pipe Failed");
    }

    for (i = 0; i < CHILD8; i++)
    {
        pid[i] = fork();

        if (pid[i] < 0)
        {
            perror("Fork Failed");
        }

        if (pid[i] == 0)
        {
            close(fd[1]);
            read(fd[0], &informacao, sizeof(data));
            close(fd[0]);

            printf("Filho %d: %s's round %d\n", i + 1, informacao.msg, informacao.round);
            exit(informacao.round);
        }
    }

    close(fd[0]);
    for (j = 1; j <= CHILD8; j++)
    {
        informacao.round = j;
        write(fd[1], &informacao, sizeof(informacao));
        sleep(2);
    }
    close(fd[1]);

    for (i = 0; i < CHILD8; i++)
    {
        waitpid(pid[i], &status[i], 0);
    }

    for (i = 1; i <= CHILD8; i++)
    { //Organizes winners per round and prints the pid of the winner
        for (j = 0; j < CHILD8; j++)
        {
            if (WEXITSTATUS(status[j]) == i)
            {
                printf("Vencedor da ronda %d: PID: %d\n", WEXITSTATUS(status[j]), pid[j]);
            }
        }
    }
}

#define CHILD9 10
#define UNITS 20
#define EACH 5000   // mudar para 5000
#define TOTAL 50000 // mudar para 50000
void pl2_ex09()
{
    typedef struct
    {
        int customer_code;
        int product_code;
        int quantity;

    } info;

    time_t t; /* É preciso para inicializar o gerador de números random (RNG) */
    info sales[TOTAL];
    pid_t pid[CHILD9];
    int fd[2], i = 0, k = 0, j = 0, pc, code, amount;
    int products[TOTAL];
    bool flag = false;

    /* Inicializa RNG (srand():stdlib.h; time(): time.h) */
    srand((unsigned)time(&t));

    /* Inicializa o array com os números random (rand(): stdlib.h) */
    for (i = 0; i < TOTAL; i++)
    {
        sales[i].customer_code = i;
        sales[i].product_code = i;
        sales[i].quantity = rand() % 35;
    }

    if (pipe(fd) < 0)
    {
        perror("Pipe Failed");
    }

    for (i = 0; i < CHILD9; i++)
    {
        pid[i] = fork();

        if (pid[i] < 0)
        {
            perror("Fork Failed");
        }

        if (pid[i] == 0)
        {
            close(fd[0]);
            for (k = (i * EACH); k < (i * EACH) + EACH; k++)
            {
                if (sales[k].quantity > UNITS)
                {
                    code = sales[k].product_code;
                    write(fd[1], &code, sizeof(int));
                }
            }
            close(fd[1]);

            exit(0);
        }
    }

    close(fd[1]);
    amount = 0;
    while (read(fd[0], &pc, sizeof(int)) != 0)
    {
        for (j = 0; j < sizeof(products) / sizeof(products[0]); j++)
        {
            if (products[j] == pc)
            {
                flag = true;
            }
            if (flag)
            {
                break;
            }
        }
        if (!flag)
        {
            products[amount] = pc;
            amount++;
        }
        else
        {
            flag = false;
        }
    }
    close(fd[0]);

    for (i = 0; i < amount; i++)
    {
        printf("O produto: %d foi vendido mais de 20 vezes!\n", products[i]);
    }
}

int main()
{
    printf("\nPL2-SCOMP 2021\n");

    //printf("\nExercício 1:\n");pl2_ex01();
    //printf("\nExercício 2a:\n");pl2_ex02a();
    //printf("\nExercício 2b:\n");pl2_ex02b();
    //printf("\nExercício 3:\n");pl2_ex03();
    //printf("\nExercício 4:\n");pl2_ex04();
    //printf("\nExercício 5:\n");pl2_ex05();
    //printf("\nExercício 6:\n");pl2_ex06();
    //printf("\nExercício 7:\n");pl2_ex07();
    //printf("\nExercício 8:\n");pl2_ex08();
    //printf("\nExercício 9:\n");pl2_ex09();
    return 0;
}