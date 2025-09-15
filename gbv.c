#include "gbv.h"
#include "util.h"
#include <stdio.h>

typedef struct{
    long offset; // posição do começo da area de diretorio
    long n_docs; // número de documentos no arquivo
} superblock;

int gbv_create(const char *filename){
    
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Erro ao criar o arquivo");
        return 0;
    }

    // Inicializa o cabeçalho do arquivo (pode ser vazio ou conter metadados iniciais)
    superblock sb;
    sb.offset = sizeof(superblock);
    sb.n_docs = 0;
    
    // Aqui, apenas escrevemos um cabeçalho simples com contagem zero
    if (fwrite(&sb, sizeof(superblock), 1, file) != 1) {
    perror("Erro ao escrever o cabeçalho do arquivo");
    fclose(file);
    return 0;
    }
    fclose(file);
    return 1; // sucesso
}

// Abre o arquivo e carrega a biblioteca (diretório em memória)
int gbv_open(Library *lib, const char *filename){
    FILE *file = fopen(filename, "rb+");
    if (!file) {
        perror("Erro ao abrir o arquivo");
        return 0;
    }

    superblock sb;
    if (fread(&sb, sizeof(superblock), 1, file) != 1) {
        perror("Erro ao ler o cabeçalho do arquivo");
        fclose(file);
        return 0;
    }

    // Carrega os documentos do arquivo para a estrutura Library
    lib->count = sb.n_docs;
    lib->docs = malloc(sizeof(Document) * lib->count);
    if (!lib->docs && lib->count > 0) {
        perror("Erro ao alocar memória para documentos");
        fclose(file);
        return 0;
    }

    if (fseek(file, sb.offset, SEEK_SET) != 0) {
        perror("Erro ao posicionar para leitura dos documentos");
        free(lib->docs);
        fclose(file);
        return 0;
    }

    if (fread(lib->docs, sizeof(Document), lib->count, file) != (size_t)lib->count) {
        perror("Erro ao ler documentos do arquivo");
        free(lib->docs);
        fclose(file);
        return 0;
    }

    fclose(file);
    return 1; // sucesso

}

// ...existing code...
int gbv_open(Library *lib, const char *filename){
    // Libera memória se já estiver aberta
    if (lib->docs != NULL && lib->count > 0) {
        free(lib->docs);
        lib->docs = NULL;
        lib->count = 0;
    }

    FILE *file = fopen(filename, "rb+");
    if (!file) {
        perror("Erro ao abrir o arquivo");
        return 0;
    }

    superblock sb;
    if (fread(&sb, sizeof(superblock), 1, file) != 1) {
        perror("Erro ao ler o cabeçalho do arquivo");
        fclose(file);
        return 0;
    }

    lib->count = sb.n_docs;
    lib->docs = malloc(sizeof(Document) * lib->count);
    if (!lib->docs && lib->count > 0) {
        perror("Erro ao alocar memória para documentos");
        fclose(file);
        return 0;
    }

    if (fseek(file, sb.offset, SEEK_SET) != 0) {
        perror("Erro ao posicionar para leitura dos documentos");
        free(lib->docs);
        fclose(file);
        return 0;
    }

    if (fread(lib->docs, sizeof(Document), lib->count, file) != (size_t)lib->count) {
        perror("Erro ao ler documentos do arquivo");
        free(lib->docs);
        fclose(file);
        return 0;
    }

    fclose(file);
    return 1; // sucesso
}
// ...existing code...


int gbv_add(Library *lib, const char *archive, const char *docname);
int gbv_remove(Library *lib, const char *docname);
int gbv_list(const Library *lib);
int gbv_view(const Library *lib, const char *docname);
int gbv_order(Library *lib, const char *archive, const char *criteria);
