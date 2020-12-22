#include "RenderLib.h"

static renderWindow * windowState = NULL;       // Estado da janela.

static SDL_Window * window = NULL;              // Ponteiro para a janela.
static SDL_Surface * windowSurface = NULL;      // Ponteiro para a superfície da janela.

static object * screenObj = NULL;               // Objeto da tela.
static renderCamera * currentCamera = NULL;     // Câmera sendo usada.
static renderCamera * defaultCamera = NULL;     // Câmera genérica.

static Uint32 bgColor;                          // Cor do plano de fundo.

static renderLayer coreLayer;                   // Layer com os ponteiros para os sprites do sistema.

static Uint8 ** storage = NULL;                 // Vetor com todos os sprites carregados.
static Uint32 storageSize;                      // Quantidade de sprites carregados.

static renderLayer toRender;                    // Layer de renderização.
static Uint32 layerPos[256];                    // Posições dos sublayeres.
static Uint8 * renderMask = NULL;               // Máscara de renderização.
static SDL_Rect renderRect;                     // Retângulo de renderização.

static renderPalette * defaultPalette = NULL;   // Paletta genérica.
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

renderLayer * getSpritesFromSheetPalette(int spriteCount, int stx, int sty, char * sheetPath) {
    SDL_Surface * sheet = loadSurface(sheetPath);   // Carregando o spritesheet.
    renderLayer * layer;
    if (sheet == NULL) {                            // Conferindo pra evitar problemas.
        printf("%s\n", errorGet());
        errorSet("getSpritesFromSheet: Não foi possível carregar a sheet.");
        return NULL;
    }           

    renderSprite * newSprite;                       // Criando um novo objeto Sprite para ser retornado.

    layer = malloc(sizeof(renderLayer));
    if (layer == NULL) {
        SDL_FreeSurface(sheet);
        errorSet("getSpritesFromSheet: Não foi possível gerar o layer de retorno.");
        return NULL;
    }
    layer->size = spriteCount;
    layer->layer = malloc(sizeof(renderSprite*) * spriteCount);
    if (layer->layer == NULL) {
        SDL_FreeSurface(sheet);
        free(layer);
        errorSet("getSpritesFromSheet: Não foi possível gerar o layer de retorno.");
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

    for (int sy = 0; sy < sty; sy++) for (int sx = 0; sx < stx; sx++) {             // Para todo sprite dentro da spritesheet   
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
        
        newSprite = createSprite(newPixels, palette, NULL, 0, 0, SPRITE_STATE_SHOWN, 0);
        if (newSprite == NULL) {
            for (int i = 0; i < scount; i++) free(layer->layer[scount]);
            free(layer->layer);
            free(layer);
            SDL_FreeSurface(sheet);
            errorSet("getSpritesFromSheet: Não foi possível criar os sprites.");
            return NULL;
        }
       
        layer->layer[scount] = newSprite;                                           // Adicionando o novo sprite ao array dado.
        scount++;
    }
    if (SDL_MUSTLOCK(sheet)) SDL_UnlockSurface(sheet);    // Caso a superfície precise fechar, ela foi fechada anteriormente, e agora será aberta.
    SDL_FreeSurface(sheet);
    return layer;
}

renderSprite * createSprite(Uint8 * pixels, renderPalette * palette, object * parent, Sint32 x, Sint32 y, Uint8 state, Uint8 layer) {
    renderSprite * newSprite; // Sprite que será retornado.
    newSprite = malloc(sizeof(renderSprite));
    if (newSprite == NULL) {
        errorSetCritical("createSprite: Não foi possível alocar espaço para o novo sprite");
        return NULL;
    }
    newSprite->object = createObject(x, y, OBJ_MASK_RENDER, RENDER_TYPE_SPRITE, newSprite); // Criando o objeto do sprite.
    if (newSprite->object == NULL) {
        free(newSprite);
        errorSet("createSprite: Não foi possível criar objeto do sprite.");
        return NULL;
    }
    if (parent != NULL) parentObject(parent, newSprite->object, true);                      // Fazendo a associação do objeto do sprite com o do parente.
    
    if (!(palette == NULL)) newSprite->palette = (void*) palette;                            // Caso a paleta tenha sido determinada, use ela, caso contrário, use a genérica.
    else newSprite->palette = (void*) &defaultPalette;

    newSprite->pixels = loadSpritePixels(pixels);
    if (newSprite->pixels == NULL) {
        printf("%s\n", errorGet());
        errorSetCritical("createSprite: Não foi possível criar a informação dos pixels do sprite");
        return NULL;
    }

    newSprite->state = state;                                           // Passando o estado do sprite.
    newSprite->layer = layer;

    return newSprite;                                                   // Retorna o novo sprite.
}

Uint8 * loadSpritePixels(Uint8 * pixels) {
    if (pixels == NULL) {
        errorSet("loadSpritePixels: NULL não é um argumento válido.");
        return NULL;
    }
    Uint64 * newPix = (Uint64 *) &pixels[0], * currentPix;
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
            } else {
                return storage[c];
            }
            
        }
        if (ll == 0 && !isBigger) c = 0;
        else if (rl == storageSize - 1 && isBigger) c = storageSize;
        else c = rl;
    } else c = 0;

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

renderLayer * getCoreSprites() {
    return &coreLayer;
}

renderWindow * getWindowState() {
    return windowState;
}

bool addSpriteToRender(renderSprite * sprite) {
    if (sprite == NULL) {
        errorSet("addSpriteToRender: NULL é um argumento inválido");
        return false;
    }
    
    toRender.layer = realloc(toRender.layer, sizeof(renderSprite *) * ++toRender.size);
    if (toRender.layer == NULL) {
        errorSet("addSpriteToRender: Não foi possível realocar o layer de renderização.");
        return false;
    }
    
    for (int i = toRender.size - 1; i > layerPos[sprite->layer]; i--) toRender.layer[i] = toRender.layer[i - 1];
    toRender.layer[layerPos[sprite->layer]] = sprite;
    
    for (int i = sprite->layer + 1; i <= 255; i++) layerPos[i] += 1;
    
    return true;
}

void changeCamera(renderCamera * newCamera) {
    if (newCamera == NULL) {
        errorSet("changeCamera: NULL não é um ponteiro válido");
        return;
    }
    currentCamera = newCamera;
    parentObject(currentCamera->object, screenObj, false);
}

bool cleanStorage() {
    for (int i = 0; i < storageSize; i++) free(storage[i]);
    storage = realloc(storage, 0);
    if (storage == NULL) {
        errorSet("cleanStorage: Não foi possível realocar o armazém");
        return false;
    }
    storageSize = 0;
}

bool clearLayer() {
    for (int i = 0; i < 256; i++) layerPos[i] = 0;
    free(toRender.layer);
    toRender.size = 0;
    toRender.layer = malloc(0);
    if(toRender.layer == NULL) {
        errorSet("clearLayer: Não foi possível alocar layer de renderização.");
        return false;
    }
    return true;
}

bool clearSpriteLayer(renderLayer * layer) {
    for (int i = 0; i < layer->size; i++) {
        clearSprite(layer->layer[i]);
    }
    layer->size = 0;
}

bool clearSprite(renderSprite * sprite) {
    free(sprite);
}

void renderFrame(SDL_Surface * blitSurface) {
    if (blitSurface == NULL) {
        errorSetCritical("renderFrame: Superfície inválida.");
        return;
    }
    for (int i = 0; i < toRender.size; i++) {
        renderCurrentSprite(blitSurface, toRender.layer[i]);
    }
}

void renderCurrentSprite(SDL_Surface * blitSurface,renderSprite * sprite) {
    if (sprite == NULL) {
        errorSetCritical("renderCurrentSprite: Tentativa de renderizar um sprite NULL.");
        return;
    }
    if (blitSurface == NULL) {
        errorSetCritical("renderCurrentSprite: Superfície inválida.");
        return;
    }
    if (! isOnArea(sprite->object->globalX, sprite->object->globalY, -7 + currentCamera->object->globalX, -7 + currentCamera->object->globalY, windowState->virtualWidth + currentCamera->object->globalX, windowState->virtualHeight + currentCamera->object->globalY)) return;    // Se o sprite não estiver na área de renderização, parar a função.

    if (!((sprite->state >> 3) & 1)) return;   // Se o sprite estiver desabilitado não mostrar.
    Sint32 printX, printY;      // Coordenadas que serão usadas pra printar o pixel.
    Uint8 v;                    // Valor da cor do pixel
    renderPalette * palette = (renderPalette*)sprite->palette;   // Pegando a paleta do sprite.
    Uint32 color[4];            // Paleta do sprite atual.

    color[0] = palette->color1;     // Pegando as cores da paleta.
    color[1] = palette->color2;
    color[2] = palette->color3;
    color[3] = palette->color4;

    for (int y = 0; y < 8; y++) for (int x = 0; x < 8; x++) {   // Para cada pixel no sprite.
        // Colocando imprimindo a simetria correta do sprite.
        if ((sprite->state & 7) == SPRITE_STATE_3h00) {               // 3h00
            printX = sprite->object->globalX + x - currentCamera->object->globalX;
            printY = sprite->object->globalY + y - currentCamera->object->globalY;
        } else if ((sprite->state & 7) == SPRITE_STATE_6h15) {        // 6h15
            printX = sprite->object->globalX + 7 - y - currentCamera->object->globalX;
            printY = sprite->object->globalY + x - currentCamera->object->globalY;
        } else if ((sprite->state & 7) == SPRITE_STATE_9h30) {        // 9h30
            printX = sprite->object->globalX + 7 - x - currentCamera->object->globalX;
            printY = sprite->object->globalY + 7 - y - currentCamera->object->globalY;
        } else if ((sprite->state & 7) == SPRITE_STATE_12h45) {        // 12h45
            printX = sprite->object->globalX + y - currentCamera->object->globalX;
            printY = sprite->object->globalY + 7 - x - currentCamera->object->globalY;
        } else if ((sprite->state & 7) == SPRITE_STATE_9h00) {        // 9h00
            printX = sprite->object->globalX + 7 - x - currentCamera->object->globalX;
            printY = sprite->object->globalY + y - currentCamera->object->globalY;
        } else if ((sprite->state & 7) == SPRITE_STATE_12h15) {        // 12h15
            printX = sprite->object->globalX + y - currentCamera->object->globalX;
            printY = sprite->object->globalY + x - currentCamera->object->globalY;
        } else if ((sprite->state & 7) == SPRITE_STATE_3h30) {        // 3h30
            printX = sprite->object->globalX + x - currentCamera->object->globalX;
            printY = sprite->object->globalY + 7 - y - currentCamera->object->globalY;
        } else if ((sprite->state & 7) == SPRITE_STATE_6h45) {        // 6h45
            printX = sprite->object->globalX + 7 - y - currentCamera->object->globalX;
            printY = sprite->object->globalY + 7 - x - currentCamera->object->globalY;
        }
        
        if (!renderMask[printY * windowState->virtualWidth + printX]) {
            if (isOnArea(printX, printY, 0, 0, windowState->virtualWidth, windowState->virtualHeight)) {  // Se o pixel estiver na área de renderização, renderize ele.
                v = getByteValue((x%4) * 2, 2, sprite->pixels[(y * 2) + x/4]);   // Pegar a cor do pixel
                if (color[v] & 0xFF000000) {
                    renderCurrentPixel(blitSurface, printX, printY, color[v]);   // Caso o pixel não seja transparente, printar ele com a cor adequada;
                    renderMask[printY * windowState->virtualWidth + printX] = 1;
                }
            }
        }
    }
}

void IOSpritePrint(renderSprite * sprite) {
    Uint8 * pixels;
    int v;

    pixels = sprite->pixels;
    
    for (int y = 0; y < 8; y++) for (int x = 0; x < 8; x++) {
        
        v = getByteValue((x%4) * 2, 2, pixels[(y * 2) + x/4]);

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

void renderUpdate() {
    SDL_Surface * renderSurface = SDL_CreateRGBSurfaceWithFormat(0, windowState->virtualWidth, windowState->virtualHeight, 32, windowSurface->format->format);    // Criando uma superfície para poder colocar na janela.

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
    renderLayer * newCore;
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

    screenObj = createObject(0, 0, OBJ_MASK_OBJ, OBJ_TYPE_OBJECT, screenObj);
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

    defaultCamera->object = createObject(0, 0, OBJ_MASK_RENDER, RENDER_TYPE_CAMERA, &defaultCamera);
    if (defaultCamera->object == NULL) {
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

    toRender.layer = malloc(0);
    if (toRender.layer == NULL) {
        reportError();
        errorSet("renderInit: Não foi possível alocar o vetor do layer de renderização.");
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
        free(toRender.layer);
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
        free(toRender.layer);
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
        free(toRender.layer);
        free(renderMask);
        free(defaultCamera);
        SDL_DestroyWindow(window);
        return false;
    }

    defaultPalette = malloc(sizeof(renderPalette));
    if (defaultPalette == NULL) {
        reportError();
        errorSet("renderInit: Não foi possível alocar a paletta genérica");
        cleanStorage();
        free(storage);
        free(windowState);
        free(allPalettes);
        free(toRender.layer);
        free(renderMask);
        free(defaultCamera);
        SDL_FreeSurface(windowSurface);
        SDL_DestroyWindow(window);
    }

    defaultPalette->color1 = 0x00000000; // Colocando os valores da paleta principal
    defaultPalette->color2 = 0xFFFF0000;
    defaultPalette->color3 = 0xFF00FF00;
    defaultPalette->color4 = 0xFF0000FF;

    bgColor = 0xFF000000;   // Colocando o valor do bg.

    newCore = getSpritesFromSheetPalette(CORE_SPRITE_LIMIT, 16, 16, "Sprites/MainSheet.bmp"); // Colocando todos os sprites principais no layer core.
    if (newCore == NULL) {
        reportError();
        errorSet("renderInit: Não foi possível pegar os sprites do sistema");
        cleanStorage();
        free(storage);
        free(windowState);
        free(allPalettes);
        free(toRender.layer);
        free(renderMask);
        free(defaultCamera);
        free(defaultPalette);
        SDL_FreeSurface(windowSurface);
        SDL_DestroyWindow(window);
        return false;
    }
    coreLayer.size = newCore->size;
    coreLayer.layer = newCore->layer;
    free(newCore);

    renderRect.x = 0;   // Colocando os valores do rect usado pra renderizar na janela.
    renderRect.y = 0;
    renderRect.h = windowState->windowHeight;
    renderRect.w = windowState->windowWidth;

    return true;
}

void renderShut() {
    clearLayer();
    free(toRender.layer);
    
    cleanStorage();
    free(storage);
    free(windowState);
    free(allPalettes);
    free(renderMask);
    free(defaultCamera);
    free(defaultPalette);

    SDL_FreeSurface(windowSurface);
    SDL_DestroyWindow(window);
}