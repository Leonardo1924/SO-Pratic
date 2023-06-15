#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>

#define NUM_EXECS 10

int execute_program(char *program, char **args){
    pid_t pid = fork();
    if(pid < 0 ){
        perror("fork");
        exit(1);
    } else if (pid == 0){
        execvp(program, args);
        perror("Execution failed");
        exit(1);
    } else {
        int status;
        waitpid(pid, &status, 0);
        if(WIFEXITED(status)){
            return WEXITSTATUS(status);
        } else {
            return -1;
        }
    }
}

int main(int argc, char *argv[]){
    if(argc < 2){
        printf("Usage: %s <program> <args>\n", argv[0]);
        return 1;
    }

    char *program = argv[1];
    char **args = &argv[1];

    long min_memory = -1;
    long max_memory = -1;
    long total_memory = 0;

    for(int i = 0; i < NUM_EXECS;i++){
        int result = execute_program(program, args);
        if(result != 0){
            printf("Execution failed with code %d\n", result);
            return 1;
        }
    }

    FILE *memstats = NULL;
    char command[256];
    snprintf(command, suzeof(command), "grep VmPeak /proc/%d/status | cut -d' ' -f4", getpid());
    memstats = poepn(command, "r");

    if(memstats != NULL){
        long memory;
        if(fscanf(memstats, "%ld", &memory) == 1){
            if(min_memory == -1 || memory < min_memory){
                min_memory = memory;
            }
            if(max_memory == -1 || memory > max_memory){
                max_memory = memory;
            }
            total_memory += memory;
            pclose(memstats);
        } else {
            printf("Error reading memory stats\n");
            return 1;
        }
    }

    long avg_memory = total_memory / NUM_EXECS;

    printf("Memória: %ld %ld %ld\n", min_memory, avg_memory, max_memory);

    return 0;

}
