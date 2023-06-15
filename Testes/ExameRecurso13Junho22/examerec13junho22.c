#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>


#define MAX_REQUESTS 5
#define MAX_ARG_LEN 20
#define LOG_FILE "comando.log"

typedef struct {
    char filtro[MAX_ARG_LEN];
    char entrada[MAX_ARG_LEN];
    char saida[MAX_ARG_LEN];
} Request;

int comando(const char* filtro, const char* entrada, const char* saida){
    int pipefd[2];
    pid_t pid;
    int stderr_flag = 0;

    if(pipe(pipefd) == -1){
        perror("pipe");
        return -1;
    }

    pid = fork();
    if(pid == -1){
        perror("fork");
        return -1;
    }
    if (pid == 0){
        close (pipefd[0]);
        dup2(pipefd[1], STDOUT_FILENO);
        close(pipefd[1]);

        execlp(filtro, filtro, (char*) NULL);

        perror("exec");
        exit(EXIT_FAILURE);
    } else {
        close(pipefd[1]);
        dup2(pipefd[0], STDIN_FILENO);
        close(pipefd[0]);

        FILE * output_file = open(saida, "w");
        if(output_file == NULL){
            perror("open");
            return -1;
        }

        char buffer[BUFSIZ];
        ssize_t bytes_read;
        while((bytes_read = read(STDIN_FILENO, buffer, BUFSIZ)) > 0){
            if(write(STDOUT_FILENO, buffer, bytes_read) == -1){
                perror("write");
                return -1;
            }
        }

        close(output_file);

        int status;
        waitpid(pid, &status, 0);
        if(WIFEXITED(status)){
            if(WEXITSTATUS(status) != 0){
                stderr_flag = 1;
            }
        }
        else{
            stderr_flag = 1;
        }
    }
    return !stderr_flag;
}

int process_requests(Request requests[]){
    FILE* log_file = fopen(LOG_FILE, "a");
    if(log_file == NULL){
        perror("fopen");
        return 0;
    }

    for(int i = 0; i < MAX_REQUESTS; i++){
        Request request = requests[i];
        int stderr_flag = comando(request.filtro, request.entrada, request.saida);

        if(stderr_flag){
            fprintf(log_file, "%s %s %s\n", request.filtro, request.entrada, request.saida);
        }
    }

    fclose(log_file);
    return 1;
}

int main(){
    const char* pipe_name = "requests";
    mkfifo(pipe_name, 0666);

    Request requests[MAX_REQUESTS];
    int request_count = 0;

    while(1){
        int fd = open(pipe_name, O_RDONLY);
        if(fd == -1){
            perror("open");
            return(EXIT_FAILURE);
        }
        
        while(request_count < MAX_REQUESTS){
            Request request;
            ssize_t bytes_read = read(fd, &request, sizeof(request));
            if(bytes_read == -1){
                perror("read");
                return(EXIT_FAILURE);
            }

            if(bytes_read == 0){
                break;
            }

            requests[request_count++] = request;
        }

        close(fd);

        if(request_count == MAX_REQUESTS){
            pid_t pid = fork();
            if(pid == -1){
                perror("fork");
                return(EXIT_FAILURE);
            }

            if(pid == 0){
                exit(process_requests(requests));
            } else {
                int stauts;
                waitpid(pid, &stauts, 0);
                request_count = 0;
            }
        }
    }

    return 0;
}