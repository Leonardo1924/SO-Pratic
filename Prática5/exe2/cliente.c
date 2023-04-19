#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <sys/wait.h> /* chamadas wait*() e macros relacionadas */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char *argv[]) {
    if(argc > 2){
        printf("Too many arguments\n");
        exit(1);
    }

    int fd = open("fifo", O_WRONLY); 
    write(fd, argv[1], strlen(argv[1]));
    close(fd);
    return 0;  
}