#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <sys/wait.h> /* chamadas wait*() e macros relacionadas */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>


int main(int argc, char *argv[]) {
    int read_bytes;
    char buffer[1024];

    int fd = open("fifo", O_WRONLY); 

    while ((read_bytes = read(0, buffer, 1024)) > 0) {
        write(fd, buffer, read_bytes);
    } 

    close(fd);
    return 0;  
}