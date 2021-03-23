#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

void pl1_ex01()
{
    int x = 1;
    pid_t p = fork(); /*pid_t: sys/types.h; fork(): unistd.h*/
    if (p == 0)
    { // Filho
        x = x + 1;
        printf("1. x = %d\n", x);
    }
    else
    { // Pai
        x = x - 1;
        printf("2. x = %d\n", x);
    }
    printf("3. %d; x = %d\n", p, x);
}

void pl1_ex01a()
{
    printf("2. x = 0\n1. x = 2\n3. 0; x = 2\n3. 1234; x = 0\n(Os printf's podem ter uma ordem diferente dependendo da velocidade dos processos)\n");
}

void pl1_ex01b()
{
    printf("Não é possivel garantir a ordem dos printf's selecionados, visto que o resultado\npode variar de acordo com a velocidade dos processos e respetivos escalonamentos.\n");
}

void pl1_ex02()
{
    printf("I'm..\n");
    if (fork() != 0)
    {                        // Pai
        wait(NULL);          // Pai espera que o 1º filho termine
        printf("..the..\n"); // O pai esperou que o 1º filho terminasse para escrever
        if (fork() != 0)
        {                        // Pai
            wait(NULL);          // Pai espera que o 2º filho termine
            printf("father!\n"); // O pai esperou que o 2º filho terminasse para escrever
        }
        else
        { // 2º Filho criado
            printf("I'll never join you!\n");
            exit(0); // 2º Filho termina
        }
    }
    else
    { // 1º Filho criado
        printf("I'll never join you!\n");
        exit(0); // 1º Filho termina
    }
    printf("I'll never join you!\n"); // Com os dois filhos terminados, apenas o pai escreve
}

void pl1_ex03()
{
    fork();
    fork();
    fork();
    printf("SCOMP!\n");
}

void pl1_ex03a()
{
    printf("Como estamos na presença de 3 forks serão criados 2^3 - 1 = 7 processos.\n(Existindo 8 processos contando com o pai inicial)\n");
}

void pl1_ex03b()
{
    printf("\n|\n");
    printf("|\n");
    printf("|___________________________________________|\n");
    printf("|                                           |\n");
    printf("|                                           |\n");
    printf("|_____________________|                     |_____________________|\n");
    printf("|                     |                     |                     |\n");
    printf("|                     |                     |                     |\n");
    printf("|__________|          |__________|          |__________|          |__________|\n");
    printf("|          |          |          |          |          |          |          |\n");
    printf("|          |          |          |          |          |          |          |\n");
    printf("|          |          |          |          |          |          |          |\n\n");
}

void pl1_ex03c()
{
    printf("Como estão a decorrer 8 processos e todos têm o printf então a mensagem vai aparecer 8 vezes.\n");
}

void pl1_ex04()
{
    int a = 0, b, c, d;
    b = (int)fork();
    c = (int)getpid(); /* getpid(), getppid(): unistd.h*/
    d = (int)getppid();
    a = a + 5;
    printf("\na=%d, b=%d, c=%d, d=%d\n", a, b, c, d);
}

void pl1_ex04a()
{
    printf("As únicas variáveis que vão ter valor igual em ambos os processos são as variáveis \"a\".\n");
    printf("A variável \"b\" do pai vai ter valor igual à variável \"c\" do filho.\n");
    printf("A variável \"c\" do pai vai ter valor igual à variável \"d\" do filho.\n");
}

void pl1_ex04b()
{
    printf("\n|\n");
    printf("|\n");
    printf("|\n");
    printf("|\n");
    printf("|___________________________________________|\n");
    printf("|                                           |\n");
    printf("|                                           |\n");
    printf("|                                           |\n");
    printf("|                                           |\n");
}

void pl1_ex05()
{
    int status;
    pid_t f1, f2;

    f1 = fork();

    if (f1 == 0)
    {
        sleep(1);
        exit(1);
    }

    f2 = fork();

    if (f2 == 0)
    {
        sleep(2);
        exit(2);
    }
    waitpid(f1, &status, 0);
    printf("Primeiro filho: %d\n", WEXITSTATUS(status));

    waitpid(f2, &status, 0);
    printf("Segundo filho: %d\n", WEXITSTATUS(status));
}

void pl1_ex06()
{
    int i;

    for (i = 0; i < 4; i++)
    {
        if (fork() == 0)
        {
            sleep(1); /*sleep(): unistd.h*/
        }
    }

    printf("This is the end.\n");
}

void pl1_ex06a()
{
    printf("Vão ser criados 2^4 - 1 = 15 processos (Existindo 16 processos contando com o pai inicial)\n\n");
    printf("                                   |\n");
    printf("                                   |\n");
    printf("                                   |__________________________________________________|\n");
    printf("                                   |                                                  |\n");
    printf("            |______________________|                        |_________________________|\n");
    printf("            |                      |                        |                         |\n");
    printf("            |                      |                        |                         |\n");
    printf("|___________|           |__________|           |____________|            |____________|\n");
    printf("|           |           |          |           |            |            |            |\n");
    printf("|           |           |          |           |            |            |            |\n");
    printf("|_____|     |_____|     |_____|    |_____|     |_____|      |_____|      |_____|      |_____|\n");
    printf("|     |     |     |     |     |    |     |     |     |      |     |      |     |      |     |\n");
    printf("|     |     |     |     |     |    |     |     |     |      |     |      |     |      |     |\n");
    printf("|     |     |     |     |     |    |     |     |     |      |     |      |     |      |     |\n");
}

void pl1_ex06b()
{
    printf("Alterar o \"4\" do ciclo \"for\" para \"2\" ou colocar a função exit após a função sleep.\n");
}

void pl1_ex06c()
{
    int i, status;
    pid_t pid[4];

    for (i = 0; i < 4; i++)
    {
        pid[i] = fork();
        if (pid[i] == 0)
        {
            sleep(1);
            printf("Filho: %d.\n", i + 1);
            exit(0);
        }
    }

    for (i = 0; i < (sizeof(pid) / sizeof(pid[0])); i++)
    {
        if (pid[i] % 2 == 0)
        {
            waitpid(pid[i], &status, 0);
        }
    }

    printf("This is the end.\n");
}

void pl1_ex06d()
{
    int i;
    int status;
    pid_t pid[4];

    for (i = 0; i < 4; i++)
    {
        pid[i] = fork();
        if (pid[i] == 0)
        {
            sleep(1);
            printf("Filho: %d.\n", i + 1);
            exit(i + 1);
        }
    }

    for (i = 0; i < (sizeof(pid) / sizeof(pid[0])); i++)
    {
        if (pid[i] % 2 == 0)
        {
            waitpid(pid[i], &status, 0);
            if (WIFEXITED(status))
            {
                int exitStatus = WEXITSTATUS(status);
                printf("Child with PID %d was exited with a status of %d\n", pid[i], exitStatus);
            }
        }
    }
    printf("This is the end.\n");
}

#define ARRAY_EX07_SIZE 1000
void pl1_ex07()
{

    int numbers[ARRAY_EX07_SIZE]; /* Array com os números random */
    int n;                        /* Número para procurar */
    time_t t;                     /* É preciso para inicializar o gerador de números random (RNG) */
    int i;                        /* Variavel para o ciclo "for" */
    int status;                   /* Guardar o valor de saída dos filhos */
    int total = 0;                /* Variável para incrementar a quantidade de vezes que encontra o número "n" */
    pid_t pid;                    /* PID do processo filho */

    /* Inicializa RNG (srand():stdlib.h; time(): time.h) */
    srand((unsigned)time(&t));

    /* Inicializa o array com os números random (rand(): stdlib.h) */
    for (i = 0; i < ARRAY_EX07_SIZE; i++)
        numbers[i] = rand() % 10000;

    /* Inicializa o "n" */
    n = rand() % 10000;

    pid = fork();
    if (pid < 0) /*Verificação de erro na criação do processo filho*/
    {
        perror("Fork não executado!");
        exit(-1); /* O programa termina com um código de erro*/
    }
    if (pid == 0) /* Processo filho*/
    {
        /*Verificação da primeira metade do array*/
        for (i = 0; i < ARRAY_EX07_SIZE / 2; i++)
        {
            if (numbers[i] == n)
            {
                total++;
            }
        }
        /*Retorna para o processo pai a quantidade de números igual a "n" existentes na primeira metade do array*/
        exit(total);
    }

    /*Verificação da segunda metade do array*/
    for (i = ARRAY_EX07_SIZE / 2; i < ARRAY_EX07_SIZE; i++)
    {
        if (numbers[i] == n)
        {
            total++;
        }
    }
    /*O processo pai espera que o processo filho termine*/
    waitpid(pid, &status, 0);
    /*Verifica se o processo filho terminou corretamente*/
    if (WIFEXITED(status))
    {
        /*Apresenta o valor total da soma de "n" encontrados em ambos os processos*/
        printf("O número %d, aparece %d vezes\n", n, WEXITSTATUS(status) + total);
    }
}

void pl1_ex08()
{
    pid_t p;

    if (fork() == 0)
    {
        printf("PID = %d\n", getpid());
        exit(0);
    }

    if ((p = fork()) == 0)
    {
        printf("PID = %d\n", getpid());
        exit(0);
    }

    printf("Parent PID = %d\n", getpid());

    printf("Waiting... (for PID=%d)\n", p);
    waitpid(p, NULL, 0);

    printf("Enter Loop...\n");
    while (1)
        ; /* Infinite loop */
}

void pl1_ex08a()
{
    printf("123 pts/0    00:00:57 PL1.o\n");
    printf("124 pts/0    00:00:00 PL1.o <defunct>\n");
    printf("Não, aparece o processo pai (123) e o primeiro filho (124) mas o último filho (125)\nnão aparece, porque tem a função wait executada no processo pai\n, logo é exited de imediato ao contrário do 124.\n");
}

void pl1_ex08b()
{
    printf("O Processo Filho aparece com \"<defunct>\", pois é um processo zombie.\n");
}

void pl1_ex09()
{
    pid_t pid[10];                /* Array para armazenar o pid dos filhos criados*/
    int i, j;                     /*Variáveis para os ciclos for*/
    int quantiadeDeNumeros = 100; /*quantidade de números que cada filho vai escrever*/
    int qtdProcessosFilhos = 10;  /* Quantidade de Processos Filhos que precisam de ser criados */

    /*Ciclo "for" para criar os 10 filhos*/
    for (i = 0; i < qtdProcessosFilhos; i++)
    {
        pid[i] = fork();
        if (pid[i] == 0)
        {
            /*Ciclo "for" para os prints dos 100 números de cada filho*/
            for (j = (i * quantiadeDeNumeros) + 1; j <= (i * quantiadeDeNumeros) + quantiadeDeNumeros; j++)
            {
                printf("%d\n", j);
            }
            exit(0);
        }
    }

    /*Ciclo "for" para o pai esperar pelos filhos que ainda não tenham terminado*/
    for (i = 0; i < 10; i++)
    {
        waitpid(pid[i], NULL, 0);
    }
}

void pl1_ex09a()
{
    printf("Não podemos garantir que o resultado esteja ordenado devido ao facto\n de que como os processos são concorrentes, pode variar\n a velocidade de cada processo filho e respetivos escalonamentos.\n");
}

#define ARRAY_EX10_SIZE 2000
void pl1_ex10()
{
    int numbers[ARRAY_EX10_SIZE]; /* Array com os números random */
    int i, j, k;                  /* Variáveis para os ciclos "for" */
    int n;                        /* Número para procurar */
    int status;                   /* Guardar o valor de saída dos filhos */
    int posicao;                  /* Guardar a posição do número "n" de cada filho caso tenha a ocorrência */
    int quantiadeDeNumeros = 200; /* Quantidade de números que cada filho vai percorrer */
    int qtdProcessosFilhos = 10;  /* Quantidade de Processos Filhos que precisam de ser criados */
    time_t t;                     /* É preciso para inicializar o gerador de números random (RNG) */
    pid_t pid[10];                /* Array que vai guardar o pid dos filhos */

    /* Inicializa RNG (srand():stdlib.h; time(): time.h) */
    srand((unsigned)time(&t));

    /* Inicializa o array com os números random (rand(): stdlib.h) */
    for (i = 0; i < ARRAY_EX10_SIZE; i++)
        numbers[i] = rand() % 1000;

    /* Inicializa o "n" */
    n = rand() % 1000;

    /*Ciclo "for" para a criação dos processos filhos */
    for (j = 0; j < qtdProcessosFilhos; j++)
    {
        pid[j] = fork();

        /*Processo Filho*/
        if (pid[j] == 0)
        {
            /* Ciclo "for" para a pesquisa dos 200 números de cada filho */
            for (k = (j * quantiadeDeNumeros); k < (j * quantiadeDeNumeros) + quantiadeDeNumeros; k++)
            {
                /* Verificação da ocorrência do número "n" */
                if (numbers[k] == n)
                {
                    /* Visto que o range da posição é de [0,200[ temos que retirar j*200 posições. */
                    posicao = k - j * 200;
                    /*Dá exit com a posição*/
                    exit(posicao);
                }
            }
            /* No caso de não ser encontrado o valor "n" retorna 255 */
            exit(255);
        }
    }

    /* Ciclo "for" para o pai esperar pelos filhos que ainda não tenham terminado e verificar o seus valores de saída */
    for (i = 0; i < 10; i++)
    {
        waitpid(pid[i], &status, 0);
        if (WIFEXITED(status))
        {
            /* Caso o valor de saída seja 255 informa que o filho não encontrou o número "n" */
            if (WEXITSTATUS(status) == 255)
            {
                printf("O filho nº %d não tem a ocorrência do número %d.\n", i + 1, n);
            }
            /* Caso o valor de saída seja diferente de 255 informa em que posição o filho encontrou o número "n" */
            else
            {
                printf("O filho nº %d tem a ocorrência do número %d na posição %d.\n", i + 1, n, WEXITSTATUS(status));
            }
        }
    }
}

#define ARRAY_EX11A_SIZE 1000
void pl1_ex11a()
{
    int numbers[ARRAY_EX11A_SIZE]; /* Array com os números random */
    int i, j, k;                   /* Variáveis para os ciclos "for" */
    int status;                    /* Guardar o valor de saída dos filhos */
    int max = -1;                  /*Guarda o maior valor*/
    int quantiadeDeNumeros = 200;  /* Quantidade de números que cada filho vai percorrer */
    int qtdProcessosFilhos = 5;    /* Quantidade de Processos Filhos que precisam de ser criados */
    time_t t;                      /* É preciso para inicializar o gerador de números random (RNG) */
    pid_t pid[5];                  /* Array que vai guardar o pid dos filhos */

    /* Inicializa RNG (srand():stdlib.h; time(): time.h) */
    srand((unsigned)time(&t));

    for (i = 0; i < ARRAY_EX11A_SIZE; i++)
    {
        numbers[i] = rand() % 256;
    }

    for (j = 0; j < qtdProcessosFilhos; j++)
    {
        pid[j] = fork();

        /*Processo Filho*/
        if (pid[j] == 0)
        {
            for (k = (j * quantiadeDeNumeros); k < (j * quantiadeDeNumeros) + quantiadeDeNumeros; k++)
            {
                if (numbers[k] > max)
                {
                    max = numbers[k];
                }
            }
            exit(max);
        }
    }

    /* Ciclo "for" para o pai esperar pelos filhos que ainda não tenham terminado e verificar o seus valores de saída */
    for (i = 0; i < 10; i++)
    {
        waitpid(pid[i], &status, 0);
        if (WIFEXITED(status))
        {
            printf("O valor máximo no intervalo [%d,%d[ é %d\n", i * 200, i * 200 + 200, WEXITSTATUS(status));
        }
    }
}

#define ARRAY_EX11B_C_SIZE 1000
void pl1_ex11b_c()
{
    int numbers[ARRAY_EX11B_C_SIZE]; /* Array com os números random */
    int result[ARRAY_EX11B_C_SIZE / 2];
    int i, j, k;                  /* Variáveis para os ciclos "for" */
    int status;                   /* Guardar o valor de saída dos filhos */
    int max = -1;                 /*Guarda o maior valor*/
    int quantiadeDeNumeros = 200; /* Quantidade de números que cada filho vai percorrer */
    int qtdProcessosFilhos = 5;   /* Quantidade de Processos Filhos que precisam de ser criados */
    time_t t;                     /* É preciso para inicializar o gerador de números random (RNG) */
    pid_t pid[5];                 /* Array que vai guardar o pid dos filhos */

    /* Inicializa RNG (srand():stdlib.h; time(): time.h) */
    srand((unsigned)time(&t));

    for (i = 0; i < ARRAY_EX11B_C_SIZE; i++)
    {
        numbers[i] = rand() % 256;
    }

    for (j = 0; j < qtdProcessosFilhos; j++)
    {
        pid[j] = fork();

        /*Processo Filho*/
        if (pid[j] == 0)
        {
            for (k = (j * quantiadeDeNumeros); k < (j * quantiadeDeNumeros) + quantiadeDeNumeros; k++)
            {
                if (numbers[k] > max)
                {
                    max = numbers[k];
                }
            }
            exit(max);
        }
    }

    max = -1;
    /* Ciclo "for" para o pai esperar pelos filhos que ainda não tenham terminado e verificar o seus valores de saída */
    for (i = 0; i < 10; i++)
    {
        waitpid(pid[i], &status, 0);
        if (WIFEXITED(status))
        {
            if (WEXITSTATUS(status) > max)
            {
                max = WEXITSTATUS(status);
            }
        }
    }

    if (fork() == 0)
    {
        for (i = 0; i < ARRAY_EX11B_C_SIZE / 2; i++)
        {
            result[i] = ((int)numbers[i] / max) * 100;
            printf("Nº%d: %d\n", i + 1, result[i]);
        }
        exit(0);
    }
    wait(NULL);
    for (i = ARRAY_EX11B_C_SIZE / 2; i < ARRAY_EX11B_C_SIZE; i++)
    {
        result[i] = ((int)numbers[i] / max) * 100;
        printf("Nº%d: %d\n", i + 1, result[i]);
    }
}

#define ARRAY_EX11D_SIZE 1000
void pl1_ex11d()
{
    int numbers[ARRAY_EX11D_SIZE]; /* Array com os números random */
    int result[ARRAY_EX11D_SIZE / 2];
    int i, j, k;                  /* Variáveis para os ciclos "for" */
    int status;                   /* Guardar o valor de saída dos filhos */
    int max = -1;                 /*Guarda o maior valor*/
    int quantiadeDeNumeros = 200; /* Quantidade de números que cada filho vai percorrer */
    int qtdProcessosFilhos = 5;   /* Quantidade de Processos Filhos que precisam de ser criados */
    time_t t;                     /* É preciso para inicializar o gerador de números random (RNG) */
    pid_t pid[5];                 /* Array que vai guardar o pid dos filhos */

    /* Inicializa RNG (srand():stdlib.h; time(): time.h) */
    srand((unsigned)time(&t));

    for (i = 0; i < ARRAY_EX11D_SIZE; i++)
    {
        numbers[i] = rand() % 256;
    }

    for (j = 0; j < qtdProcessosFilhos; j++)
    {
        pid[j] = fork();

        /*Processo Filho*/
        if (pid[j] == 0)
        {
            for (k = (j * quantiadeDeNumeros); k < (j * quantiadeDeNumeros) + quantiadeDeNumeros; k++)
            {
                if (numbers[k] > max)
                {
                    max = numbers[k];
                }
            }
            exit(max);
        }
    }

    max = -1;
    /* Ciclo "for" para o pai esperar pelos filhos que ainda não tenham terminado e verificar o seus valores de saída */
    for (i = 0; i < 10; i++)
    {
        waitpid(pid[i], &status, 0);
        if (WIFEXITED(status))
        {
            if (WEXITSTATUS(status) > max)
            {
                max = WEXITSTATUS(status);
            }
        }
    }

    if (fork() != 0)
    {
        for (i = ARRAY_EX11D_SIZE / 2; i < ARRAY_EX11D_SIZE; i++)
        {
            result[i] = ((int)numbers[i] / max) * 100;
        }
    }
    else
    {
        for (i = 0; i < ARRAY_EX11D_SIZE / 2; i++)
        {
            result[i] = ((int)numbers[i] / max) * 100;
            printf("Nº%d: %d\n", i + 1, result[i]);
        }
        exit(0);
    }
    wait(&status);

    if (WIFEXITED(status))
    {
        for (i = ARRAY_EX11D_SIZE / 2; i < ARRAY_EX11D_SIZE; i++)
        {
            printf("Nº%d: %d\n", i + 1, result[i]);
        }
    }
}

int spawn_childs(int n)
{
    int i;
    for (i = 0; i < n; i++)
    {
        if (fork() == 0)
        {
            return i + 1;
        }
    }
    return 0;
}

void pl1_ex12b()
{
    int i;
    int numeroFilhos = 6;
    int status;
    int posicao;

    posicao = spawn_childs(numeroFilhos);
    if (posicao > 0)
    {
        exit((posicao * 2);
    }

    for (i = 0; i < numeroFilhos; i++)
    {
        wait(&status);
        if (WIFEXITED(status))
        {
            printf("Child number %d\n", WEXITSTATUS(status));
        }
    }
}

int main()
{
    printf("\nPL1-SCOMP 2021\n");
    //printf("\nExercício 1:\n");pl1_ex01();
    //printf("\nExercício 1a:\n");pl1_ex01a();
    //printf("\nExercício 1b:\n");pl1_ex01b();
    //printf("\nExercício 2:\n");pl1_ex02();
    //printf("\nExercício 3:\n");pl1_ex03();
    //printf("\nExercício 3a:\n");pl1_ex03a();
    //printf("\nExercício 3b:\n");pl1_ex03b();
    //printf("\nExercício 3c:\n");pl1_ex03c();
    //printf("\nExercício 4:");pl1_ex04();
    //printf("\nExercício 4a:\n");pl1_ex04a();
    //printf("\nExercício 4b:\n");pl1_ex04b();
    //printf("\nExercício 5:\n");pl1_ex05();
    //printf("\nExercício 6:\n");pl1_ex06();
    //printf("\nExercício 6a:\n");pl1_ex06a();
    //printf("\nExercício 6b:\n");pl1_ex06b();
    //printf("\nExercício 6c:\n");pl1_ex06c();
    //printf("\nExercício 6d:\n");pl1_ex06d();
    //printf("\nExercício 7:\n");pl1_ex07();
    //printf("\nExercício 8:\n");pl1_ex08();
    //printf("\nExercício 8a:\n");pl1_ex08a();
    //printf("\nExercício 8b:\n");pl1_ex08b();
    //printf("\nExercício 9:\n");pl1_ex09();
    //printf("\nExercício 9a:\n");pl1_ex09a();
    //printf("\nExercício 10:\n");pl1_ex10();
    //printf("\nExercício 11a:\n");pl1_ex11a();
    //printf("\nExercício 11b e 11c:\n");pl1_ex11b_c();
    //printf("\nExercício 11d:\n");pl1_ex11d();
    //printf("\nExercício 12b:\n");pl1_ex12b();
    return 0;
}