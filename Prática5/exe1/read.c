#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <sys/wait.h> /* chamadas wait*() e macros relacionadas */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

int main(int argc, char *argv[]) {
    int read_bytes;
    char buffer[1024];

    int fd = open("fifo", O_RDONLY); 

    while ((read_bytes = read(fd, buffer, 1024)) > 0) {
        write(1, buffer, read_bytes);
    } 

    close(fd);
    return 0;  
}