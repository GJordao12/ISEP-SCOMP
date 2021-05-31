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

#define MAX_PESSOAS 100
#define NOME 50
#define MORADA 50

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

struct pessoa
{
    int id;
    char nome[NOME];
    char morada[MORADA];
};

int verificaID(int identificacao, struct pessoa *pessoas, int num_pessoas)
{
    for (int i = 0; i < num_pessoas; i++)
    {
        if ((pessoas + i)->id == identificacao)
        {
            return 0;
        }
    }
    return 1;
}

int main()
{
    int i, num_pessoas = 0, opcao, identificacao, fd;
    struct pessoa *pessoas;

    fd = shm_open("/shm_pessoas", O_CREAT | O_TRUNC | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
        printf("ERROR with SHM_OPEN\n");
    }
    if (ftruncate(fd, sizeof(struct pessoa) * MAX_PESSOAS) == -1)
    {
        printf("ERROR TRUNCATING\n");
    }
    pessoas = mmap(NULL, sizeof(struct pessoa) * MAX_PESSOAS, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (pessoas == MAP_FAILED)
    {
        printf("ERROR MAPPING\n");
    }

    sem_t *sem1;

    //Cria/abre o semáforo
    //permissões 0644 (owner: read and write; group: read; other: read)
    if ((sem1 = sem_open("semaforo1", O_CREAT | O_EXCL, 0644, 1)) == SEM_FAILED)
    {
        perror("Error at sem_open().\n");
        exit(0);
    }

    do
    {
        printf("## Menu ##\n1-> Consultar Dados.\n2-> Inserir Dados.\n3-> Consultar todos os Dados.\n4-> Sair.\nOpção: ");
        scanf("%d", &opcao);
        printf("\n");
        switch (opcao)
        {
        case 1:
            if (num_pessoas > 0)
            {
                printf("Insira a identificação da pessoa a consultar: ");
                scanf("%d", &identificacao);
                int flag = 0;
                for (i = 0; i < num_pessoas; i++)
                {
                    if ((pessoas + i)->id == identificacao)
                    {
                        printf("\nPessoa: Identificação-> %d\nNome-> %sMorada-> %s\n", (pessoas + i)->id, (pessoas + i)->nome, (pessoas + i)->morada);
                        flag = 1;
                        break;
                    }
                }
                if (flag == 0)
                {
                    printf("\nERRO: Não existe nenhuma pessoa com essa identificação!\n\n");
                }
            }
            else
            {
                printf("ERRO: Não existem dados de momento!\n\n");
            }
            break;
        case 2:
            if (num_pessoas < MAX_PESSOAS)
            {
                int verifica = 0;

                down(sem1);

                do
                {
                    printf("Identificação: ");
                    scanf("%d", &((pessoas + num_pessoas)->id));
                    verifica = verificaID((pessoas + num_pessoas)->id, pessoas, num_pessoas);
                    fflush(stdin);
                    getchar();
                    if (verifica == 0)
                    {
                        printf("ERRO: Já existe uma pessoa com essa identificação!\n\n");
                    }
                } while (verifica == 0);

                printf("Nome: ");
                fgets((pessoas + num_pessoas)->nome, NOME, stdin);
                fflush(stdin);

                printf("Morada: ");
                fgets((pessoas + num_pessoas)->morada, MORADA, stdin);
                fflush(stdin);

                num_pessoas++;

                printf("\nINFO: Dados Inseridos com Sucesso!\n\n");

                up(sem1);
            }
            else
            {
                printf("WARNING: Atingiu o número máximo de pessoas!\n\n");
            }
            break;
        case 3:
            if (num_pessoas > 0)
            {
                for (i = 0; i < num_pessoas; i++)
                {
                    printf("Pessoa: Identificação-> %d\nNome-> %sMorada-> %s\n", (pessoas + i)->id, (pessoas + i)->nome, (pessoas + i)->morada);
                }
            }
            else
            {
                printf("ERRO: Não existem dados de momento!\n\n");
            }
            break;
        case 4:
            printf("Volte Sempre!\n");
            break;
        default:
            printf("ERRO: Opção inválida, tente novamente!\n\n");
        }
    } while (opcao != 4);

    //Fecha o semáforo
    if (sem_close(sem1) == -1)
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

    //Desfaz o mapeamento
    if (munmap(pessoas, sizeof(struct pessoa) * MAX_PESSOAS) < 0)
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
    if (shm_unlink("/shm_pessoas") < 0)
    {
        perror("No unlink()");
        exit(1);
    }

    return 0;
}
