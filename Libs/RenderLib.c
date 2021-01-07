#include "RenderLib.h"

static renderWindow * windowState = NULL;       // Estado da janela.

static SDL_Window * window = NULL;              // Ponteiro para a janela.
static SDL_Surface * windowSurface = NULL;      // Ponteiro para a superfície da janela.

static object * screenObj = NULL;               // Objeto da tela.
static renderCamera * currentCamera = NULL;     // Câmera sendo usada.
static renderCamera * defaultCamera = NULL;     // Câmera genérica.

static Uint32 bgColor;                          // Cor do plano de fundo.

static renderGroup * coreLayer;                 // Layer com os ponteiros para os sprites do sistema.

static Uint8 ** storage = NULL;                 // Vetor com todos os sprites carregados.
static Uint32 storageSize;                      // Quantidade de sprites carregados.

static renderGroup toRender;                    // Layer de renderização.
static Uint32 layerPos[257];                    // Posições dos sublayeres.
static Uint8 * renderMask = NULL;               // Máscara de renderização.
static SDL_Rect renderRect;                     // Retângulo de renderização.

static renderPalette ** allPalettes = NULL;     // Todas as paletas carregadas.
static Uint32 paletteCount;                     // Quantidade de paletas carregadas.

SDL_Surface * loadSurface(char * path) {
    SDL_Surface * loadSurface = SDL_LoadBMP(path);   // Superfície que irá carregar a imagem.
    
    if (loadSurface == NULL) {  // Checando se o carregamento foi bem sucedido.
        errorSet("loadSurface: Erro ao carregar a superfície.");
        printf("Erro carregando superfície: %s\n", SDL_GetError());
        return NULL;
    } 
        
    return loadSurface;  // Retornando a superfície
}

renderGroup * getSpritesFromSheet(char * sheetPath, int spriteCount, int stx, int sty, int x0, int x1, int y0, int y1, bool followSheetPos, Uint8 spriteLayer) {
    SDL_Surface * sheet = loadSurface(sheetPath);   // Carregando o spritesheet.
    renderGroup * group;
    if (sheet == NULL) {                            // Conferindo pra evitar problemas.
        printf("%s\n", errorGet());
        errorSet("getSpritesFromSheet: Não foi possível carregar a sheet.");
        return NULL;
    }     

    // Conferindo se as coordenadas dadas são válidas.
    if (((x0 > x1 || x1 > stx) && !(x0 == -1 && x1 == -1)) || ((y0 > y1 || y1 > sty) && !(y0 = -1 && y1 == -1))) {
        errorSet("getSpriteFromSheet: Coordenadas inválidas.");
        SDL_FreeSurface(sheet);
        return NULL;
    }
    
    if (x0 == -1) {
        x0 = 0;
        x1 = stx;
    }
    if (y0 == -1) {
        y0 = 0;
        y1 = sty;
    }

    // Caso spriteCount seja -1, mudar para o número máximo de sprites dentro da área dada.
    if (spriteCount == -1) spriteCount = (x1 - x0) * (y1 - y0);  
    
    renderSprite * newSprite;                       // Criando um novo objeto Sprite para ser retornado.

    group = createGroup(spriteCount, 0, 0, NULL, NULL);
    if (group == NULL) {
        SDL_FreeSurface(sheet);
        errorSet("getSpritesFromSheet: Não foi possível criar o group.");
        return NULL;
    }
    
    renderPalette * palette;
    Uint32 paletteColor[4];
    Uint8 newPixels[16];                            // Novo conjunto de pixels q será anexado ao sprite.
    int scount = 0;                                 // Contagem dos Sprites que foram carregados.
    int index;                                      // Índice do píxel.
    int cb;                                         // Contagem do bit dentro do sprite.
    int ci;                                         // Contagem do indice dentro do sprite.
    int v;                                          // Valor do pixel atual (0, 1, 2 e 3).

    if (SDL_MUSTLOCK(sheet)) SDL_LockSurface(sheet);// Caso seja necessário fechar a superfície para mexer nos píxels, feche.
   
    Uint32 * pixels = (Uint32*) sheet->pixels;      // Pegando os dados necessários da superfície
    int sp = sheet->pitch;
    int sh = sheet->h;
    int sw = sheet->w;
    
    for (int sy = y0; sy < y1; sy++) for (int sx = x0; sx < x1; sx++) {             // Para todo sprite dentro da spritesheet   
        if (spriteCount <= scount) break;
        
        for (int i = 0; i < 4; i++) {
            index = ((sty * 8) * sw) + (sy * sw /2) + ((sx * 4) + i);
            paletteColor[i] = pixels[index];
        }
        palette = createPalette(paletteColor[0], paletteColor[1], paletteColor[2], paletteColor[3]);
        
        for (int y = 0; y < 8; y++) for (int x = 0; x < 8; x++) {                   // Repetir isso para cada pixel do sprite
            cb = x%4;                                                               // Pegando o bit do pixel do sprite.
            ci = x/4;                                                               // Pegando o índice do pixel.
            
            index = (((sy * 8) + y) * sw) + ((sx * 8) + x);                         // Índice a ser usado no pixel atual.
            v = getColorValue(pixels[index]);                                       // Pegando o valor do pixel atual.

            index = (y * 2) + ci;                                                   // Índice a ser usado no novo pixel.
            newPixels[index] = setByteValue(cb * 2, 2, v, newPixels[index]);  // Colocando o valor do novo pixel em seu local.
        }
        
        if (followSheetPos) newSprite = createSprite(newPixels, palette, NULL, NULL, sx * 8, sy * 8, SPRITE_STATE_SHOWN, spriteLayer);
        else newSprite = createSprite(newPixels, palette, NULL, NULL, 0, 0, SPRITE_STATE_SHOWN, 0);
        if (newSprite == NULL) {
            for (int i = 0; i < scount; i++) free(group->group[scount]);
            free(group->group);
            free(group);
            SDL_FreeSurface(sheet);
            errorSet("getSpritesFromSheet: Não foi possível criar os sprites.");
            return NULL;
        }
        group->group[scount] = newSprite;                                           // Adicionando o novo sprite ao array dado.
        scount++;
    }
    if (SDL_MUSTLOCK(sheet)) SDL_UnlockSurface(sheet);    // Caso a superfície precise fechar, ela foi fechada anteriormente, e agora será aberta.
    SDL_FreeSurface(sheet);
    return group;
}

renderSprite * createSprite(Uint8 * pixels, renderPalette * palette, object * parent, char * inst, Sint32 x, Sint32 y, Uint8 state, Uint8 layer) {
    renderSprite * newSprite; // Sprite que será retornado.
    newSprite = malloc(sizeof(renderSprite));
    if (newSprite == NULL) {
        errorSetCritical("createSprite: Não foi possível alocar espaço para o novo sprite");
        return NULL;
    }
    newSprite->obj = createObject(x, y, OBJ_MASK_RENDER, RENDER_TYPE_SPRITE, newSprite, inst); // Criando o objeto do sprite.
    if (newSprite->obj == NULL) {
        free(newSprite);
        errorSet("createSprite: Não foi possível criar objeto do sprite.");
        return NULL;
    }
    if (parent != NULL) parentObject(parent, newSprite->obj, true);                      // Fazendo a associação do objeto do sprite com o do parente.
    
    if (!(palette == NULL)) newSprite->palette = (void*) palette;                            // Caso a paleta tenha sido determinada, use ela, caso contrário, use a genérica.
    else newSprite->palette = createPalette(0x00000000, 0xFFFF0000, 0xFF00FF00, 0xFF0000FF);

    newSprite->pixels = loadSpritePixels(pixels);
    if (newSprite->pixels == NULL) {
        printf("%s\n", errorGet());
        errorSetCritical("createSprite: Não foi possível criar a informação dos pixels do sprite");
        return NULL;
    }

    newSprite->state = state;                                           // Passando o estado do sprite.
    newSprite->layer = layer;

    newSprite->meta = NULL;

    return newSprite;                                                   // Retorna o novo sprite.
}

renderGroup * createGroup(Uint32 size, Sint32 x, Sint32 y, object * parent, char * inst) {
    renderGroup * ret = malloc(sizeof(renderGroup));
    if (ret == NULL) {
        errorSet("createGroup: Não foi possível alocar o group.");
        return NULL;
    }
    ret->group = malloc(sizeof(renderSprite *) * size);
    if (ret->group == NULL) {
        errorSet("createGroup: Não foi possível alocar o espaço do group.");
        free(ret);
        return NULL;
    }
    ret->size = size;
    return ret;
}

renderMetasprite * createMeta(Sint32 x, Sint32 y, object * parent, char * inst) {
    renderMetasprite * ret = malloc(sizeof(renderMetasprite));
    if (ret == NULL) {
        errorSet("createMeta: Não foi possível alocar espaço para o metasprite.");
        return NULL;
    }
    ret->state = SPRITE_STATE_SHOWN;
    ret->obj = createObject(x, y, OBJ_MASK_RENDER, RENDER_TYPE_META, ret, inst);
    if (parent) parentObject(parent, ret->obj, false);
    if (ret->obj == NULL) {
        errorSet("createMeta: Não foi possível criar o objeto do metasprite.");
        free(ret);
        return NULL;
    }
    
    return ret;
}

renderGroup * addToGroup(renderGroup * group, renderSprite * sprite) {
    if (group == NULL) {
        errorSet("addToGroup: NULL não é um group válido.");
        return NULL;
    }
    if (sprite == NULL) {
        errorSet("addToGroup: NULL não é um sprite válido.");
        return NULL;
    }
    group->group = realloc(group->group, sizeof(renderSprite*) * ++group->size);
    if (group->group == NULL) {
        errorSet("addToGroup: Não foi possível realocar o group.");
        return NULL;
    }
    group->group[group->size - 1] = sprite;
    return group;
}

renderMetasprite * addSpriteToMeta(renderMetasprite * meta, renderSprite * sprite) {
    if (!meta) {
        return NULL;
    }
    if (!sprite) {
        return NULL;
    }
    parentObject(meta->obj, sprite->obj, false);
    sprite->meta = meta;
}

renderMetasprite * addToMeta(renderMetasprite * meta, renderGroup * group) {
    if (meta == NULL) {
        errorSet("addToMeta: NULL não é um metasprite válido");
        return NULL;
    }
    if (group == NULL) {
        errorSet("addToMeta: NULL não é um group válido.");
        return NULL;
    }

    for (int i = 0; i < group->size; i++) {
        addSpriteToMeta(meta, group->group[i]);
    }

    return meta;
}

renderMetasprite * createMetaFromSheet(char * path, int spriteCount, int stx, int sty, int x0, int x1, int y0, int y1, Sint32 x, Sint32 y, object * parent, Uint8 layer) {
    renderMetasprite * ret = createMeta(x, y, parent, NULL);
    if (ret == NULL) {
        errorSet("createMetaFromSheet: Não foi possível criar o metasprite.");
        return NULL;
    }
    renderGroup * retLayer = getSpritesFromSheet(path, spriteCount, stx, sty, x0, x1, y0, y1, true, layer);
    if (retLayer == NULL) {
        destroyObject(ret->obj);
        errorSet("createMetaFromSheet: Não foi possível criar o layer do metasprite");
        return NULL;
    }
    if (addToMeta(ret, retLayer) == NULL) {
        freeMeta(ret);
        freeGroup(retLayer, true);
        errorSet("createMetaFromSheet: Não foi possível associar o layer ao metasprite");
        return NULL;
    }
    return ret;
}

Uint8 * loadSpritePixels(Uint8 * pixels) {
    if (pixels == NULL) {
        errorSet("loadSpritePixels: NULL não é um argumento válido.");
        return NULL;
    }
    const Uint64 * newPix = (Uint64 *) &pixels[0];
    Uint64 * currentPix;
    Uint32 ll = 0, rl = storageSize - 1, c;
    bool isBigger, isEqual;
    if (storageSize > 0) {
        while (1) { // Busca binária no armazém de sprites pra conferir se não existem sprites iguais
            c = (ll + rl)/2;
            if ((rl - ll) == 1) {
                currentPix = (Uint64 *) &storage[ll][0];

                isBigger = (currentPix[1] > newPix[1]) || ((currentPix[1] == newPix[1]) && (currentPix[0] > currentPix[0]));
                isEqual = (currentPix[1] == newPix[1]) && (currentPix[0] == newPix[0]);
                if (isEqual) return storage[ll];
                if (!isBigger) break;

                currentPix = (Uint64 *) &storage[rl][0];

                isBigger = (currentPix[1] > newPix[1]) || ((currentPix[1] == newPix[1]) && (currentPix[0] > currentPix[0]));
                isEqual = (currentPix[1] == newPix[1]) && (currentPix[0] == newPix[0]);
                if (isEqual) return storage[rl];
                break;
            }
            
            currentPix = (Uint64 *) &storage[c][0];

            isBigger = (currentPix[1] > newPix[1]) || ((currentPix[1] == newPix[1]) && (currentPix[0] > currentPix[0]));
            isEqual = (currentPix[1] == newPix[1]) && (currentPix[0] == newPix[0]);

            if (!isEqual) {
                if (ll == rl) break;
                if (isBigger) ll = c;
                else rl = c;
            } 
            else {
                return storage[c];
            }
            
        }
        if (ll == 0 && !isBigger) c = 0;
        else if (rl == storageSize - 1 && isBigger) c = storageSize;
        else c = rl;
    } 
    else c = 0;

    storage = realloc(storage, sizeof(Uint8 *) * ++storageSize);
    if (storage == NULL) {
        errorSet("loadSpritePixels: Não foi possível realocar o armazém de sprites");
        return NULL;
    }
    for (int i = storageSize - 1; i > c ; i--) storage[i] = storage[i - 1];

    storage[c] = malloc(sizeof(Uint8) * 16);    // Alocando o espaço para o novo sprite.
    if (storage[c] == NULL) {
        errorSet("loadSpritePixels: Não foi possível alocar o novo sprite para o armazém");
        return NULL;
    }
    currentPix = (Uint64*) &storage[c][0];      // Passando o ponteiro em modo de Uint 64 pra acelerar o processo.
    currentPix[0] = newPix[0];                  
    currentPix[1] = newPix[1];

    return storage[c];
}

renderPalette * createPalette(Uint32 color1, Uint32 color2, Uint32 color3, Uint32 color4) {
    renderPalette * newP = (renderPalette *) malloc(sizeof(renderPalette));
    if (newP == NULL) {
        errorSet("createPalette: Não foi possível criar a paleta.");
        return NULL;
    }
    newP->color1 = color1;
    newP->color2 = color2;
    newP->color3 = color3;
    newP->color4 = color4;
    allPalettes = (renderPalette **) realloc(allPalettes, sizeof(renderPalette *) * ++paletteCount);
    if (allPalettes == NULL) {
        free(newP);
        errorSet("createPalette: Não foi possível realocar o vetor de paletas");
        return NULL;
    }
    allPalettes[paletteCount - 1] = (renderPalette *) malloc(sizeof(renderPalette));
    if (allPalettes[paletteCount - 1] == NULL) {
        free(newP);
        allPalettes = (renderPalette **) realloc(allPalettes, sizeof(renderPalette *) * --paletteCount);
        if (allPalettes == NULL) {
            errorSetCritical("createPalette: Não foi alocar a paleta, e não foi possível realocar o vetor.");
            return NULL;
        }
        errorSet("Não foi possível alocar a nova paleta");
        return NULL;
    }
    allPalettes[paletteCount - 1] = newP;
    return newP;
}

renderPalette * createEmptyPalette() {
    return createPalette(0,0,0,0);
}

renderCamera * getDefaultCamera() {
    return defaultCamera;
}

renderGroup * getCore() {
    return coreLayer;
}

renderSprite * getCoreSprite(int n) {
    if (n < 0 || n >= CORE_SPRITE_LIMIT) {
        errorSet("getCoreSprites: Índice inválido.");
        return NULL;
    }
    return coreLayer->group[n];
}

renderWindow * getWindowState() {
    return windowState;
}

object * cloneSprite(object * source, char * inst) {
    if (!source) {
        return NULL;
    }
    renderSprite * spriteSource = (renderSprite *) source->item;
    renderSprite * ret = createSprite(spriteSource->pixels, spriteSource->palette, NULL, inst, 0, 0, spriteSource->state, spriteSource->layer);
    return ret->obj;
}

object * cloneMeta(object * source, char * inst) {
    if (!source) {
        return NULL;
    }
    renderSprite * toAddSprite, * spriteSource;
    renderMetasprite * metaSource = source->item;
    renderMetasprite * ret = createMeta(0, 0, NULL, inst);
    if (!ret) {
        return NULL;
    }

    return ret->obj;
}

bool addSpriteToRender(renderSprite * sprite) {
    if (sprite == NULL) {
        errorSet("addSpriteToRender: NULL é um argumento inválido");
        return false;
    }
    
    toRender.group = realloc(toRender.group, sizeof(renderSprite *) * ++toRender.size);
    if (toRender.group == NULL) {
        errorSet("addSpriteToRender: Não foi possível realocar o group de renderização.");
        return false;
    }
    
    for (int i = toRender.size - 1; i > layerPos[sprite->layer]; i--) toRender.group[i] = toRender.group[i - 1];
    toRender.group[layerPos[sprite->layer]] = sprite;
    
    for (int i = sprite->layer + 1; i <= 256; i++) layerPos[i] += 1;
    
    return true;
}

bool addMetaToRender(renderMetasprite * meta) {
    if (!meta) return false;
    object * cChild;
    for (int i = 0; i < meta->obj->childrenCount; i++) {
        cChild = (object*) meta->obj->children[i];
        if (cChild->type == RENDER_TYPE_SPRITE && cChild->mask == OBJ_MASK_RENDER) {
            addSpriteToRender((renderSprite*) cChild->item);
        }
    }
    return true;
}

void changeCamera(renderCamera * newCamera) {
    if (newCamera == NULL) {
        errorSet("changeCamera: NULL não é um ponteiro válido");
        return;
    }
    currentCamera = newCamera;
    parentObject(currentCamera->obj, screenObj, false);
}

bool cleanStorage() {
    for (int i = 0; i < storageSize; i++) {
        free(storage[i]);
    }
    free(storage);
    storage = NULL;
    storageSize = 0;
}

bool clearRenderLayer() {
    for (int i = 0; i < 256; i++) layerPos[i] = 0;
    free(toRender.group);
    toRender.size = 0;
    toRender.group = malloc(0);
    if(toRender.group == NULL) {
        errorSet("clearLayer: Não foi possível alocar group de renderização.");
        return false;
    }
    return true;
}

bool clearLayer(renderGroup * group) {
    for (int i = 0; i < group->size; i++) {
        freeSprite(group->group[i]);
    }
    group->size = 0;
}

bool freeSprite(renderSprite * sprite) {
    if (!sprite) {
        return false;
    }
    for (int i = layerPos[sprite->layer]; i < layerPos[sprite->layer + 1]; i++){
        if (toRender.group[i] == sprite) {
            toRender.group[i] = NULL;
           
            memmove(toRender.group + i, toRender.group + i + 1, (toRender.size - i - 1) * sizeof(renderSprite *));

            toRender.group = realloc(toRender.group, sizeof(renderSprite *) * toRender.size - 1);

            if (!toRender.group) {
                return false;
            }
            toRender.size--;
            for (int j = sprite->layer + 1; j < 257; j++) layerPos[j]--;
            break;
        }
    }

    free(sprite);
    return true;
}

bool freeGroup(renderGroup * group, bool removeSprites) {
    if (group == NULL) {
        return false;
    }
    if (removeSprites) {
        for (int i = 0; i < group->size; i++) {
            destroyObject(group->group[i]->obj);
        }
    }
    free(group->group);
    free(group);
}

bool freeMeta(renderMetasprite * meta) {
    if (meta == NULL) {
        errorSet("freeMeta: NULL não é um metasprite válido");
        return false;
    }
    
    free(meta); 
    return true;
}

void renderFrame(SDL_Surface * blitSurface) {
    if (blitSurface == NULL) {
        errorSetCritical("renderFrame: Superfície inválida.");
        return;
    }
    for (int i = 0; i < toRender.size; i++) {
        renderCurrentSprite(blitSurface, toRender.group[i]);
    }
}

void renderCurrentSprite(SDL_Surface * blitSurface,renderSprite * sprite) {
    if (sprite == NULL) {
        errorSetCritical("renderCurrentSprite: Tentativa de renderizar um sprite NULL.\n");
        return;
    }
    if (blitSurface == NULL) {
        errorSetCritical("renderCurrentSprite: Superfície inválida.");
        return;
    }
    if (! isOnArea(sprite->obj->globalX, sprite->obj->globalY, -7 + currentCamera->obj->globalX, -7 + currentCamera->obj->globalY, windowState->virtualWidth + currentCamera->obj->globalX, windowState->virtualHeight + currentCamera->obj->globalY)) return;    // Se o sprite não estiver na área de renderização, parar a função.

    if (!((sprite->state >> 3) & 1)) return;    // Se o sprite estiver desabilitado não mostrar.
    //if (sprite->alpha <= 0 || sprite->alpha > 13) return;   // Se o sprite estiver com alpha 0, ou acima do máximo, não mostrar.
    Uint8 pstate;
    
    if (sprite->meta != NULL) { // Se o sprite estiver associado a um meta.
        if (!((((renderMetasprite*)sprite->meta)->state >> 3) & 1)) return; // Se o metasprite estiver desabilitado, não mostrar.
        pstate = ((sprite->state ^ ((renderMetasprite*)sprite->meta)->state) & 0b100) | ((sprite->state + ((renderMetasprite*)sprite->meta)->state) & 0b11);  // Calculando as flags com a mistura do meta e do sprite.
    } 
    else {  // Se não.
        pstate = sprite->state & 7;
    }
    
    Sint32 printX, printY;      // Coordenadas que serão usadas pra printar o pixel.
    Uint8 v;                    // Valor da cor do pixel
    
    //renderPalette * palette = (renderPalette*)sprite->palette;   // Pegando a paleta do sprite.

    //(((Uint32 *)&(((renderPalette*)sprite->palette)[0]))[v]);

    for (int y = 0; y < 8; y++) for (int x = 0; x < 8; x++) {   // Para cada pixel no sprite.
        // Colocando imprimindo a simetria correta do sprite.
        if ((pstate) == SPRITE_STATE_3h00) {                // 3h00
            printX = sprite->obj->globalX + x - currentCamera->obj->globalX;
            printY = sprite->obj->globalY + y - currentCamera->obj->globalY;
        } 
        else if ((pstate) == SPRITE_STATE_6h15) {           // 6h15
            printX = sprite->obj->globalX + 7 - y - currentCamera->obj->globalX;
            printY = sprite->obj->globalY + x - currentCamera->obj->globalY;
        } 
        else if ((pstate) == SPRITE_STATE_9h30) {           // 9h30
            printX = sprite->obj->globalX + 7 - x - currentCamera->obj->globalX;
            printY = sprite->obj->globalY + 7 - y - currentCamera->obj->globalY;
        } 
        else if ((pstate) == SPRITE_STATE_12h45) {          // 12h45
            printX = sprite->obj->globalX + y - currentCamera->obj->globalX;
            printY = sprite->obj->globalY + 7 - x - currentCamera->obj->globalY;
        } 
        else if ((pstate) == SPRITE_STATE_9h00) {           // 9h00
            printX = sprite->obj->globalX + 7 - x - currentCamera->obj->globalX;
            printY = sprite->obj->globalY + y - currentCamera->obj->globalY;
        } 
        else if ((pstate) == SPRITE_STATE_12h15) {          // 12h15
            printX = sprite->obj->globalX + y - currentCamera->obj->globalX;
            printY = sprite->obj->globalY + x - currentCamera->obj->globalY;
        } 
        else if ((pstate) == SPRITE_STATE_3h30) {           // 3h30
            printX = sprite->obj->globalX + x - currentCamera->obj->globalX;
            printY = sprite->obj->globalY + 7 - y - currentCamera->obj->globalY;
        } 
        else if ((pstate) == SPRITE_STATE_6h45) {           // 6h45
            printX = sprite->obj->globalX + 7 - y - currentCamera->obj->globalX;
            printY = sprite->obj->globalY + 7 - x - currentCamera->obj->globalY;
        }
        
            
        if (!renderMask[printY * windowState->virtualWidth + printX]) {
            if (isOnArea(printX, printY, 0, 0, windowState->virtualWidth, windowState->virtualHeight)) {  // Se o pixel estiver na área de renderização, renderize ele.
                v = getByteValue((x&3) * 2, 2, sprite->pixels[(y * 2) + (x>>2)]);   // Pegar a cor do pixel
                if ((((Uint32 *)&(((renderPalette*)sprite->palette)[0]))[v]) & 0xFF000000) {
                    renderCurrentPixel(blitSurface, printX, printY, (((Uint32 *)&(((renderPalette*)sprite->palette)[0]))[v]));   // Caso o pixel não seja transparente, printar ele com a cor adequada;
                    renderMask[printY * windowState->virtualWidth + printX] = 1;
                }
            }
        }
        
    }
}

void IOSpritePrint(Uint8 * pixels) {
    int v;

    for (int y = 0; y < 8; y++) for (int x = 0; x < 8; x++) {
        
        v = getByteValue((x & 3) * 2, 2, pixels[(y * 2) + x>>2]);

        switch (v) {
            case 0:
                printf("  ");
            break;
            case 1:
                printf("# ");
            break;
            case 2:
                printf(". ");
            break;
            case 3:
                printf("| ");
            break;
        }
        if (x == 7) printf("\n");
    }
}

void setBgColor(Uint32 newColor) {
    bgColor = newColor;
}

Uint32 getBgColor() {
    return bgColor;
}

void changeMetaPalette(renderMetasprite * meta, renderPalette * palette) {
    object * c;
    for (int i = 0; i < meta->obj->childrenCount; i++) {
        c = (object*)meta->obj->children[i];
        if (c->mask == OBJ_MASK_RENDER && c->type == RENDER_TYPE_SPRITE) {
            ((renderSprite*) c->item)->palette = palette;
        }
    }
}

void renderUpdate() {
    SDL_Surface * renderSurface = SDL_CreateRGBSurfaceWithFormat(0, windowState->virtualWidth, windowState->virtualHeight, 32, windowSurface->format->format);    // Criando uma superfície para poder colocar na janela.
    if (renderSurface == NULL) {
        errorSetCritical("Não foi possível criar a superfície de renderização: ");
        printf("%s\n", SDL_GetError());
        return;
    }

    SDL_FillRect(renderSurface, &renderRect, bgColor);  // Pintando a cor do fundo.

    for (int i = 0; i < windowState->virtualWidth * windowState->virtualHeight; i++) renderMask[i] = 0;  // Setando a máscara de renderização pra 0.

    if (SDL_MUSTLOCK(renderSurface)) SDL_LockSurface(renderSurface);    // Algumas superfícies precisam ser trancadas antes de poder ter seus pixeis acessados, isso garante que não teremos problemas com isso.

    renderFrame(renderSurface);  // Renderizar os layeres de renderização.

    if (SDL_MUSTLOCK(renderSurface)) SDL_UnlockSurface(renderSurface);  // Algumas superfícies precisam ser trancadas antes de poder ter seus pixeis acessados, isso garante que não teremos problemas com isso.

    SDL_BlitScaled(renderSurface, NULL, windowSurface, &renderRect);    // Blitando a superfície renderizada na superfície da janela.
    SDL_UpdateWindowSurface(window);    // Atualizando a janela.

    SDL_FreeSurface(renderSurface); // Liberando a superfície usada pra renderizar.
}

bool renderInit() {
    windowState = malloc(sizeof(renderWindow));
    if (windowState == NULL) {
        errorSet("renderInit: Não foi possível alocar o estado de janela.");
        return false;
    }
    
    windowState->windowWidth = 1024; // Setando os valores da janela, e da janela virtual, respectivamente.
    windowState->windowHeight = 1024;
    windowState->virtualWidth = 256;
    windowState->virtualHeight = 256;

    storage = malloc(0);
    if (storage == NULL) {
        reportError();
        errorSet("renderInit: Não foi possível alocar memória para o armazém de sprites.");
        free(windowState);
        return false;
    }

    screenObj = createObject(0, 0, OBJ_MASK_OBJ, OBJ_TYPE_OBJECT, screenObj, NULL);
    if (screenObj == NULL) {
        reportError();
        errorSet("renderInit: Não foi possível criar objeto de tela.");
        free(windowState);
        cleanStorage();
        free(storage);
        return false;
    }
    
    defaultCamera = malloc(sizeof(renderCamera));
    if (defaultCamera == NULL) {
        reportError();
        errorSet("renderInit: Não foi possível alocar a câmera.");
        cleanStorage();
        free(storage);
        free(windowState);
        return false;
    }

    defaultCamera->obj = createObject(0, 0, OBJ_MASK_RENDER, RENDER_TYPE_CAMERA, defaultCamera, NULL);
    if (defaultCamera->obj == NULL) {
        reportError();
        errorSet("renderInit: Não foi possível criar o objeto da câmera.");
        cleanStorage();
        free(storage);
        free(defaultCamera);
        free(windowState);
        return false;
    }
    
    changeCamera(defaultCamera);
    
    allPalettes = malloc(0);
    if (allPalettes == NULL) {
        reportError();
        errorSet("renderInit: Não foi possível alocar o vetor de paletas.");
        cleanStorage();
        free(storage);
        free(windowState);
        free(defaultCamera);
        return false;
    }
    
    paletteCount = 0;

    toRender.group = malloc(0);
    if (toRender.group == NULL) {
        reportError();
        errorSet("renderInit: Não foi possível alocar o vetor do group de renderização.");
        cleanStorage();
        free(storage);
        free(windowState);
        free(allPalettes);
        free(defaultCamera);
        return false;
    }

    renderMask = malloc(sizeof(Uint8) * (windowState->virtualHeight * windowState->virtualWidth));   // Alocando o espaço necessário para a máscara de renderização.
    if (renderMask == NULL) {
        reportError();
        errorSet("renderInit: Não foi possível alocar a máscara de renderização.");
        cleanStorage();
        free(storage);
        free(windowState);
        free(allPalettes);
        free(toRender.group);
        free(defaultCamera);
        return false;
    }
   
    for (int i = 0; i < windowState->virtualWidth * windowState->virtualHeight; i++) renderMask[i] = 0;  // Setando como zero todos os valores da máscara de renderização.
    

    window = SDL_CreateWindow("Troloze's Amazing Adventure(TM)", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowState->windowWidth, windowState->windowHeight, SDL_WINDOW_SHOWN); // Criar janela.

    if(window == NULL) {    // Checar se ela foi criada com sucesso.
        printf("renderInit: %s\n", SDL_GetError());
        cleanStorage();
        free(storage);
        free(windowState);
        free(allPalettes);
        free(toRender.group);
        free(renderMask);
        free(defaultCamera);
        return false;
    }

    windowSurface = SDL_GetWindowSurface(window);   // Pegar a superfície da janela.
    if (windowSurface == NULL) {
        printf("renderInit: %s\n", SDL_GetError());
        cleanStorage();
        free(storage);
        free(windowState);
        free(allPalettes);
        free(toRender.group);
        free(renderMask);
        free(defaultCamera);
        SDL_DestroyWindow(window);
        return false;
    }

    bgColor = 0xFF000000;   // Colocando o valor do bg.

    coreLayer = getSpritesFromSheet("Sprites/MainSheet.bmp", CORE_SPRITE_LIMIT, 16, 16, -1, -1, -1, -1, false, 0);    // Colocando todos os sprites principais no group core.
    if (coreLayer == NULL) {
        reportError();
        errorSet("renderInit: Não foi possível pegar os sprites do sistema");
        cleanStorage();
        free(storage);
        free(windowState);
        free(allPalettes);
        free(toRender.group);
        free(renderMask);
        free(defaultCamera);
        SDL_FreeSurface(windowSurface);
        SDL_DestroyWindow(window);
        return false;
    }
    
    renderRect.x = 0;   // Colocando os valores do rect usado pra renderizar na janela.
    renderRect.y = 0;
    renderRect.h = windowState->windowHeight;
    renderRect.w = windowState->windowWidth;

    return true;
}

void renderShut() {
    clearRenderLayer();
    free(toRender.group);
    
    cleanStorage();
    free(windowState);
    free(allPalettes);
    free(renderMask);
    free(defaultCamera);
    freeGroup(coreLayer, false);

    SDL_FreeSurface(windowSurface);
    SDL_DestroyWindow(window);
}
