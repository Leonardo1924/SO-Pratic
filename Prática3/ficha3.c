#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <sys/wait.h> /* chamadas wait*() e macros relacionadas */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

void exercise1(){
    execlp("ls", "ls", "-l", NULL);
}

void exercise2(){
    pid_t pid;
    int status;
    pid = fork();
    if(pid == 0){
        execlp("ls", "ls", "-l", NULL);
    }
    if (pid < 0)
    {
        perror("fork");
        exit(1);
    }
    else{
        wait(&status);
        printf("Child process terminated with status: %d \n", status);
    }
}

void exercise3(int argc, char *argv[]){
    for(int i = 2; i < argc; i++){
        pid_t pid;
        int status;
        pid = fork();
        if(pid == 0){
            execlp(argv[i], argv[i], NULL);
            _exit(i);
        }
        if (pid < 0)
        {
            perror("fork");
            exit(1);
        }
    }

    for(int i = 2; i < argc; i++){
        int status;
        pid_t child = wait(&status);
        if(WIFEXITED(status)){
            printf("Child process %d terminated with status: %d \n", child, WEXITSTATUS(status));
        }
    }
}

int my_system(char *command, char **args) {
    pid_t pid;
    int status;

    pid = fork();
    if (pid == -1) {
        return -1;
    } else if (pid == 0) {
        /* Child process */
        execvp(command, args);
        /* execvp() only returns if an error occurs */
        exit(EXIT_FAILURE);
    } else {
        /* Parent process */
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            /* Child process terminated normally */
            return WEXITSTATUS(status);
        } else {
            /* Child process terminated abnormally */
            return -1;
        }
    }
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
        exercise3(argc, argv);
    }
    char str4[] = "exe4";
    if(strcmp(argv[1], str4) == 0){
        char *args[] = {argv[2], argv[3], NULL};
        my_system(argv[2], args);
    }
    return 0;
}
