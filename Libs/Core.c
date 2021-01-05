#include "Core.h"

static object * root = NULL;
static bool updateSituation;

object * createObject(Sint32 x, Sint32 y, Uint8 mask, Uint8 type, void * item, char * inst) {
    char * nInst = NULL;
    object * toReturn = (object *) malloc(sizeof(object));  // Alocando a memória do novo objeto.
    if (toReturn == NULL) {
        errorSet("createObject: Não foi possível alocar objeto.");
        return NULL;
    }

    toReturn->mask = mask;
    toReturn->type = type;
    toReturn->childrenCount = 0;                            // O objeto não tem nenhum filial no momento de sua criação.
    toReturn->children = malloc(0);                         // Preparando o vetor children para poder ser realocado no futuro.
    if (toReturn->children = NULL) {
        errorSet("createObject: Não foi possível alocar vetor de filiais do objeto.");
        free(toReturn);
        return NULL;
    }
    if (inst != NULL) {
        nInst = malloc(sizeof(char) * strlen(inst));
        if (!nInst) {
            errorSet("createObject: Não foi possível alocar a string de inst");
            free(toReturn->children);
            free(toReturn);
            return NULL;
        }
        strcpy(nInst, inst);
        toReturn->instance = nInst;
    }      
    else toReturn->instance = NULL;                       

    toReturn->isInst = false;
    toReturn->start = NULL;
    toReturn->update = NULL;
    toReturn->end = NULL;
    toReturn->item = item;
    toReturn->localX = x;                                   // Setando a posição x do novo objeto.
    toReturn->localY = y;                                   // Setando a posição y do novo objeto.
    toReturn->parent = NULL;                                // Deixando o parente como NULL para poder realizar a parentagem com o root.
    parentObject(root, toReturn, false);                    // Colocando a root como parente.
    toReturn->isUpdated = !updateSituation;                 // Setando a situação de update.
    
    return toReturn;                                        // Retornando o objeto.
}

void addFuncToObj(object * obj, bool (*start)(void * self), bool (*update)(void * self), bool (*end)(void * self)) {
    obj->start = start;
    obj->update = update;
    obj->end = end;
}

void destroyObject(object * targetObject) {
    if (targetObject == NULL) return;
    if (targetObject->end) targetObject->end(targetObject);
    const Uint8 type = targetObject->type, mask = targetObject->mask;

    unparentObject(targetObject, false);  
 
    while (targetObject->childrenCount > 0) destroyObject(targetObject->children[0]);   // Destruindo todos os objetos filiais a este.

    switch(mask) {
        case OBJ_MASK_RENDER:
            switch(type) {
                case RENDER_TYPE_SPRITE:
                    freeSprite(targetObject->item);
                break;
                case RENDER_TYPE_META:
                    freeMeta(targetObject->item);
                break;
            }
        break;
        case OBJ_MASK_GAME:
            switch(type) {
                case GAME_TYPE_AREA:
                    freeArea(targetObject->item);
                break;
            }
        break;
    }
    
    free(targetObject->children);                                                       // Liberando a memória do vetor de filiais.
    
    free(targetObject);                                                                 // Liberando a memória do objeto.
    
}   

void parentObject(object * parent, object * child, bool keepGlobalPosition) {
    if (child == NULL) return;
    if (child->parent != NULL) unparentObject(child, keepGlobalPosition ? true : false);    // Caso o filial já tenha um parente, realizar a deparentagem.
    
    if (parent == NULL) parent = root;

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
    } 
    else {
        child->globalX = child->localX;
        child->globalY = child->localY;
    }

    for (int i = child->parentPos; i < parent->childrenCount - 1; i++) {                    // Para todo filial do parente após o objeto orfão.
        parent->children[i] = parent->children[i + 1];                                      // Passar todos os objetos uma casa pra trás.
        ((object*) parent->children[i])->parentPos--;                                       // Ajustar o contador de posição dos outros filiados.
    }

    parent->children[parent->childrenCount - 1] = NULL;                                     // Setando o último valor do vetor de filiais pra NULL.
    parent->children = realloc(parent->children, sizeof(object) * --parent->childrenCount); // E removendo esse último valor do vetor.

    child->parent = NULL;                                                                   // E finalmente colocando NULL como ponteiro para o parente.
    child->parentPos = 0;
}

object * cloneObject(object * source) {
    object * ret = createObject(0, 0, OBJ_MASK_OBJ, OBJ_TYPE_OBJECT, NULL, source->instance);
    if (!ret) {
        errorSet("cloneObject: Não foi possível clonar o objeto.");
        return NULL;
    }
    return ret;
}

void setObjectInstanceID(object * source, char * instanceID) {
    char * newI = malloc(sizeof(char) * strlen(instanceID));
    strcpy(newI, instanceID);
    source->instance = newI;
    for (int i = 0; i < source->childrenCount; i++) {
        setObjectInstanceID(source->children[i], instanceID);
    }
}

object * instanceObject(object * toInstance, Sint32 x, Sint32 y) {
    if (!toInstance) {
        return NULL;
    }
    setObjectInstanceID(toInstance, toInstance->instance);
    const Uint8 type = toInstance->type, mask = toInstance->mask;
    object * inst = NULL, * child;
    switch(mask) {
        case OBJ_MASK_OBJ:
            switch(type) {
                case OBJ_TYPE_OBJECT:
                    inst = cloneObject(toInstance);
                break;
            }
        break;
        case OBJ_MASK_RENDER:
            switch(type) {
                case RENDER_TYPE_SPRITE:
                    inst = cloneSprite(toInstance, toInstance->instance);
                break;
                case RENDER_TYPE_META:
                    inst = cloneMeta(toInstance, toInstance->instance);
                break;
            }
        break;
        case OBJ_MASK_GAME:
            switch(type) {
                case GAME_TYPE_AREA:
                    inst = cloneArea(toInstance, toInstance->instance);
                break;
            }
        break;
    }
    if (!inst) {
        errorSet("instanceObject: Não foi possível instanciar.");
        return NULL;
    }
    addFuncToObj(inst, toInstance->start, toInstance->update, toInstance->end);
    for (int i = 0; i < toInstance->childrenCount; i++) {
        child = instanceObject(toInstance->children[i], ((object*)toInstance->children[i])->localX, ((object*)toInstance->children[i])->localY);
        if (!child) {
            continue;
        }
        parentObject(inst, child, false);
    }

    inst->localX = x;
    inst->localY = y;
    inst->isInst = true;

    if (inst->start) inst->start((void *)inst);

    return inst;
}

object * getRoot() {
    return root;
}

void coreUpdate() {
    runObjectUpdate(root);
    updateObjectPosition(root);
    
    updateSituation = !updateSituation;
}

bool runObjectUpdate(object * target) {
    if (target->update && target->isInst) {
        if (!target->update(target)) {
            errorSet("runObjectUpdate: Erro dentro de um update.");
            return false;
        }
    }
    for (int i = 0; i < target->childrenCount; i++) {
        if (!runObjectUpdate(target->children[i])) {
            return false;
        }
    }
    
    return true;
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

bool coreInit() {
    root = (object *) malloc(sizeof(object));          // Inicializando o objeto raíz e setando seus parâmetros.
    if (root == NULL) {
        errorSet("coreInit: Não foi possível alocar a raíz.");
        return false;
    }
    root->parent = NULL;
    root->parentPos = 0;
    root->localX = 0;
    root->globalX = 0;
    root->localY = 0;
    root->globalY = 0;
    root->mask = 0;
    root->type = 255;
    root->childrenCount = 0;
    root->children = malloc(0);
    if (root->children == NULL) {
        errorSet("coreInit: Não foi possível o vetor de filiais da raíz.");
        free(root);
        return false;
    }
    root->isUpdated = true;
    root->start = NULL;
    root->update = NULL;
    root->end = NULL;
    updateSituation = false;                // Deixando a situação de atualização falsa.
    return true;
}

void coreShut() {
    destroyObject(root);
    root = NULL;
}