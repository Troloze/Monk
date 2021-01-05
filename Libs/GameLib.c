#include "GameLib.h"


static gameArea ** dump = NULL;
static Uint8 ** overlap = NULL;

static gameOverlapFunc *** func = NULL;

static gameOverlapFunc zeroF;

static gameOverlapFunc *** toCheckFunc = NULL;
static char *** toCheckCol = NULL;
static char ** checkList = NULL;
static Uint32 * colCheckPos = NULL;
static Uint32 checkCount = 0;

static Uint32 * dumpPos = NULL;

gameArea * createArea(Uint16 w, Uint16 h, Uint8 layer, Uint64 layerMask, object * parent, char * inst) {
    if (layer >= 64 || layer < 0) {
        errorSet("createArea: Layer inválido, use um layer entre 0 e 63.");
        return NULL;
    }
    char * newTag = NULL;
    gameArea * ret = malloc(sizeof(gameArea));

    if (ret == NULL) {
        errorSet("createArea: Não foi possível criar a área.");
        return NULL;
    }

    ret->obj = createObject(0, 0, OBJ_MASK_GAME, GAME_TYPE_AREA, ret, inst);
    if (!ret->obj) {
        free(ret);
        free(newTag);
        errorSet("createArea: Não foi possível criar objeto.");
        return NULL;
    }
    if (parent) parentObject(parent, ret->obj, false);

    ret->active = true;
    ret->width = w;
    ret->height = h;
    ret->layer = layer;
    ret->layerMask = layerMask;

    if (!addAreaToDump(ret)) {
        destroyObject(ret->obj);
        free(ret);
        free(newTag);
        errorSet("createArea: Não foi possível adicionar área ao dump");
        return NULL;
    }

    return ret;
}

gameArea * addAreaToDump(gameArea * toDump) {
    if(!toDump) {
        errorSet("addAreaToDump: Não é possível adicionar uma área NULL.");
        return NULL;
    }

    const Uint32 firstIndex = dumpPos[toDump->layer];
    const Uint32 lastIndex = dumpPos[64];

    toDump->pos = dumpPos[toDump->layer];
    
    func = realloc(func, sizeof(gameOverlapFunc **) * (lastIndex + 1));
    if (!func) {
        errorSet("addAreaToDump: Não foi possível realocar o func.");
        return NULL;
    }

    memmove(func + firstIndex + 1, func + firstIndex, (lastIndex - firstIndex) * sizeof(gameOverlapFunc**));
    func[firstIndex] = NULL;

    for (int i = 0; i <= lastIndex; i++) {
        func[i] = realloc(func[i], sizeof(gameOverlapFunc*) * (lastIndex  + 1));
        if (!func[i]) {
            errorSet("addAreaToDump: Não foi possível realocar as colunas de func.");
            return NULL;
        }
        memmove(func[i] + firstIndex + 1, func[i] + firstIndex, (lastIndex - firstIndex) * sizeof(gameOverlapFunc*));
    }

    for (int i = 0; i <= lastIndex; i++) {
        func[firstIndex][i] = 0;
        func[i][firstIndex] = 0;
    }

    overlap = realloc(overlap, sizeof(Uint8 *) * (lastIndex + 1));                        // Realocando o overlap para colocar mais um elemento.
    if (!overlap) {
        errorSet("addAreaToDump: Não foi possível realocar o overlap.");
        free(dump);
        return NULL;
    }

    memmove(overlap + firstIndex + 1, overlap + firstIndex, (lastIndex - firstIndex) * sizeof(Uint8*));      // Movendo os elementos de overlap.

    overlap[firstIndex] = NULL;

    for (int i = 0; i <= lastIndex; i++) {                                                // Realocando as colunas de overlap.
        overlap[i] = realloc(overlap[i], sizeof(Uint8) * lastIndex + 1);
        if (!overlap[i]) {
            errorSet("addAreaToDump: Não foi possível realocar o overlap.");
            free(dump);
            for (int j = 0; j < dumpPos[64]; j++) if(overlap[j]) free(overlap[j]);
            free(overlap);
            return NULL;
        }
        memmove(overlap[i] + firstIndex + 1, overlap[i] + firstIndex, (lastIndex - firstIndex) * sizeof(Uint8));
    }

    for (int i = 0; i <= dumpPos[64]; i++) {
        overlap[dumpPos[toDump->layer]][i] = 0;
        overlap[i][dumpPos[toDump->layer]] = 0;
    }

    dump = realloc(dump, sizeof(gameArea *) * (lastIndex + 1));
    if (!dump) {
        errorSet("addAreaToDump: Não foi possível adicionar a área ao dump.");
        return NULL;
    }

    memmove(dump + firstIndex + 1, dump + firstIndex, (lastIndex - firstIndex) * sizeof(gameArea *));

    dump[firstIndex] = toDump;

    for (int i = firstIndex + 1; i <= lastIndex; i++) {
        dump[i]->pos++;
    }
    
    for (int i = toDump->layer + 1; i <= 64; i++) dumpPos[i]++;

    return toDump;
}

bool removeAreaFromDump(gameArea * area) {
    if (!area) {
        errorSet("removeAreaFromDump: Não é possível remover área NULL.");
        return false;
    }
    
    const Uint32 firstIndex = area->pos, lastIndex = dumpPos[64] - 1;
    Uint32 pos; 

    free(func[firstIndex]);
    memmove(func + firstIndex, func + firstIndex + 1, (lastIndex - firstIndex) * sizeof(gameOverlapFunc**));
    func = realloc(func, sizeof(gameOverlapFunc**) * lastIndex);
    if (!func) {
        errorSet("removeAreaFromDump: Não foi possível realocar o func.");
        return NULL;
    }

    for(int i = 0; i < lastIndex; i++) {
        memmove(func[i] + firstIndex, func[i] + firstIndex + 1, (lastIndex - firstIndex) * sizeof(gameOverlapFunc*));
        func[i] = realloc(func[i], sizeof(gameOverlapFunc*) * lastIndex);
        if (!func[i]) {
            errorSet("removeAreaFromDump: Não foi possível realocar as colunas de func.");
        }
    }

    free(overlap[firstIndex]);

    memmove(overlap + firstIndex, overlap + firstIndex + 1, (lastIndex - firstIndex) * sizeof(Uint8 *));
    overlap = realloc(overlap, sizeof(Uint8*) * lastIndex);
    if (!overlap) {
        errorSet("removeAreaFromDump: Não foi possível realocar o overlap.");
        return false;
    }

    for (int i = 0; i < lastIndex; i++) {
        memmove(overlap[i] + firstIndex, overlap[i] + firstIndex + 1, (lastIndex - firstIndex) * sizeof(Uint8));
        overlap[i] = realloc(overlap[i], sizeof(Uint8) * lastIndex);
        if (!overlap[i]) {
            errorSet("removeAreaFromDump: Não foi possível realocar o overlap.");
            return false;
        }
    }

    memmove(dump + firstIndex, dump + firstIndex + 1, (lastIndex - firstIndex) * sizeof(gameArea *));
    dump = realloc(dump, sizeof(gameArea*) * lastIndex);

    for (int i = firstIndex; i < lastIndex; i++) {
        dump[i]->pos = i;
    }
    
    for (int i = area->layer + 1; i <= 64; i++) dumpPos[i]--;

    return true;
}

bool freeArea(gameArea * area) {
    if (area == NULL) return true;
    removeAreaFromDump(area);
    free(area);
    return true;
}

bool updateArea() {
    gameArea * current, * check;
    int tagCheck = 0, tagCurrent = 0, currentTPos = -1, checkTPos = -1, comp;
    char * cuInst, * chInst;
    gameOverlapFunc * cFunc;
    Uint8 * tagOC;
    Uint32 overSize;    
    for (int i = 0; i < 64; i++) {
        for (int j = dumpPos[i]; j < dumpPos[i + 1]; j++) {
            current = dump[j];
            cuInst = current->obj->instance;
            if (current->layerMask != 0) {
                for (int x = 0; x < 64; x++) {
                    if (current->layerMask & (1 << x)) {
                        for (int y = dumpPos[x]; y < dumpPos[x+1]; y++) {
                            check = dump[y];
                            chInst = check->obj->instance;
                            cFunc = &zeroF;
                            for (int f = 0; f < checkCount; f++) {
                                comp = strcmp(cuInst, checkList[f]);
                                if (comp == 0) {
                                    for (int f1 = 0; f1 < colCheckPos[f]; f1++) {
                                        comp = strcmp(chInst, toCheckCol[f][f1]);
                                        if (comp == 0) {
                                            cFunc = toCheckFunc[f][f1];
                                        }
                                        if (comp < 0) {
                                            break;   
                                        }
                                    }
                                    if (comp < 0) break;
                                    comp = 0;
                                }
                                if (comp < 0) break;
                            }
                            if (areaOverlap(current->obj->globalX, current->obj->globalY, current->width, current->height, check->obj->globalX, check->obj->globalY, check->width, check->height)) {
                                switch (overlap[j][y]) {
                                case 0: 
                                    overlap[j][y] = 1;
                                    if (cFunc->onEnter) {
                                        cFunc->onEnter(current->obj, check->obj);
                                    }
                                    break;
                                case 1: 
                                    overlap[j][y] = 2;
                                    if (cFunc->onOverlap) {
                                        cFunc->onOverlap(current->obj, check->obj);
                                    }
                                    break;
                                case 2: 
                                    overlap[j][y] = 2;
                                    if (cFunc->onOverlap) {
                                        cFunc->onOverlap(current->obj, check->obj);
                                    }
                                    break;
                                case 3: 
                                    overlap[j][y] = 1;
                                    if (cFunc->onEnter) {
                                        cFunc->onEnter(current->obj, check->obj);
                                    }
                                    break;
                                default: 
                                    overlap[j][y] = 1;
                                    if (cFunc->onEnter) {
                                        cFunc->onEnter(current, check);
                                    }
                                }
                                
                            } 
                            else {
                                switch (overlap[j][y]) {
                                case 0: 
                                    overlap[j][y] = 0;
                                    break;
                                case 1: 
                                    overlap[j][y] = 3;
                                    if (cFunc->onExit) {
                                        cFunc->onExit(current->obj, check->obj);
                                    }
                                    break;
                                case 2: 
                                    overlap[j][y] = 3;
                                    if (cFunc->onExit) {
                                        cFunc->onExit(current->obj, check->obj);
                                    }
                                    break;
                                case 3: 
                                    overlap[j][y] = 0;
                                    break;
                                default: 
                                    overlap[j][y] = 0;
                                }
                            }
                        }
                    }
                    else {  // Se o layer atual não estiver na mascara, só zera tudo q é sucesso.
                        for (int y = dumpPos[x]; y < dumpPos[x + 1]; y++) {
                            overlap[j][y] = 0;;
                        }
                    }
                }
            }
            else {
                for (int y = 0; y < dumpPos[64]; y++) {
                    overlap[j][y] = 0;
                } 
            }
        }
    } 
    return true;
}

void printOverlap() {
    for (int i = 0; i < dumpPos[64]; i++) {
        for (int j = 0; j < dumpPos[64]; j++) {
            printf("%d ", overlap[i][j]);
        }
        printf("\n");
    }
}


bool addFuncToInstOverlap(char * inst1, char * inst2, bool (*onEnter)(void * self, void * overlap), bool (*onOverlap)(void * self, void * overlap), bool (*onExit)(void * self, void * overlap)) {
    bool exists1 = false, exists2 = false;
    Sint32 pos1 = 0, pos2 = 0;
    if (checkList) {
        for (int i = 0; i < checkCount; i++) {
            pos1 = i;
            if (strcmp(checkList[i], inst1) == 0) {
                for (int j = 0; j = colCheckPos[i]; i++) {
                    pos2 = j;
                    if (strcmp(toCheckCol[i][j], inst2) == 0) {
                        exists2 = true;
                        break;
                    }

                    if (strcmp(toCheckCol[i][j], inst2) < 0) break;
                }
                exists1 = true;
                break;
            }

            if (strcmp(checkList[i], inst1) < 0) break;
        }
    }

    if (!exists1) {
        checkList = realloc(checkList, sizeof(char *) * (checkCount + 1));
        memmove(checkList + pos1 + 1, checkList + pos1, (checkCount - pos1) * sizeof(char*));
        checkList[pos1] = malloc(sizeof(char) * strlen(inst1));
        strcpy(checkList[pos1], inst1);

        colCheckPos = realloc(colCheckPos, sizeof(Uint32) * (checkCount + 1));
        memmove(colCheckPos + pos1 + 1, colCheckPos + pos1, (checkCount - pos1) * sizeof(Uint32));
        colCheckPos[pos1] = 0;

        toCheckCol = realloc(toCheckCol, sizeof(char **) * (checkCount + 1));
        memmove(toCheckCol + pos1 + 1, toCheckCol + pos1, (checkCount - pos1) * sizeof(char**));
        toCheckCol[pos1] = NULL;

        toCheckFunc = realloc(toCheckFunc, sizeof(gameOverlapFunc **) * (checkCount + 1));
        openMem(toCheckFunc, pos1, checkCount, sizeof(gameOverlapFunc**));
        toCheckCol[pos1] = NULL;

        checkCount++;
    }

    if (!exists2) {
        toCheckCol[pos1] = realloc(toCheckCol[pos1], sizeof(char*) * (colCheckPos[pos1] + 1));
        openMem(toCheckCol[pos1], pos2, colCheckPos[pos1], sizeof(char*));
        toCheckCol[pos1][pos2] = realloc(toCheckCol[pos1][pos2], sizeof(char) * strlen(inst2));
        strcpy(toCheckCol[pos1][pos2], inst2);

        toCheckFunc[pos1] = realloc(toCheckFunc[pos1], sizeof(gameOverlapFunc *) * (colCheckPos[pos1] + 1));
        openMem(toCheckFunc[pos1], pos2, colCheckPos[pos1], sizeof(gameOverlapFunc *)); 
        toCheckFunc[pos1][pos2] = NULL;

        colCheckPos[pos1]++;
    }

    if (toCheckFunc[pos1][pos2]) free(toCheckFunc[pos1][pos2]);

    toCheckFunc[pos1][pos2] = malloc(sizeof(gameOverlapFunc));
    toCheckFunc[pos1][pos2]->onEnter = onEnter;
    toCheckFunc[pos1][pos2]->onOverlap = onOverlap;
    toCheckFunc[pos1][pos2]->onExit = onExit;
}


object * cloneArea(object * source, char * inst) {
    if (!source) {
        return NULL;
    }

    const gameArea * areaSource = source->item;
    gameArea * ret = createArea(areaSource->width, areaSource->height, areaSource->layer, areaSource->layerMask, NULL, inst);
    if (!ret) return NULL;
    return ret->obj;
}


bool gameUpdate() {
    for (int i = 0; i < dumpPos[64]; i++) if (!dump[i]->active) freeArea(dump[i]);
    
    if (!updateArea()) return false;
    
    return true;
}



bool gameInit() {
    colCheckPos = malloc(sizeof(Uint32 *) * 2);
    checkCount = 0;
    if (!colCheckPos) {
        errorSet("gameInit: Não foi possível alocar o colCheckPos");
        return false;
    }
    
    zeroF.onEnter = NULL;
    zeroF.onExit = NULL;
    zeroF.onOverlap = NULL;
    
    dump = malloc(sizeof(gameArea *) * 64);
    if (dump == NULL) {
        errorSet("gameInit: Não foi possível criar o dump de áreas.");
        return false;
    }

    dumpPos = malloc(sizeof(Uint32) * 65);
    if (dumpPos == NULL) {
        errorSet("gameInit: Não foi possível criar a contagem dos dumps.");
        free(dump);
        return false;
    }
    
    for (int i = 0; i <= 64; i++) dumpPos[i] = 0;
    

    overlap = malloc(0);
    if (!overlap) {
        free(dump);
        free(dumpPos);
    }

    return true;
}

bool gameShut() {
    Uint32 dumpSize = dumpPos[64];
    if (dump) {
        free(dump);
        dump = NULL;
    }
    
    if (overlap) {
        free(overlap);
        overlap = NULL;
    }

    if (dumpPos) free(dumpPos);
    free(colCheckPos);
    dumpPos = NULL;
}