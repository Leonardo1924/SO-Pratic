#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>

#define MAX 100

int mycp(const char * from_path, const char* to_path){

    ssize_t res;
    //ssize_t resW;
    char buffer[1];


    int fd_origem = open(from_path,O_RDONLY);
    if(fd_origem < 0){
        perror("Error open");
        return -1;
    }

    int fd_destino = open(to_path, O_WRONLY|O_CREAT|O_TRUNC,0640);
    if (fd_destino < 0){
        perror("Error Open");
        return -1;
    }

    while((res = read(fd_origem,buffer,1)) > 0){
        
        write(fd_destino,buffer,res);
    }

    close(fd_origem);
    close(fd_destino);

    return 0;
}

int mycat(){

    ssize_t resR;
    char buffer[MAX];

    while((resR = read(STDIN_FILENO,buffer,MAX)) > 0){
        write(STDOUT_FILENO,buffer,resR);
    }
    
    return 0;

}

int myREADC(int FD, char *c){
    read(FD,c,1);
}

int myreadln(int fd, char *line, int size){
    char c;
    int i = 0;
    while(i < size){
        if(myREADC(fd,&c) <= 0){
            break;
        }
        if(c == '\n'){
            break;
        }
        line[i++]=c;
    }
    line[i]='\0';
    return i;
}

int mynl(){
    ssize_t resR;
    char buffer[MAX];
    int count = 1;

    while ((resR = read(STDIN_FILENO,buffer,MAX)) > 0){
        char line_num_str[MAX];
        sprintf(line_num_str, "     %d  ", count);
        write(STDOUT_FILENO, line_num_str, strlen(line_num_str));
        write(STDOUT_FILENO, buffer,resR);
        count++;
    }
    
    return 0;

}

int main(int argc, char *argv[]){
    int fdOrigem;
    char line[MAX];
    int size = 5;
    
    fdOrigem = open(argv[1],O_RDONLY);
    if(fdOrigem < 0){
        perror("Error open");
        return -1;
    }

    int n = myreadln(fdOrigem,line,5);
    printf("Read %d bytes: %s\n",n, line);
    close(fdOrigem);  


    //mynl();
    return 0;
}