#include "FileLib.h"

static FILE * currentFile = NULL;   // Arquivo carregado
static int fileStatus;              // Estado do arquivo
static char ** loadDump = NULL;     // Pilha de memória.
static char ** nameDump = NULL;     // Pilha de nomes.
static int dumpSize;                // Tamanho das pilhas.

int loadFile(char * path) {
    int ret, cSize;    // Variável q ira retornar o número de dados dentro do save
    int test;
    char * cName = NULL, * cData = NULL;
    if (fileStatus != FILE_STATUS_INACTIVE) {  // Caso o arquivo já estiver aberto, retorna um erro.
        printf("Já existe um arquivo aberto.\n");
        return -1;
    }

    // -- Colocando o sufixo no caminho.
    char * newPath = malloc(sizeof(char) * (strlen(path) + strlen(FILE_SUFIX) + 2));
    strcpy(newPath, path);
    strcat(newPath, FILE_SUFIX);
    // --

    currentFile = fopen(newPath, "r");  // Abrindo o arquivo para leitura.
    free(newPath);  // Liberando a memória da string usada pro caminho com o caminho.

    if (currentFile == NULL) {  // Caso o arquivo não tenha sido aberto com sucesso, retornar um erro.
        return -2;
    }

    fscanf(currentFile, "%d\n", &ret);  // Lendo a quantidade de dados no arquivo.

    fileStatus = FILE_STATUS_READ;  // Setando o status do arquivo pra leitura.

    loadDump = malloc(sizeof(char*) * ret); // Alocando o dump de memória.
    nameDump = malloc(sizeof(char*) * ret); // Alocando o dump de nomes.
    dumpSize = 0;   // Tamanho de dump é 0, irá aumentar depois.

    for(int i = 0; i < ret; i++) {  // Lendo os dados e importando para o dump de memória.
        fscanf(currentFile,"%d:", &cSize);
        cName = malloc(sizeof(char) * cSize);
        fscanf(currentFile, "%[^:]%*c%d:", cName, &cSize);
        cData = malloc(sizeof(char) * cSize);
        fscanf(currentFile, "%[^:]%*c\n", cData);

        addToDump(cName, cData);

        free(cName);    // Liberando as strings auxiliares que utilizamos.
        cName = NULL;
        free(cData);
        cData = NULL;
    }

    return ret; // Retornando a quantidade de dados no arquivo. 
}

bool saveFile(char * path) {
    if (fileStatus != FILE_STATUS_INACTIVE) {  // Checando se já há um arquivo aberto, e nesse caso retornando um erro.
        printf("Já existe um arquivo aberto.\n");
        return false;
    }

    // -- Colocando o sufixo no caminho.
    char * newPath = malloc(sizeof(char) * (strlen(path) + strlen(FILE_SUFIX) - 1));
    strcpy(newPath, path);
    strcat(newPath, FILE_SUFIX);
    //--

    currentFile = fopen(newPath, "w");  // Abrindo o arquivo em modo de escrita.
    free(newPath);                      // Liberando a memória da string usada pra achar o caminho.
    if (currentFile == NULL) {          // Caso algum erro tenha acontecido, retornar um erro.
        printf("Não foi possível abrir o arquivo.\n");
        return false;
    }

    loadDump = malloc(0);               // Alocando as pilhas de memória.
    nameDump = malloc(0);
    dumpSize = 0;

    fileStatus = FILE_STATUS_WRITE;     // Colocando o estado do arquivo para em modo de leitura.

    return true;                        // Retornando verdadeiro, pois a função foi concluida sem erros.
}

bool closeFile() {
    if (fileStatus == FILE_STATUS_INACTIVE) {   // Checando se há um arquivo aberto, e caso contrário, retornando erro.
        printf("Não há um arquivo aberto para ser fechado.\n");
        return false;
    }
    
    if (fileStatus == FILE_STATUS_WRITE) {      // Caso o arquivo esteja em modo de escrita:
        fprintf(currentFile, "%d\n", dumpSize); // Escrever o tamanho do arquivo, em dados.
        for (int i = 0; i < dumpSize; i++) {    // Escrever todos os dados no arquivo, e liberar suas memórias.
            fprintf(currentFile, "%d:%s:%d:%s:\n", strlen(nameDump[i]), nameDump[i], strlen(loadDump[i]), loadDump[i]);
            free(loadDump[i]);
            loadDump[i] = NULL;
            free(nameDump[i]);
            nameDump[i] = NULL;
        }
        free(loadDump);                         // Liberar a pilha de memória.
        loadDump = NULL;
        free(nameDump);                         // Liberar a pilha de nomes
        nameDump = NULL;
        dumpSize = 0;
    }

    if (fileStatus == FILE_STATUS_READ) {       // Caso o arquivo esteja em modo de leitura:
        for (int i = 0; i < dumpSize; i++) {    // Liberar a memória de todos os dados.
            free(loadDump[i]);                  
            loadDump[i] = NULL;
            free(nameDump[i]);
            nameDump[i] = NULL;
        }
        free(loadDump);                         // Liberar a pilha de memória.
        free(nameDump);                         // Liberar a pilha de nomes.
        dumpSize = 0;
    }

    fclose(currentFile);                        // Fechar o arquivo.
    currentFile = NULL;
    fileStatus = FILE_STATUS_INACTIVE;          // Colocar o estado do arquivo para inativo.

    return true;                                // Retornar verdadeiro, pois a função foi finalizada sem erros.
}

bool addToDump(char * name, char * data) {
    int pos = 0;    

    for (int i = 0; i < dumpSize; i++) {                                // Procurar a posição dentro do dump para o novo nome, caso já exista um dado com o nome, retornar erro.
        if(strcmp(nameDump[i], name) == 0) {
            printf("Já existe um dado com nomeado \"%s\".\n", name);
            return false;
        }
        if(strcmp(nameDump[i], name) < 0) {
            pos = i;
            break;
        }
    }
    
    loadDump = realloc(loadDump, sizeof(char *) * ++dumpSize);          // Realocar a memória do dump para colocar o novo dado.
    nameDump = realloc(nameDump, sizeof(char *) * dumpSize);            // Realocar a memória do dump de nomes para colocar o novo nome.

    for (int i = dumpSize - 2; i >= pos; i--) {                         // Mudar de lugar os dados e nomes para colocar os novos nas posições corretas.
        loadDump[i + 1] = loadDump[i];
        nameDump[i + 1] = nameDump[i];
    }

    loadDump[pos] = malloc(sizeof(char) * strlen(data));                // Alocar memória para o dado dentro do dump.
    nameDump[pos] = malloc(sizeof(char) * strlen(name));                // Alocar memória para o nome dentro do dump.

    strcpy(loadDump[pos], data);                                        // Colocar o dado dentro do dump de memória.
    strcpy(nameDump[pos], name);                                        // Colocar o nome do dado dentro do dump de nomes.

    return true;                                                        // Função finalizada com sucesso, retorna verdadeiro.
}

bool writeData(char * name, char * data) {
    if (fileStatus != FILE_STATUS_WRITE) {      // Caso o arquivo não esteja aberto em modo de escrita, retornar um erro.
        if (fileStatus == FILE_STATUS_READ) printf("O arquivo está aberto em modo de leitura, feche o arquivo e abra-o em modo de escrita para poder escrever dados.\n");
        else if (fileStatus == FILE_STATUS_INACTIVE) printf("Não há um arquivo aberto.\n");
        return false;
    }

    if (strlen(name) > 20) {                    // Caso o tamanho do nome do dado esteja grande demais, retornar um erro.
        printf("Nome \"%s\" é muito grande, por favor, utilize nomes com 20 caracteres ou menos\n", name);
        return false;
    }

    if (strlen(data) > 20) {                    // Caso o tamanho do dado esteja grande demais, retorar um erro. 
        printf("dado \"%s\" é muito grande, por favor, utilize dados com 20 caracteres ou menos\n", data);
        return false;
    }
    
    if (!addToDump(name, data)) return false;   // No caso da função ter falhado, retornar um erro.
    
    return true;                                // Função finalizada com sucesso, retorna verdadeiro.
}

char * readData(char * name) {
    char * ret = NULL;

    if (fileStatus != FILE_STATUS_READ) {   // Caso a arquivo não esteja em modo de leitura, retornar um erro.
        if (fileStatus == FILE_STATUS_WRITE) printf("O arquivo está aberto em modo de escrita, feche o arquivo e abra-o em modo de leitura para poder ler dados.\n");
        else if (fileStatus == FILE_STATUS_INACTIVE) printf("Não há um arquivo aberto.\n");
        return NULL;
    }
    
    for (int i = 0; i < dumpSize; i++) {    // Procurar o dado com o nome colocado no argumento, retornando um erro caso nenhum nome tenha sido encontrado.
        if (strcmp(name, nameDump[i]) == 0) {
            ret = loadDump[i];
            break;
        }
        if (i == dumpSize - 1) {
            printf("Não foi encontrado nenhum dado de nome \"%s\".\n", name);
        }
    }
    
    return ret;                             // Retorna o ponteiro para o dado, ou NULL caso algum erro tenha acontecido.
}

bool fileInit() {
    fileStatus = FILE_STATUS_INACTIVE;  // Colocando o estado do arquivo como inativo.
    return true;
}

bool fileShut() {
    if (fileStatus != FILE_STATUS_INACTIVE) closeFile();    // Caso o arquivo não esteja fechado, fecha-lo.
    return true;
}