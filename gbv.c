#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "gbv.h"
#include "util.h"

#define MAX_NAME 256
// Variável estática para armazenar o nome do arquivo da biblioteca
static char current_archive_name[MAX_NAME] = {0};



typedef struct{
    long offset; // posição do começo da area de diretorio
    long n_docs; // número de documentos no arquivo
} superblock;


/*
int gbv_create(const char *filename){
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Erro ao criar o arquivo");
        return 0;
    }

    // Inicializa o cabeçalho do arquivo 
    superblock sb;
    sb.offset = sizeof(superblock);
    sb.n_docs = 0;
    
    // Escreve o cabeçalho no arquivo
    if (fwrite(&sb, sizeof(superblock), 1, file) != 1) {
    perror("Erro ao escrever o cabeçalho do arquivo");
    fclose(file);
    return 0;
    }
    fclose(file);
    return 1;
} */


// gbv.c

int gbv_create(const char *filename){
    FILE *file = fopen(filename, "wb");
    if (!file) {
        perror("Erro ao criar o arquivo");
        return 0;
    }

    // Inicializa o cabeçalho do arquivo 
    superblock sb;
    // O diretório (vazio) começa logo após o superbloco
    // Esta é a posição inicial para a ÁREA DE DADOS
    sb.offset = sizeof(superblock); 
    sb.n_docs = 0;
    
    // Escreve o cabeçalho no arquivo
    if (fwrite(&sb, sizeof(superblock), 1, file) != 1) {
        perror("Erro ao escrever o cabeçalho do arquivo");
        fclose(file);
        return 0;
    }
    fclose(file);
    return 1;
}




void gbv_close(Library *lib) {
    if (lib->docs) {
        free(lib->docs);
        lib->docs = NULL;
    }
    lib->count = 0;
}



int gbv_open(Library *lib, const char *filename){
    FILE *file = fopen(filename, "rb");
    // Se o arquivo não puder ser aberto em 'rb'
    if (!file) {
        // Tenta criar a biblioteca
        if (!gbv_create(filename)) {
            printf("Falha ao criar a biblioteca.\n");
            return 1;
        }
        
        // Se a criação foi bem-sucedida, reabre o arquivo em modo 'rb'
        file = fopen(filename, "rb");
        if (!file) {
            perror("Erro fatal ao reabrir a biblioteca recém-criada");
            return 1;
        }
    }

    //salva o nome do arquivo da biblioteca para usar em funções futuras
    strncpy(current_archive_name, filename, MAX_NAME - 1);
    current_archive_name[MAX_NAME - 1] = '\0';

    // Lê o superbloco
    superblock sb;
    if (fread(&sb, sizeof(superblock), 1, file) != 1) {
        perror("Erro ao ler o superbloco");
        fclose(file);
        return 1;
    }

    // Aloca memória para o diretório
    lib->docs = NULL;
    lib->count = sb.n_docs;
    
    if (sb.n_docs > 0) {
        lib->docs = malloc(sizeof(Document) * sb.n_docs);
        if (!lib->docs) {
            perror("Erro ao alocar memória para o diretório");
            fclose(file);
            return 1;
        }
        
        // Posiciona para ler o diretório
        if (fseek(file, sb.offset, SEEK_SET) != 0) { 
            perror("Erro ao posicionar para ler o diretório");
            free(lib->docs);
            fclose(file);
            return 1;
        }
        
        // Lê o diretório
        if (fread(lib->docs, sizeof(Document), sb.n_docs, file) != (size_t)sb.n_docs) {
            perror("Erro ao ler o diretório");
            free(lib->docs);
            fclose(file);
            return 1; 
        }
    }
    
    fclose(file);
    return 0; 
}


int gbv_add(Library *lib, const char *archive, const char *docname) {
    FILE *doc_file = fopen(docname, "rb");
    if (!doc_file) {
        perror("Erro ao abrir o documento");
        return 0;
    }
    
    struct stat st;
    if (stat(docname, &st) != 0) {
        perror("Erro ao obter estatísticas do documento");
        fclose(doc_file);
        return 0;
    }

    int found_index = -1;
    for (int i = 0; i < lib->count; i++) {
        if (strcmp(lib->docs[i].name, docname) == 0) {
            found_index = i;
            break;
        }
    }

    //Prepara a nova entrada
    int target_index;
    int is_new = (found_index == -1);

    if (is_new) {
        // Se for novo, aloca mais espaço no diretório
        Document *tmp = realloc(lib->docs, sizeof(Document) * (lib->count + 1));
        if (!tmp) {
            perror("Erro ao realocar memória para o diretório");
            fclose(doc_file);
            return 0;
        }
        lib->docs = tmp;
        target_index = lib->count; 
        lib->count++; 
    } else {
        // Se for substituição, usa o índice encontrado
        target_index = found_index;
        printf("Documento '%s' substituído.\n", docname); 
    }
    

// Abre o arquivo da biblioteca no modo de adição
    FILE *gbv_file = fopen(archive, "rb+");
    if (!gbv_file) {
        perror("Erro ao abrir a biblioteca para escrita de dados");
        fclose(doc_file);
        // Desfaz a alocação se for um documento novo que falhou
        if (is_new) lib->count--; 
        return 0;
    }

    // A área de dados está no meio do arquivo.
    // Para adicionar, devemos ignorar o diretório existente no final 
    // e posicionar o ponteiro no final da ÁREA DE DADOS.

    // 1. Encontrar o fim da ÁREA DE DADOS
    // A área de dados termina onde o diretório começa (sb.offset)
    superblock temp_sb;
    fseek(gbv_file, 0, SEEK_SET);
    fread(&temp_sb, sizeof(superblock), 1, gbv_file);
    
    // Posiciona o ponteiro no FINAL DA ÁREA DE DADOS (que é onde o diretório começa)
    fseek(gbv_file, temp_sb.offset, SEEK_SET);
    long data_offset = ftell(gbv_file); // O novo offset do documento

    // 2. Copia os dados
    // ... (Copia os dados do documento para o arquivo container a partir de 'data_offset') ...
    char buffer[BUFFER_SIZE];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, doc_file)) > 0) {
        if (fwrite(buffer, 1, bytes_read, gbv_file) != bytes_read) {
            perror("Erro ao escrever dados no container");
            fclose(gbv_file);
            fclose(doc_file);
            return 0;
        }
    }
    
    // 3. Atualiza os metadados
    // ... (Atualiza lib->docs[target_index] com novo nome, size, date, offset) ...
    strncpy(lib->docs[target_index].name, docname, MAX_NAME - 1);
    lib->docs[target_index].name[MAX_NAME - 1] = '\0';
    lib->docs[target_index].size = st.st_size;
    lib->docs[target_index].date = time(NULL);
    lib->docs[target_index].offset = data_offset; // O novo offset de dados
    fclose(doc_file);
    printf("%ld\n", lib->docs[target_index].offset);

    // 4. Reescreve o superbloco E o diretório

    // O novo diretório começa imediatamente após o último byte do novo documento.
    long directory_offset = ftell(gbv_file); 

    // Reescreve o diretório completo na nova posição
    fseek(gbv_file, directory_offset, SEEK_SET);
    if (fwrite(lib->docs, sizeof(Document), lib->count, gbv_file) != (size_t)lib->count) {
        perror("Erro ao escrever o diretório");
        fclose(gbv_file);
        return 0;
    }
    
    // Atualiza o superbloco com a nova posição do diretório
    superblock sb;
    sb.n_docs = lib->count;
    sb.offset = directory_offset; // O NOVO offset do diretório

    fseek(gbv_file, 0, SEEK_SET);
    if (fwrite(&sb, sizeof(superblock), 1, gbv_file) != 1) {
        perror("Erro ao reescrever o superbloco");
        fclose(gbv_file);
        return 0;
    }

    printf("%ld\n", sb.offset);
    fclose(gbv_file);
    printf("Documento '%s' adicionado/substituído.\n", docname);
    return 1;
}

    /*
    // Abre o arquivo da biblioteca no modo de adição
    FILE *gbv_file = fopen(archive, "rb+");
    if (!gbv_file) {
        perror("Erro ao abrir a biblioteca para escrita de dados");
        fclose(doc_file);
        // Desfaz a alocação se for um documento novo que falhou
        if (is_new) lib->count--; 
        return 0;
    }

    // Posiciona o ponteiro no FINAL DO ARQUIVO para adicionar o documento
    fseek(gbv_file, 0, SEEK_END);
    long data_offset = ftell(gbv_file);

    // Copia os dados do documento para o final do arquivo container 
    char buffer[BUFFER_SIZE];
    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, doc_file)) > 0) {
        if (fwrite(buffer, 1, bytes_read, gbv_file) != bytes_read) {
            perror("Erro ao escrever dados no container");
            fclose(gbv_file);
            fclose(doc_file);
            return 0;
        }
    }
    
    // Atualiza os metadados do documento 
    strncpy(lib->docs[target_index].name, docname, MAX_NAME - 1);
    lib->docs[target_index].name[MAX_NAME - 1] = '\0';
    lib->docs[target_index].size = st.st_size;
    lib->docs[target_index].date = time(NULL);
    lib->docs[target_index].offset = data_offset; // O novo offset
    fclose(doc_file);
    printf("%ld\n", lib->docs[target_index].offset);
    //Reescreve o superbloco
    superblock sb;
    sb.n_docs = lib->count;
    sb.offset = sizeof(superblock); 

    fseek(gbv_file, 0, SEEK_SET);
    fwrite(&sb, sizeof(superblock), 1, gbv_file);


    printf("%ld\n", sb.offset);
    //Reescreve o diretório completo
    fseek(gbv_file, sb.offset, SEEK_SET);
    fwrite(lib->docs, sizeof(Document), lib->count, gbv_file);

    
    fclose(gbv_file);
    printf("Documento '%s' adicionado/substituído.\n", docname);
    return 1;
}
*/

int gbv_list(const Library *lib){
    if (lib->count == 0) {
        printf("A biblioteca está vazia.\n");
        return 1;
    }

    printf("Documentos na biblioteca:\n");
    printf("%-30s %-10s %-20s\n", "Nome", "Tamanho", "Data de Inserção");
    printf("-----------------------------------------------------------\n");

    char date_buffer[20];
    for (int i = 0; i < lib->count; i++) {
        format_date(lib->docs[i].date, date_buffer, sizeof(date_buffer));
        printf("%-30s %-10ld %-20s\n", lib->docs[i].name, lib->docs[i].size, date_buffer);
    }

    return 1;
}


int gbv_view(const Library *lib, const char *docname) {
    // Localiza o documento na biblioteca
    int found_index = -1;
    for (int i = 0; i < lib->count; i++) {
        if (strcmp(lib->docs[i].name, docname) == 0) {
            found_index = i;
            break;
        }
    }

    if (found_index == -1) {
        printf("Documento '%s' não encontrado na biblioteca.\n", docname);
        return 0;
    }

    const Document *doc = &lib->docs[found_index];

    // Abre o arquivo container
    FILE *archive_file = fopen(current_archive_name, "rb");
    if (!archive_file) {
        perror("Erro ao abrir a biblioteca para visualização");
        return 0;
    }

    // Configuração inicial para navegação
    long current_offset = doc->offset;
    long doc_end_offset = doc->offset + doc->size;
    char buffer[BUFFER_SIZE];
    char command = ' ';

    printf("\n--- Visualização do Documento '%s' ---\n", docname);

    do {
        fseek(archive_file, current_offset, SEEK_SET);

        // Calcula quantos bytes ler
        long bytes_remaining = doc_end_offset - current_offset;
        size_t bytes_to_read = (bytes_remaining > BUFFER_SIZE) ? BUFFER_SIZE : bytes_remaining;
        size_t bytes_read = fread(buffer, 1, bytes_to_read, archive_file);

        if (bytes_read > 0) {
            fwrite(buffer, 1, bytes_read, stdout);
        } else {
            current_offset = doc_end_offset; 
        }

        printf("\n\n-- Bloco de %ld a %ld -- [n] Próximo, [p] Anterior, [q] Sair: ", 
               current_offset - doc->offset, 
               (current_offset - doc->offset) + bytes_read);

        // Limpa o buffer de entrada e espera pelo comando
        fflush(stdout); 
        if (scanf(" %c", &command) != 1) { 
            command = 'q'; 
        }

        // Lógica de navegação
        if (command == 'n') {
            // Avança para o próximo bloco, se houver bytes restantes
            if (current_offset + bytes_read < doc_end_offset) {
                current_offset += bytes_read;
            } else {
                printf("\n(Já está no final do documento.)\n");
            }
        } else if (command == 'p') {
            // Volta um bloco completo
            long prev_offset = current_offset - BUFFER_SIZE;
            
            if (current_offset > doc->offset) {
                // Se a volta cair antes do início, volta exatamente para o início
                if (prev_offset < doc->offset) {
                    current_offset = doc->offset; 
                } else {
                    current_offset = prev_offset; 
                }
            } else {
                printf("\n(Já está no início do documento.)\n");
            }
        } else if (command != 'q') {
            printf("\nComando inválido. Use 'n', 'p' ou 'q'.\n");
        }

    } while (command != 'q');

    fclose(archive_file);
    return 1;
}


int gbv_remove(Library *lib, const char *docname) {
    int found_index = -1;
    for (int i = 0; i < lib->count; i++) {
        if (strcmp(lib->docs[i].name, docname) == 0) {
            found_index = i;
            break;
        }
    }

    if (found_index == -1) {
        printf("Documento '%s' não encontrado na biblioteca.\n", docname);
        return 0;
    }

    // Remove o documento do array mantendo a ordem de inserção
    for (int i = found_index; i < lib->count - 1; i++) {
        lib->docs[i] = lib->docs[i + 1];
    }
    lib->count--;

    if (lib->count > 0) {
        Document *tmp = realloc(lib->docs, sizeof(Document) * lib->count);
        if (!tmp) {
            perror("Erro ao realocar memória para o diretório");
            return 0;
        }
        lib->docs = tmp;
    } else {
        free(lib->docs);
        lib->docs = NULL;
    }

FILE *gbv_file = fopen(current_archive_name, "rb+"); 
    if (!gbv_file) {
        perror("Erro ao abrir a biblioteca para remoção (rb+)");
        return 0;
    }

    // 1. LER o offset atual do diretório antes de reescrever
    superblock sb_read;
    fseek(gbv_file, 0, SEEK_SET);
    if (fread(&sb_read, sizeof(superblock), 1, gbv_file) != 1) {
        perror("Erro ao ler o superbloco para remoção");
        fclose(gbv_file);
        return 0;
    }
    
    // 2. Atualiza e reescreve o superbloco com a nova contagem (lib->count)
    superblock sb;
    sb.n_docs = lib->count;
    // O offset do diretório é mantido (é o que estava no superbloco antes)
    sb.offset = sb_read.offset; 
    
    fseek(gbv_file, 0, SEEK_SET);
    if (fwrite(&sb, sizeof(superblock), 1, gbv_file) != 1) {
        perror("Erro ao reescrever o superbloco");
        fclose(gbv_file);
        return 0;
    }

    // 3. Reescreve o diretório na POSIÇÃO ANTERIOR
    fseek(gbv_file, sb.offset, SEEK_SET);
    if (fwrite(lib->docs, sizeof(Document), lib->count, gbv_file) != (size_t)lib->count) {
        perror("Erro ao escrever o diretório");
        fclose(gbv_file);
        return 0;
    }
    
    // 4. Se o número de documentos diminuiu, o arquivo agora tem "lixo" no final 
    // (o espaço antigo do diretório). Você pode opcionalmente truncar o arquivo aqui.
    // truncate(current_archive_name, sb.offset + (sizeof(Document) * lib->count));

    fclose(gbv_file);

    printf("Documento '%s' removido da biblioteca.\n", docname);
    return 1;
}


    /*
    FILE *gbv_file = fopen(current_archive_name, "rb+"); 
    if (!gbv_file) {
        perror("Erro ao abrir a biblioteca para remoção (rb+)");
        return 0;
    }

    // Atualiza e reescreve o superbloco com a nova contagem (lib->count)
    superblock sb;
    sb.n_docs = lib->count;
    sb.offset = sizeof(superblock);
    
    fseek(gbv_file, 0, SEEK_SET);
    if (fwrite(&sb, sizeof(superblock), 1, gbv_file) != 1) {
        perror("Erro ao reescrever o superbloco");
        fclose(gbv_file);
        return 0;
    }

    // Reescreve o diretório
    fseek(gbv_file, sb.offset, SEEK_SET);
    if (fwrite(lib->docs, sizeof(Document), lib->count, gbv_file) != (size_t)lib->count) {
        perror("Erro ao escrever o diretório");
        fclose(gbv_file);
        return 0;
    }
    
    fclose(gbv_file);

    printf("Documento '%s' removido da biblioteca.\n", docname);
    return 1;
}
*/


int gbv_order(Library *lib, const char *archive, const char *criteria){
    return 1;
}

