#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>

#define MAX_BUFFER_SIZE 256
#define NUM_REGIONS 5

int vacinados(char* regiao, int idade){
    int fd[2];
    pipe(fd);

    pid_t pid = fork();

    if(pid == -1){
        perror("Fork failed");
        exit(1);
    }

    if(pid == 0){
        close(fd[0]);
        dup2(fd[1], 1);
        execlp("grep", "grep", idade , "ficheiro", NULL);
        perror("Exec failed");
        exit(1);
    } else {
        close(fd[1]);
        wait(NULL);
        int count;
        read(fd[0], &count, sizeof(count));

        printf("Nuúmero de pacientes com %d anos: %d\n", idade, count);
    }

    return 0;
}

bool vacinado(char* cidadao){
    int fd[2];
    pipe(fd);

    pid_t pid = fork();
    if(pid == -1){
        perror("Fork failed");
        exit(1);
    }

    if(pid == 0){
        clode(fd[0]);
        dup2(fd[1], 1);
        execlp("grep", "grep", cidadao, "ficheiro", NULL);
        perror("Exec failed");
        exit(1);
    } else {
        clode(fd[1]);
        int status;
        pid_t result = waitpid(pid, &status, WNOHANG);

        if(result == -1){
            perror("Wait failed");
            exit(1);
        } else if (result == 0){
            return false;
        } else {
            if(WIFEXITED(status)){
                int exitStatus = WEXITSTATUS(status);
                if(exitStatus == 0){
                    return true;
                } else {
                    return false;
                }
            }
        }
    }
    return false;
}

int main(){
    int pipes[NUM_REGIONS][2];
    for(int i = 0; i < NUM_REGIONS; i++){
        if(pipe(pipes[i]) == -1){
            perror("Pipe failed");
            exit(1);
        }
    }

    int fd[NUM_REGIONS];
    fd[0] = open("regiao1.txt", O_WRONLY | O_CREAT | O_APPEND, 0666);
    fd[1] = open("regiao2.txt", O_WRONLY | O_CREAT | O_APPEND, 0666);
    fd[2] = open("regiao3.txt", O_WRONLY | O_CREAT | O_APPEND, 0666);
    fd[3] = open("regiao4.txt", O_WRONLY | O_CREAT | O_APPEND, 0666);
    fd[4] = open("regiao5.txt", O_WRONLY | O_CREAT | O_APPEND, 0666);

   char buffer[MAX_BUFFER_SIZE];

   while (1){
    for(int i = 0; i < NUM_REGIONS; i++){
        int flags = fcntl(pipes[i][0], F_GETFL);
        fcntl(pipes[i][0], F_SETFL, flags | O_NONBLOCK);

        ssize_t bytesRead = read(pipes[i][0], buffer, sizeof(buffer));
        if(bytesRead > 0){
            write(fd[i], buffer, bytesRead);
            write(fd[i], "\n", 1);
            printf("Dados Recebidos e armazenados na regiao %d\n", i+1);
        }
    }

        usleep(1000);
   }

    for(int i = 0; i < NUM_REGIONS; i++){
        close(pipes[i][0]);
        close(pipes[i][1]);
        close(fd[i]);
    }

    return 0;
}