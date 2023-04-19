#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <sys/wait.h> /* chamadas wait*() e macros relacionadas */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    ssize_t read_bytes;
    
    int log = open("log.txt", O_WRONLY | O_CREAT | O_APPEND, 0666);

    int fifo = mkfifo("fifo", 0666);
    if (mkfifo == -1) {
        perror("mkfifo");
        exit(1);
    }

    for(char buffer[1024] = {0};;){
        if(fork() == 0){
            int fd = open("fifo",O_RDONLY, 0666);
            for(read_bytes = 0; read_bytes = read(fd, buffer, 1024) > 0;){
                write(log, buffer, read_bytes);
            }
            close(fd);
        }
        wait(NULL);
    }
    close(log);
}