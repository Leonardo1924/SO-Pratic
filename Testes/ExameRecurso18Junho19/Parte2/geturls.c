#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_URL_LENGTH 100

void geturls(const char *url) {
    // Código para baixar a página e extrair as URLs
    // ...

    // Exemplo de URLs encontradas
    const char *urls[] = {
        "https://www.example.com/page1",
        "https://www.example.com/page2",
        "https://www.example.com/page3"
    };
    int num_urls = sizeof(urls) / sizeof(urls[0]);

    // Imprime as URLs na saída padrão
    for (int i = 0; i < num_urls; i++) {
        // Verifica o tamanho da URL antes de imprimir
        if (strlen(urls[i]) < MAX_URL_LENGTH) {
            printf("%s\n", urls[i]);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s <url>\n", argv[0]);
        return 1;
    }

    const char *url = argv[1];
    geturls(url);

    return 0;
}