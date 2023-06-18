#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>
#include <sys/time.h>

#define MAX_URL_LENGTH 100
#define MAX_CONCURRENT_PROCESSES 10
#define TIMEOUT_SECONDS 60


int main(int argc, char *argv[]){
    if(argc < 2){
        printf("Usage: %s nome_do_ficheiro\n", argv[0]);
        exit(1);
    }

    const char *input_file = "input.txt";
    const char *output_file = argv[1];

    int input_fd = open(input_file, O_RDONLY);
    if(input_fd == -1){
        perror(input_file);
        exit(2);
    }

    int output_fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC | O_APPEND, 0644);
    if(output_fd == -1){
        perror(output_file);
        close(input_fd);
        exit(1);
    }

    char url[MAX_URL_LENGTH];
    int process_count = 0;
    struct timeval start_time, current_time;
    gettimeofday(&start_time, NULL);

    while(read(input_fd, url, sizeof(url)) > 0){
        while(process_count >= MAX_CONCURRENT_PROCESSES){
            gettimeofday(&current_time, NULL);
            double elapsed_time = (current_time.tv_sec - start_time.tv_sec) +
                                  (current_time.tv_usec - start_time.tv_usec) / 1000000.0;

            if(elapsed_time >= TIMEOUT_SECONDS){
                printf("Timeout!\n");
                break;
            }

            int status;
            pid_t terminated_pid;
            while((terminated_pid = waitpid(-1, &status, WNOHANG)) > 0){
                if(WIFEXITED(status) || WIFSIGNALED(status)){
                    printf("Process %d terminated\n", terminated_pid);
                    process_count--;
                }
            }
        }
        usleep(100000);
    }

    if(process_count < MAX_CONCURRENT_PROCESSES){
        pid_t pid = fork();
        if(pid == -1){
            perror("fork");
            exit(1);
        }
        else if(pid == 0){
            execl("./geturls", "geturls", url, NULL);
            perror("execlp");
            exit(1);
        } else {
            process_count++;
        }
    }

    while(process_count > 0){
        int status;
        pid_t terminated_pid = wait(&status);
        if(WIFEXITED(status) || WIFSIGNALED(status)){
            printf("Process %d terminated\n", terminated_pid);
            process_count--;
        }
    }
}