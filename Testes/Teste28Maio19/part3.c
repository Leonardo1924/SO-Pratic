#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>


#define NUM_INSTANCE 8

int main(int argc, char *argv[]){
    if(argc < 2){
        fprintf(stderr, "Uso: %s <padrão>\n", argv[0]);
        return 1;
    }

    char* padrao = argv[1];
    int counter = 0;

    for(int i = 0; i < NUM_INSTANCE; i++){
        pid_t pid = fork();

        if(pid < 0){
            perror("fork");
            exit(1);
        } else if ( pid == 0){
            dup2(STDOUT_FILENO, STDERR_FILENO);
            execlp("patgrep", "patgrep", padrao, NULL);

            perror("execlp");
            exit(1);
        }
    }

    for(int i = 0; i < NUM_INSTANCE; i++){
        int status;
        wait(&status);
        if(WIFEXITED(status)){
            if(WEXITSTATUS(status) == 0){
                counter++;
            }
        }
    }

    printf("O padrão %s aparece %d vezes no ficheiro %s\n", padrao, counter, argv[2]);

    return 0;
}