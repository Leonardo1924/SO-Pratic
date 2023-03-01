#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

#include "../includes/person.h"

Person p;

int insertPerson(char const *name, char const *age) {
    int fd;
    fd = open("registo", O_CREAT| O_WRONLY|O_APPEND, 0600);
        if(fd == -1) {
            perror("Error Creating or Opening File");
            return 1;
        }
        strcpy(p.name, name);
        p.age = atoi(age);

        if (write(fd, &p, sizeof(Person)) < 1) {
            perror("Error Writing to File");
            return -1;
        }
        close(fd);
        return 0;
}

int readPerson(){
    int fd;
        fd = open("registo", O_RDONLY,0666);
        if(fd < 0) {
            perror("Error Opening File");
            return -1;
        }
        while(read(fd, &p, sizeof(Person)) > 0) {
            printf("Name: %s\nAge: %d\n", p.name, p.age);
        }
        close(fd);
        return 0;
}

int updatePersonAge(char const *name, char const *age) {
    int fd;
    fd = open("registo", O_RDWR);
        if(fd == -1) {
            perror("Error Creating or Opening File");
            return 1;
        }
    
    while(read(fd, &p, sizeof(Person)) > 0) {
        if (strcmp(p.name, name) == 0) {
            p.age = atoi(age);
            lseek(fd, -sizeof(Person), SEEK_CUR);
            write(fd, &p, sizeof(Person));
            }
        }
    close(fd);
    return 0;
}

int updateByPosition(int total, char const *pos, char const *age){
    int fd;
    fd = open("registo", O_RDWR);
        if(fd == -1) {
            perror("Error Creating or Opening File");
            return 1;
        }
    int position = atoi(pos);
    if (position > total) {
        perror("Error: Position out of range");
        return -1;
    }
    lseek(fd, (position - 1) * sizeof(Person), SEEK_SET);
    read(fd, &p, sizeof(Person));
    p.age = atoi(age);
    lseek(fd, -sizeof(Person), SEEK_CUR);
    write(fd, &p, sizeof(Person));
    close(fd);
    return 0;
}

int sizeofFile() {
    int fd;
    fd = open("registo", O_RDONLY);
    if (fd == -1) {
        perror("Error Opening File");
        return -1;
    }
    int size = lseek(fd, 0, SEEK_END);
    close(fd);
    return size;
}


int main(int argc, char const *argv[])
{
    if (argc < 4) {
        perror("Error: Not enough arguments");
        return -1;
    }
    
    if (strcmp(argv[1], "-i") == 0) {
        insertPerson(argv[2], argv[3]);
        int size = sizeofFile();
        int pos = size / sizeof(Person);
        printf("Person inserted at position %d\n", pos);
    }
    if (strcmp(argv[1], "-r") == 0) {
        readPerson();
    }
    if (strcmp(argv[1], "-u") == 0) {
        updatePersonAge(argv[2], argv[3]);
    }
    if (strcmp(argv[1], "-o") == 0) {
        int size = sizeofFile();
        int total = size / sizeof(Person);
        int result = updateByPosition(total, argv[2], argv[3]);
        if (result == 0) {
            printf("Person updated\n");
        }
    }
    return 0;
}