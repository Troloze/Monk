#include "ErrorLib.h"

static char * currentError = NULL;
static bool critError = false;

bool errorSet(char * error) {
    if (error == NULL) {    // Se a entrada do erro for NULL, retornar um erro.
        errorSet("errorSet: Argumento não pode ser NULL.");
        return false;
    }
    
    if (currentError != NULL) free(currentError);   // Se o erro atual não for nulo, libera-lo.
    
    currentError = malloc(sizeof(char) * strlen(error));    // Alocar espaço pra string do novo erro.
    if (currentError == NULL) { // Caso não tenha sido possível alocar a memória, retornar um erro crítico.
        errorSetCritical("errorSet: Não foi possivel declarar erro, Limite de memória atingido.\n");
        return false;
    }
    
    strcpy(currentError, error);    // Copiar a string pro erro.
    
    return true;    // Retornar verdadeiro.
}

void errorSetCritical(char * error) {
    printf(error);  // Printar erro crítico.
    critError = true;   // Ativar a flag de erro crítico.
}

char * errorGet() {
    return currentError;    // Retorna o erro atual.
}

bool errorUpdate() {
    if (critError) return false;    // Caso a flag de erro crítico esteja levantada, retornar falso.
    return true;    // Se não, verdadeiro.
}

bool errorInit() {
    return errorSet("errorGet: Nenhum erro registrado até o momento."); // Inicalizar o sistema com uma mensagem dizendo que não há erros registrados.
}

bool errorShut() {
    free(currentError); // Liberar a string de erro.
    currentError = NULL;
    return true;    // Retornar verdadeiro.
}