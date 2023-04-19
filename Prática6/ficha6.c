#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#define terminal dup(1)

void exe1(){
    int fd = open("/etc/passwd", O_RDONLY,0640);
    int fd1 = open("saida.txt", O_RDWR | O_CREAT | O_TRUNC, 0640);
    int fd2 = open("erros.txt", O_RDWR | O_CREAT | O_TRUNC, 0640);
    dup2(fd,0);
    dup2(fd1,1);
    dup2(fd2,2);
    close(fd);
    close(fd1);
    close(fd2);
}

void exe2(){

    exe1();

    pid_t child = fork();
    if(child == 0){
        char buffer[1000] = {0};
        ssize_t r_bytes = read(0,buffer,1000);
        if(r_bytes < 0){
            perror("Erro na leitura");
            _exit(1);
        }
        write(1,buffer,r_bytes);
        _exit(0);
    }
    else if(child < 0){
        perror("Erro no fork");
        _exit(1);
    }
    else{
        int status;
        wait(&status);
        if(WIFEXITED(status)){
            printf("Terminou com sucesso");
        }
        else{
            printf("Terminou com erro");
        }
    }
}

void exe3(){
    exe1();
    int job = execvp("wc",NULL);
    if(job < 0){
        perror("Erro no exec");
        _exit(1);
    }
}

void exe4(){
    int fd[2];
    pid_t pid;
    int status;

    if(pipe(fd) < 0){
        perror("Erro no pipe");
        _exit(1);
    }

    pid = fork();

    if(pid < 0){
        perror("Error no fork");
        exit(1);
    }
    else if (pid == 0){
        close(fd[1]);
        if(dup2(fd[0], STDIN_FILENO) == -1){
            perror("Erro no dup2");
            _exit(1);
        }

        execlp("wc", "wc", NULL);

        perror("Erro no execlp");
        _exit(1);
    } else {
        close(fd[0]);
        char input[256];
        ssize_t bytesRead;
        char string[] = "Digite algumas linhas de texto (Ctrl+D para terminar):\n";
        write(STDOUT_FILENO, string, strlen(string));
        while((bytesRead = read(STDIN_FILENO, input, 256)) > 0){
            if (write(fd[1], input, bytesRead)) {
                perror("Erro no write");
                _exit(1);
            }
        }

        close(fd[1]);

        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) { 
            printf("O comando WC terminou com status de saida %d\n", WEXITSTATUS(status));
        } 
        else if (WIFSIGNALED(status)) { 
            printf("O comando WC terminou devido ao sinal %d\n", WTERMSIG(status));
        }
    }
}

int main(int argc , char *argv[]){
    char str[] = "exe1";
    if(strcmp(argv[1],str) == 0){
        exe1();
        write(terminal, "terminei\n",sizeof("terminei\n"));
    }
    char str2[] = "exe2";
    if(strcmp(argv[1],str2) == 0){
        exe2();
        write(terminal, "terminei\n",sizeof("terminei\n"));
    }
    char str3[] = "exe3";
    if(strcmp(argv[1],str3) == 0){
        exe3();
        write(terminal, "terminei\n",sizeof("terminei\n"));
    }
    char str4[] = "exe4";
    if(strcmp(argv[1],str4) == 0){
        exe4();
        write(terminal, "terminei\n",sizeof("terminei\n"));
    }
    return 0;
}