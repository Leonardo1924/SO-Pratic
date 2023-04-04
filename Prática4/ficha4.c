#include <unistd.h> /* chamadas ao sistema: defs e decls essenciais */
#include <sys/wait.h> /* chamadas wait*() e macros relacionadas */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>

#define rows 4
#define cols 6

int exercise1(int value){
    int fd[2];
    pid_t pid;

    if(pipe(fd) == -1){
        perror("pipe");
        exit(1);
    }

    pid = fork();

    if(pid == -1){
        perror("fork");
        exit(1);
    }

    if (pid == 0){
        int num;

        close(fd[1]);

        while(read(fd[0], &num, sizeof(int)) > 0){
            printf("Filho: Recebi o número %d\n", num);
        }

        close(fd[0]);
    }
    else{
        int num = value;

        close(fd[0]);

        write(fd[1], &num, sizeof(int));

        printf("Pai: Enviei o número %d\n", num);

        close(fd[1]);
    }

    return 0;

}

int exercise2(int value){
    int fd[2];
    pid_t pid;

    if(pipe(fd) == -1){
        perror("pipe");
        exit(1);
    }

    pid = fork();

    if(pid == -1){
        perror("fork");
        exit(1);
    }

    if (pid == 0){
        int num;

        close(fd[1]);

        while(read(fd[0], &num, sizeof(int)) > 0){
            printf("Filho: Recebi o número %d\n", num);
        }

        close(fd[0]);
    }
    else{
        int num = value;

        close(fd[0]);

        sleep(5);

        write(fd[1], &num, sizeof(int));

        printf("Pai: Enviei o número %d\n", num);

        close(fd[1]);
    }

    return 0;

}

int exercise3(int value){
    int fd[2];
    pid_t pid;

    if(pipe(fd) == -1){
        perror("pipe");
        exit(1);
    }

    pid = fork();

    if(pid == -1){
        perror("fork");
        exit(1);
    }

    if (pid == 0) { // processo filho
        int num = value;

        close(fd[0]); // fecha o descritor de leitura

        write(fd[1], &num, sizeof(num)); // escreve um inteiro no descritor de escrita

        printf("Filho: enviei o número %d\n", num);

        close(fd[1]); // fecha o descritor de escrita
    } else { // processo pai
        int num;

        close(fd[1]); // fecha o descritor de escrita

        read(fd[0], &num, sizeof(num)); // lê um inteiro do descritor de leitura

        printf("Pai: recebi o número %d\n", num);

        close(fd[0]); // fecha o descritor de leitura
    }

    return 0;

}

int exercise4(){
    int fd[2]; // array para os descritores de arquivo do pipe
    pid_t pid;

    if (pipe(fd) == -1) { // cria o pipe e verifica se houve erro
        perror("pipe");
        exit(1);
    }

    pid = fork(); // cria o processo filho

    if (pid == -1) { // verifica se houve erro ao criar o processo filho
        perror("fork");
        exit(1);
    }

    if (pid == 0) { // processo filho
        int nums[] = { 42, 84, 126 };
        int num_count = sizeof(nums) / sizeof(int);

        close(fd[0]); // fecha o descritor de leitura

        for (int i = 0; i < num_count; i++) {
            write(fd[1], &nums[i], sizeof(int)); // escreve um inteiro no descritor de escrita
            printf("Filho: enviei o número %d\n", nums[i]);
        }

        close(fd[1]); // fecha o descritor de escrita
    } else { // processo pai
        int num;

        close(fd[1]); // fecha o descritor de escrita

        sleep(5); // atraso de 5 segundos

        while (read(fd[0], &num, sizeof(int)) > 0) { // lê inteiros do descritor de leitura enquanto houver dados
            printf("Pai: recebi o número %d\n", num);
        }

        close(fd[0]); // fecha o descritor de leitura
    }

    return 0;
}

int exercise5(int argc, char *argv[]){
    int fd[2]; // array para os descritores de arquivo do pipe
    pid_t pid;

    if (pipe(fd) == -1) { // cria o pipe e verifica se houve erro
        perror("pipe");
        exit(1);
    }

    pid = fork(); // cria o processo filho

    if (pid == -1) { // verifica se houve erro ao criar o processo filho
        perror("fork");
        exit(1);
    }

    if (pid == 0) { // processo filho
        int num_count = argc - 1;

        close(fd[0]); // fecha o descritor de leitura

        for (int i = 1; i < argc; i++) {
            int num = atoi(argv[i]);
            write(fd[1], &num, sizeof(int)); // escreve um inteiro no descritor de escrita
            printf("Filho: enviei o número %d\n", num);
        }

        close(fd[1]); // fecha o descritor de escrita
    } else { // processo pai
        int num;

        close(fd[1]); // fecha o descritor de escrita

        sleep(5); // atraso de 5 segundos

        while (read(fd[0], &num, sizeof(int)) == sizeof(int)) { // lê inteiros do descritor de leitura enquanto houver dados
            printf("Pai: recebi o número %d\n", num);
        }

        close(fd[0]); // fecha o descritor de leitura
    }

    return 0;
}

void search_number(int matrix[rows][cols], int number, int *results, int start, int end){
    int count = 0;
    for(int i = start; i < end; i++){
        for(int j = 0; j < cols; j++){
            if(matrix[i][j] == number){
                count++;
            }
        }
    }
    *results = count;
}

int exercise6(){
    int number = 5;
    int results[rows];
    int fd[2];
    pid_t pid;

    int matrix[rows][cols] = {
        { 1, 2, 3, 4, 5, 6 },
        { 5, 5, 5, 5, 5, 5 },
        { 7, 8, 9, 10, 11, 12},
        { 5, 5, 5, 5, 5, 5 }
    };

    if (pipe(fd) == -1) { // cria o pipe e verifica se houve erro
        perror("pipe");
        exit(1);
    }

    int segment_size = rows / 2;
    int start = 0;

    for(int i = 0; i < 2; i++){
        pid = fork(); // cria o processo filho

        if (pid == -1) { // verifica se houve erro ao criar o processo filho
            perror("fork");
            exit(1);
        }

        if (pid == 0) { // processo filho
            int end = start + segment_size;
            if (i == 1) {
                end = rows;
            }

            close(fd[0]); // fecha o descritor de leitura

            int count;
            search_number(matrix, number, &count, start, end);
            write(fd[1], &count, sizeof(int)); // escreve um inteiro no descritor de escrita

            close(fd[1]); // fecha o descritor de escrita
            exit(0);
        }
        else { // processo pai
            start += segment_size;
        }
    }

    close(fd[1]); // fecha o descritor de escrita

    int total = 0;
    int count;

    while (read(fd[0], &count, sizeof(int)) == sizeof(int)) { // lê inteiros do descritor de leitura enquanto houver dados
        total += count;
    }

    close(fd[0]); // fecha o descritor de leitura

    printf("O número %d aparece %d vezes na matriz.\n", number, total);

    return 0;
}


int main(int argc, char *argv[]){
    char str1[] = "exe1";
    if(strcmp(argv[1], str1) == 0){
        exercise1(atoi(argv[2]));
    }
    char str2[] = "exe2";
    if(strcmp(argv[1], str2) == 0){
        exercise2(atoi(argv[2]));
    }
    char str3[] = "exe3";
    if(strcmp(argv[1], str3) == 0){
        exercise3(atoi(argv[2]));
    }
    char str4[] = "exe4";
    if(strcmp(argv[1], str4) == 0){
        exercise4();
    }
    char str5[] = "exe5";
    if(strcmp(argv[1], str5) == 0){
        exercise5(argc, argv);
    }
    char str6[] = "exe6";
    if(strcmp(argv[1], str6) == 0){
        exercise6();
    }
    return 0;
}