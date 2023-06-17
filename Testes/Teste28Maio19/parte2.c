#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>



#define MAX_LINE_LENGTH 10
#define MAX_MAIL_PROCESS 10

void send_email(char* aluno, char* nota){
    char command[120];
    snprintf(command, sizeof(command), "mail -s Sistemas_Operativos %s@alunos.uminho.pt", aluno);

    int mail_pipe[2];
    if(pipe(mail_pipe) == -1){
        perror("pipe");
        exit(1);
    }

    pid_t pid = fork();
    if(pid == -1){
        perror("fork");
        exit(1);
    } else if (pid == 0){
        close(mail_pipe[1]);
        dup2(mail_pipe[0], STDIN_FILENO);
        close(mail_pipe[0]);
        execlp("echo", "echo", command, NULL);
        perror("execlp");
        exit(1);
    } else {
        close(mail_pipe[0]);
        write(mail_pipe[1], nota, strlen(nota));
        close(mail_pipe[1]);
        int status;
        waitpid(pid, &status, 0);
        if(WIFEXITED(status)){
            printf("O aluno %s foi notificado com a nota %s\n", aluno, nota);
        }
        else{
            printf("O aluno %s não foi notificado com a nota %s\n", aluno, nota);
        }
    }
}

int main(int argc, char *argv[]){
    if(argc < 2){
        printf("Erro ao abrir o arquivo de notas %s\n", argv[1]);
        return 1;
    }

    int file = open(argv[1], O_RDONLY);
    if(file == -1){
        perror("open");
        return 1;
    }

    char line[MAX_LINE_LENGTH];
    int mail_process_count = 0;

    ssize_t read_bytes;

    while((read_bytes = read(file,line,sizeof(line))) > 0){
        char aluno[7], nota[4];
        sscanf(line, "%6s %3s", aluno, nota);

        if (mail_process_count >= MAX_MAIL_PROCESS){
            int status;
            wait(&status);
            mail_process_count--;
        }

        pid_t pid = fork();
        if(pid < 0){
            perror("fork");
            exit(1);
        } else if (pid == 0){
            send_email(aluno, nota);
            exit(0);
        } else {
            mail_process_count++;
        }
    }

    close(file);
    for(int i = 0; i < mail_process_count; i++){
        int status;
        wait(&status);
    }

    return 0;
}