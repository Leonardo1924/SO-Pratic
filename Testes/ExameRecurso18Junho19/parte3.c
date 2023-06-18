#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <sys/time.h>

#define MAX_URL_SIZE 100
#define MAX_PROCESS 8

/*
Considere um programa que lê URLs do seu stdin (uma por linha), 
descarrega a página Web correspondente e imprime asURLs nela contidas, 
cada uma escrita atomicamente num único write. Para obter todas as URLs nas páginas, 
sem repetições,usa-se na shell em 
conjunto com o programa uniq da seguinte forma:
$ fetchurls < seedURLs.txt | uniq
Escreva um programa em C com a mesma funcionalidade e fazendo uso dos programas
fetchurls e uniq
mas quepermita ter 8 processos a descarregar páginas concorrentemente. 
Assuma que cada URL não excede os 100 bytes e a 
existência de uma função readline, com o mesmo protótipo deread.
*/

int main() {
    int processos_ativos = 0;
    int pipefd[2];
    pipe(pipefd);

    while(processos_ativos >= MAX_PROCESS){
        int status;
        wait(&status);
        processos_ativos--;
    }

    pid_t pid = fork();
    if(pid < 0){
        perror("Error creating Child");
        exit(1);
    } else if (pid == 0){
        dup2(pipefd[1],STDOUT_FILENO);
        close(pipefd[0]);
        close(pipefd[1]);

        execl("./fetchurls", "fetchurls", NULL);

        perror("Error executing fetchurls");
        exit(1);
    } else {
        processos_ativos++;
    }
    
}