#include "Core.h"

static object * root = NULL;
static bool updateSituation;

object * createObject(Sint32 x, Sint32 y) {
    object * toReturn = (object *) malloc(sizeof(object));  // Alocando a memória do novo objeto.
    toReturn->childrenCount = 0;                            // O objeto não tem nenhum filial no momento de sua criação.
    toReturn->children = malloc(0);                         // Preparando o vetor children para poder ser realocado no futuro.
    toReturn->localX = x;                                   // Setando a posição x do novo objeto.
    toReturn->localY = y;                                   // Setando a posição y do novo objeto.
    toReturn->parent = NULL;                                // Deixando o parente como NULL para poder realizar a parentagem com o root.
    parentObject(root, toReturn, false);                    // Colocando a root como parente.
    toReturn->isUpdated = !updateSituation;                 // Setando a situação de update.
    
    return toReturn;                                        // Retornando o objeto.
}

void destroyObject(object * targetObject) {
    unparentObject(targetObject, false);                                                // Desassociando sprite ao parente.

    while (targetObject->childrenCount > 0) destroyObject(targetObject->children[0]);   // Destruindo todos os objetos filiais a este.

    free(targetObject->children);                                                       // Liberando a memória do vetor de filiais.

    free(targetObject);                                                                 // Liberando a memória do objeto.
    targetObject = NULL;                                                                // Setando o ponteiro para null.
}   

void parentObject(object * parent, object * child, bool keepGlobalPosition) {
    if (child->parent != NULL) unparentObject(child, keepGlobalPosition ? true : false);                                       // Caso o filial já tenha um parente, realizar a deparentagem.
    
    if (keepGlobalPosition) {
        child->localX = child->globalX - parent->globalX;                                   // Atualizando as posições locais para manter a posição global.
        child->localY = child->globalY - parent->globalY;                   
    }

    child->globalX = child->localX + parent->globalX;                                       // Atualizando a posição global.
    child->globalY = child->localY + parent->globalY;   

    child->parent = (void*) parent;                                                         // Associando o parente e o filial.
                
    parent->children = realloc(parent->children, sizeof(object) * ++parent->childrenCount); // Realocando o vetor de filiais para encaixar o novo.
    child->parentPos = parent->childrenCount - 1;                                           // Salvando a posição do filial no vetor.
    parent->children[parent->childrenCount - 1] = (void*) child;                            // Colocando o ponteiro do objeto filial no vetor de filiais do parente.
}

void unparentObject(object * child, bool keepGlobalPosition) {
    if (child->parent == NULL) return;

    object * parent = (object*) child->parent;                                              // Pegando o objeto parente.

    if (keepGlobalPosition) {
        child->localX = child->globalX;
        child->localY = child->globalY;
    } else {
        child->globalX = child->localX;
        child->globalY = child->localY;
    }

    for (int i = child->parentPos; i < parent->childrenCount - 1; i++) {                    // Para todo filial do parente após o objeto orfão.
        parent->children[i] = parent->children[i + 1];                                      // Passar todos os objetos uma casa pra trás.
        ((object*) parent->children[i])->parentPos--;                                       // Ajustar o contador de posição dos outros filiados.
    }

    parent->children[parent->childrenCount - 1] = NULL;                                     // Setando o último valor do vetor de filiais pra NULL.
    parent->children = realloc(parent->children, sizeof(object) * --parent->childrenCount); // E removendo esse último valor do vetor.

    child->parent = NULL;                                                                  // E finalmente colocando NULL como ponteiro para o parente.
    child->parentPos = 0;
}

object * getRoot() {
    return root;
}

void coreUpdate() {
    updateObjectPosition(root);
    updateSituation = !updateSituation;
}

void updateObjectPosition(object * targetObject) {    

    if(targetObject->isUpdated == updateSituation) return;

    if (targetObject->parent != NULL) {
        targetObject->globalX = targetObject->localX + ((object*)targetObject->parent)->globalX;    // Atualizando as posições globais X e Y do objeto.
        targetObject->globalY = targetObject->localY + ((object*)targetObject->parent)->globalY;   
    }

    targetObject->isUpdated = updateSituation;                                                      // Colocando o sprite como atualizado.

    for (int i = 0; i < targetObject->childrenCount; i++) {
        updateObjectPosition((object *)targetObject->children[i]);                                  // Atualizando a posição de todas as filiais.
    }
}

void coreInit() {
    root = (object *) malloc(sizeof(object));          // Inicializando o objeto raíz e setando seus parâmetros.
    root->parent = NULL;
    root->parentPos = 0;
    root->localX = 0;
    root->globalX = 0;
    root->localY = 0;
    root->globalY = 0;
    root->childrenCount = 0;
    root->children = malloc(0);
    root->isUpdated = true;

    updateSituation = false;                // Deixando a situação de atualização falsa.
}

void coreShut() {
    destroyObject(root);
    root = NULL;
}