#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <sys/wait.h> /* chamadas wait*() e macros relacionadas */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#define MAX3 10
#define MAX4 10
#define LINHAS 10
#define COLUNAS 50

void exercise1(){
    pid_t child = fork();
    if(child != 0){
        printf("Sou o filho, meu pid é %d e o pid do meu pai é %d\n", getpid(), getppid());
    }
}

void exercise2(){
    pid_t child = fork();
    if(child != 0){
        printf("Sou o pai, meu pid é %d e o pid do meu filho é %d\n", getpid(), child);
    }
    else{
        printf("Sou o filho, meu pid é %d e o pid do meu pai é %d\n", getpid(), getppid());
    }
}

void exercise3(){
    pid_t children[MAX3] = {0};
    pid_t exitcode[MAX3] = {0};
    for(size_t i = 0; i < MAX3; i++){
        children[i] = fork();
        if(children[i] == 0){
            printf("Sou o filho %d, meu pid é %d e o pid do meu pai é %d\n", i, getpid(), getppid());
            _exit(i);
        }
        else if (children[i] > 0){
            int status;
            wait(&status);
            if(WIFEXITED(status)){
                exitcode[i] = WEXITSTATUS(status);
            }
        }
        else{
            perror("fork");
            _exit(1);
        }
    }

    for(size_t i = 0; i < MAX3; i++){
        printf("O filho %d terminou com o código %d\n", children[i], exitcode[i]);
    }
}

void exercise4(){
    for(size_t i = 0; i < MAX4; i++){
        pid_t child = fork();
        if(child == 0){
            printf("Sou o filho %d, meu pid é %d e o pid do meu pai é %d\n", i, getpid(), getppid());
            _exit(i);
        }
        else if (child < 0){
            perror("fork");
            _exit(1);
        }
    }

    for(size_t i = 0; i < MAX4; i++){
        int status;
        pid_t child = wait(&status);
        if(WIFEXITED(status)){
            printf("O filho %d terminou com o código %d\n", child, WEXITSTATUS(status));
        }
    }
}

int exercicio5(){
    int status, find = 32;
    int matriz[LINHAS][COLUNAS];
    pid_t pid;

    for(int i = 0; i < LINHAS;i++){
        if(fork() == 0){
            for(int j = 0; j < COLUNAS; j++){
                matriz[i][j] = rand() % 100;
                if(matriz[i][j] == find){
                    _exit(i);
                }
            }
            _exit(-1);
        }
    }

    for(int i = 0; i < LINHAS; i++){
        pid = wait(&status);
        if(WIFEXITED(status)){
            if(WEXITSTATUS(status) != -1){
                printf("O filho %d encontrou o número %d na linha %d\n", pid, find, WEXITSTATUS(status));
            }
        }
    }
    return 0;
}

int exercicio6(){
    int status, find = 32;
    int matriz[LINHAS][COLUNAS];
    int matches[LINHAS] = {0};
    pid_t pid;

    for(int i = 0; i < LINHAS;i++){
        if(fork() == 0){
            for(int j = 0; j < COLUNAS; j++){
                matriz[i][j] = rand() % 100;
                if(matriz[i][j] == find){
                    _exit(i);
                }
            }
            _exit(-1);
        }
    }

    for(int i = 0; i < LINHAS;i++){
        pid = wait(&status);
        int linha = WEXITSTATUS(status);
        if(linha != 255)
            matches[linha] = 1;
    }

    for(int i = 0; i < LINHAS; i++){
        for (int j = 0; j < COLUNAS; j++)
        {
            printf("%2d ", matriz[i][j]);
        }
        printf("\n");
    }

    printf("\n\n");

    for(int i = 0; i < LINHAS; i++){
        if(matches[i] == 1){
            printf("Linha %d contém o número %d\n", i, find);
        }
    }

    return 0;

}


int main(int argc, char *argv[]){
    char str1[] = "exe1";
    if(strcmp(argv[1], str1) == 0){
        exercise1();
    }
    char str2[] = "exe2";
    if(strcmp(argv[1], str2) == 0){
        exercise2();
    }
    char str3[] = "exe3";
    if(strcmp(argv[1], str3) == 0){
        exercise3();
    }
    char str4[] = "exe4";
    if(strcmp(argv[1], str4) == 0){
        exercise4();
    }
    char str5[] = "exe5";
    if(strcmp(argv[1], str5) == 0){
        exercicio5();
    }
    
    return 0;
}