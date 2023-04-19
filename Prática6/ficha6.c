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

void exe5(){
    int fd[2];
    pid_t pid1 , pid2;
    int status1, status2;

    if(pipe(fd) < 0){
        perror("Erro no pipe");
        _exit(1);
    }

    pid1 = fork();
    
    if(pid1 < 0){
        perror("Error no fork");
        exit(1);
    }
    else if (pid1 == 0){
        close(fd[0]);
        if(dup2(fd[1], STDOUT_FILENO) == -1){
            perror("Erro no dup2");
            _exit(1);
        }
        execlp("ls", "ls","/etc", NULL);
        perror("Erro no execlp");
        _exit(1);
    }

    pid2 = fork();
    if(pid2 < 0){
        perror("Error no fork");
        exit(1);
    }
    else if (pid2 == 0){
        close(fd[1]);
        if(dup2(fd[0], STDIN_FILENO) == -1){
            perror("Erro no dup2");
            _exit(1);
        }
        execlp("wc", "wc", "-l" ,NULL);
        perror("Erro no execlp");
        _exit(1);
    }

    close(fd[0]);
    close(fd[1]);

    waitpid(pid1, &status1, 0);
    waitpid(pid2, &status2, 0);

     if (WIFEXITED(status1) && WIFEXITED(status2)) {
        printf("Os comandos /bin/ls /etc | /usr/bin/wc -l terminaram com status de saída %d e %d, respectivamente\n",
               WEXITSTATUS(status1), WEXITSTATUS(status2));
    } else {
        printf("Pelo menos um dos comandos foi terminado por um sinal\n");
    }
}

void exe6(){
    int fd[2];
  pid_t pid1, pid2, pid3, pid4;

  // Criando o pipe
  if (pipe(fd) == -1) {
    perror("Erro ao criar o pipe");
    exit(EXIT_FAILURE);
  }

  // Executando o comando grep -v ^# /etc/passwd
  pid1 = fork();
  if (pid1 == -1) {
    perror("Erro ao criar processo filho");
    exit(EXIT_FAILURE);
  } else if (pid1 == 0) {
    // Filho 1
    dup2(fd[1], STDOUT_FILENO); // Redirecionando a saída padrão para o pipe
    close(fd[0]); // Fechando o descritor de leitura do pipe
    execlp("grep", "grep", "-v", "^#", "/etc/passwd", NULL); // Executando o comando grep
    perror("Erro ao executar o comando grep");
    exit(EXIT_FAILURE);
  }

  // Executando o comando cut -f7 -d:
  pid2 = fork();
  if (pid2 == -1) {
    perror("Erro ao criar processo filho");
    exit(EXIT_FAILURE);
  } else if (pid2 == 0) {
    // Filho 2
    dup2(fd[0], STDIN_FILENO); // Redirecionando a entrada padrão para o pipe
    close(fd[1]); // Fechando o descritor de escrita do pipe
    execlp("cut", "cut", "-f7", "-d:", NULL); // Executando o comando cut
    perror("Erro ao executar o comando cut");
    exit(EXIT_FAILURE);
  }

  // Executando o comando uniq
  pid3 = fork();
  if (pid3 == -1) {
    perror("Erro ao criar processo filho");
    exit(EXIT_FAILURE);
  } else if (pid3 == 0) {
    // Filho 3
    dup2(fd[0], STDIN_FILENO); // Redirecionando a entrada padrão para o pipe
    close(fd[1]); // Fechando o descritor de escrita do pipe
    execlp("uniq", "uniq", NULL); // Executando o comando uniq
    perror("Erro ao executar o comando uniq");
    exit(EXIT_FAILURE);
  }

  // Executando o comando wc -l
  pid4 = fork();
  if (pid4 == -1) {
    perror("Erro ao criar processo filho");
    exit(EXIT_FAILURE);
  } else if (pid4 == 0) {
    // Filho 4
    dup2(fd[0], STDIN_FILENO); // Redirecionando a entrada padrão para o pipe
    close(fd[1]); // Fechando o descritor de escrita do pipe
    execlp("wc", "wc", "-l", NULL); // Executando o comando wc -l
    perror("Erro ao executar o comando wc");
    exit(EXIT_FAILURE);
  }

  // Fechando os descritores de arquivo do pipe no processo pai
    close(fd[0]);
    close(fd[1]);

    // Aguardando a finalização dos processos filhos
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);
    waitpid(pid3, NULL, 0);
    waitpid(pid4, NULL, 0);
    
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
    char str5[] = "exe5";
    if(strcmp(argv[1],str5) == 0){
        exe5();
    }
    char str6[] = "exe6";
    if(strcmp(argv[1],str6) == 0){
        exe6();
    }
    return 0;
}