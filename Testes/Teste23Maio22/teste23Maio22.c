#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int mensagens(char* palavra, char* ficheiro) {
    int pipefd[2];
    pipe(pipefd);

    pid_t grep_pid = fork();
    if (grep_pid == -1) {
        perror("Erro ao criar o processo filho (grep)");
        exit(1);
    } else if (grep_pid == 0) {
        // Filho (grep)
        close(pipefd[0]);  // Fecha a extremidade de leitura do pipe
        dup2(pipefd[1], STDOUT_FILENO);  // Redireciona a saída padrão para a extremidade de escrita do pipe
        close(pipefd[1]);  // Fecha a extremidade de escrita do pipe

        // Executa o comando "grep palavra ficheiro"
        execlp("grep", "grep", palavra, ficheiro, NULL);
        perror("Erro ao executar o comando grep");
        exit(1);
    } else {
        // Pai
        close(pipefd[1]);  // Fecha a extremidade de escrita do pipe

        int wc_pipefd[2];
        pipe(wc_pipefd);

        pid_t wc_pid = fork();
        if (wc_pid == -1) {
            perror("Erro ao criar o processo filho (wc)");
            exit(1);
        } else if (wc_pid == 0) {
            // Filho (wc)
            close(wc_pipefd[0]);  // Fecha a extremidade de leitura do pipe
            dup2(pipefd[0], STDIN_FILENO);  // Redireciona a entrada padrão para a extremidade de leitura do primeiro pipe
            close(pipefd[0]);  // Fecha a extremidade de leitura do primeiro pipe
            dup2(wc_pipefd[1], STDOUT_FILENO);  // Redireciona a saída padrão para a extremidade de escrita do segundo pipe
            close(wc_pipefd[1]);  // Fecha a extremidade de escrita do segundo pipe

            // Executa o comando "wc -l"
            execlp("wc", "wc", "-l", NULL);
            perror("Erro ao executar o comando wc");
            exit(1);
        } else {
            // Pai
            close(pipefd[0]);  // Fecha a extremidade de leitura do primeiro pipe
            close(wc_pipefd[1]);  // Fecha a extremidade de escrita do segundo pipe

            int grep_status, wc_status;
            waitpid(grep_pid, &grep_status, 0);
            waitpid(wc_pid, &wc_status, 0);

            if (WIFEXITED(grep_status) && WIFEXITED(wc_status)) {
                char buffer[10];
                int count = 0;

                // Lê a saída do segundo pipe (resultado do comando wc -l)
                while (read(wc_pipefd[0], buffer, sizeof(buffer)) > 0) {
                    count += atoi(buffer);
                }

                close(wc_pipefd[0]);  // Fecha a extremidade de leitura do segundo pipe

                return count;
            } else {
                perror("Erro na execução dos comandos grep e wc");
                exit(1);
            }
        }
    }
}

int autores_que_usaram(char* palavras, int n, char* autores[n]){
    int ocorrencias[n];
    pid_t pid[n];

    for(int i = 0; i < n; i++){
        pid[i] = fork();

        if(pid[i] == -1){
            perror("Erro ao criar o processo filho");
            exit(1);
        } else if (pid[i] == 0){
            //Filho
            int count = mensagens(palavras, autores[i]);
            exit(count);
        }
    }

    int total_ocorrencias = 0;
    for(int i = 0; i < n; i++){
        int status;
        waitpid(pid[i], &status, 0);

        if(WIFEXITED(status)){
            ocorrencias[i] = WEXITSTATUS(status);
            total_ocorrencias += ocorrencias[i];
        } 
    }

    for(int i = 0; i < n; i++){
        printf("Ocorrencias da palavra '%s' no ficheiro de '%s': %d\n", palavras, autores[i], ocorrencias[i]);
    }

    return total_ocorrencias;
}

int main(int argc, char* argv[]){

    char palavra[100];

    printf("Digite a palavra a ser pesquisada: ");
    scanf("%s",&palavra);

    if(strcmp("exe1",argv[1]) == 0){
        char ficheiro[100];


        printf("Digite o nome do ficheiro: ");
        scanf("%s",ficheiro);

        int ocorrencias = mensagens(palavra,ficheiro);
        printf("Ocorrencias da palavra '%s' no ficheiro '%s': %d\n", palavra , ficheiro, ocorrencias);
    } else {
        int n;
        printf("Digite o numero de autores: ");
        scanf("%d",&n);

        char* autores[n];
        for(int i = 0; i < n; i++){
            autores[i] = malloc(100 * sizeof(char));
            printf("Digite o nome do autor %d: ",i+1);
            scanf("%s",autores[i]);
        }

        int total_ocorrencias = autores_que_usaram(palavra,n,autores);
        printf("Total de ocorrencias da palavra '%s' nos ficheiros dos autores: %d\n", palavra, total_ocorrencias);

        for(int i = 0; i < n; i++){
            free(autores[i]);
        }
    }
    return 0;
}