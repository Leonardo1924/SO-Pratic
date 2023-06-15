#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

void defeitos(char *imagens[], int n, int max){
  int processos_ativos = 0;
  for (int i = 0; i < n; i++)
  {
    while (processos_ativos >= max)
    {
      int status;
      wait(&status);
      processos_ativos--;
    }

    pid_t pid = fork();

    if (pid < 0)
    {
      perror("Error creating Child");
      exit(1);
    }
    else if (pid == 0)
    {
      execl("./detectAnom", "detectAnom", imagens[i], NULL);

      perror("Error executing detectAnom");
      exit(1);
    }
    else
    {
      processos_ativos++;
    }
  }
  
  while (processos_ativos > 0){
      int status;
      wait(&status);
      processos_ativos--;
  }
}

void conta(char *imagens[], int n) {
  int pipefd[2];
  pipe(pipefd);

  pid_t pid = fork();

  if(pid < 0){
    perror("Error creating Child");
    exit(1);
  } else if (pid == 0){
    dup2(pipefd[1],STDOUT_FILENO);
    close(pipefd[0]);
    close(pipefd[1]);

    defeitos(imagens, n, 10);

    exit(0);
  } else {

    close(pipefd[1]);
    wait(NULL);

    dup2(pipefd[0],STDIN_FILENO);

    close(pipefd[0]);

    execlp("wc", "wc", "-l", NULL);

  }
}