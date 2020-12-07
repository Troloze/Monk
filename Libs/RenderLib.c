#include "RenderLib.h"


static Uint16 windowWidth;                  // Largura da janela.
static Uint16 windowHeight;                 // Altura da janela.
static Uint16 virtualWidth;                 // Largura virtual da janela.
static Uint16 virtualHeight;                // Altura virtual da janela.

static SDL_Window * window = NULL;          // Ponteiro para janela.
static SDL_Surface * windowSurface = NULL;  // Ponteiro para a superfície da janela.

static object * screenObject = NULL;        // Ponteiro para o objeto tela.
static renderCamera * currentCamera = NULL; // Ponteiro para a câmera a ser renderizada.
static renderCamera defaultCamera;          // Câmera básica.

static Uint32 bgColor;                      // Cor do fundo.

static renderLayer core;                    // Layer dos sprites principais.

static renderLayer storage;                 // Layer dos sprites genéricos.
static Uint32 storageSize;                  // Contagem do número de sprites genéricos.

static renderLayer * toRenderLayers = NULL; // Layeres de renderização.
static Uint32 * toRenderLayerSize = NULL;   // Tamanho dos layeres para renderização.
static Uint32 toRenderSize;                 // Tamanho do grupo de layers de renderização.

static renderPalette defaultPalette;        // Paleta genérica.
static renderPalette ** allPalettes;         // Lista com todas as paletas.
static Uint32 paletteCount;

static Uint8 * printedMask = NULL;          // Máscara de renderização.
static renderSprite defaultRenderSprite;    // Sprite genérico. (por enquanto é o cursor).
static SDL_Rect renderRect;                 // Rect com as dimensões da janela.

SDL_Surface * loadSurface(char * path) {
    SDL_Surface * loadSurface = SDL_LoadBMP(path);   // Superfície que irá carregar a imagem.
    
    if (loadSurface == NULL) printf("Erro carregando superfície: %s\n", SDL_GetError()); // Checando se o carregamento foi bem sucedido.
        
    return loadSurface;  // Retornando a superfície
}

bool getSpritesFromSheet(int spriteCount, int stx, int sty, renderSprite * spriteArray, char * sheetPath) {
    SDL_Surface * sheet = loadSurface(sheetPath);   // Carregando o spritesheet.
    if (sheet == NULL) return false;                // Conferindo pra evitar problemas.

    renderSprite newSprite;                         // Criando um novo objeto Sprite para ser retornado.

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
        for (int y = 0; y < 8; y++) for (int x = 0; x < 8; x++) {                   // Repetir isso para cada pixel do sprite
            cb = x%4;                                                               // Pegando o bit do pixel do sprite.
            ci = x/4;                                                               // Pegando o índice do pixel.
            
            index = (((sy * 8) + y) * sw) + ((sx * 8) + x);                         // Índice a ser usado no pixel atual.
            v = getColorValue(pixels[index]);                                       // Pegando o valor do pixel atual.

            index = (y * 2) + ci;                                                   // Índice a ser usado no novo pixel.
            newPixels[index] = setByteValue(cb * 2, 2, v, newPixels[index]);  // Colocando o valor do novo pixel em seu local.
        }
        newSprite = createSprite(newPixels, NULL, NULL, 0, 0, SPRITE_STATE_SHOWN);
        spriteArray[scount] = newSprite;                                            // Adicionando o novo sprite ao array dado.
        scount++;
        if (spriteCount <= scount) break;
    }
    if (SDL_MUSTLOCK(sheet)) SDL_UnlockSurface(sheet);    // Caso a superfície precise fechar, ela foi fechada anteriormente, e agora será aberta.
    SDL_FreeSurface(sheet);
    return true;
}

bool getSpritesFromSheetPalette(int spriteCount, int stx, int sty, renderSprite * spriteArray, char * sheetPath) {
    SDL_Surface * sheet = loadSurface(sheetPath);   // Carregando o spritesheet.
    if (sheet == NULL) return false;                // Conferindo pra evitar problemas.

    renderSprite newSprite;                         // Criando um novo objeto Sprite para ser retornado.
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
        newSprite = createSprite(newPixels, palette, NULL, 0, 0, SPRITE_STATE_SHOWN);
        spriteArray[scount] = newSprite;                                            // Adicionando o novo sprite ao array dado.
        scount++;
        if (spriteCount <= scount) break;
    }
    if (SDL_MUSTLOCK(sheet)) SDL_UnlockSurface(sheet);    // Caso a superfície precise fechar, ela foi fechada anteriormente, e agora será aberta.
    SDL_FreeSurface(sheet);
    return true;
}

renderSprite createSprite(Uint8 pixels[16], renderPalette * palette, object * parent, Sint32 x, Sint32 y, Uint8 state) {
    renderSprite newSprite; // Sprite que será retornado.
    newSprite.object = createObject(x, y);                              // Criando o objeto do sprite.
    if (parent != NULL) parentObject(parent, newSprite.object, true);   // Fazendo a associação do objeto do sprite com o do parente.
    
    if (!(palette == NULL)) newSprite.palette = (void*) palette;        // Caso a paleta tenha sido determinada, use ela, caso contrário, use a genérica.
    else newSprite.palette = (void*) &defaultPalette;

    if (!(pixels == NULL)) for (int i = 0; i < 16; i++) newSprite.pixels[i] = pixels[i];    // Passando a informação dos pixels pro sprite.
    else for (int i = 0; i < 16; i++) newSprite.pixels[i] = 0;                              // Caso pixels seja nulo, vai criar um sprite de cor 0;

    newSprite.state = state;                                            // Passando o estado do sprite.

    return newSprite;                                                   // Retorna o novo sprite.
}

void addSpriteToLayer(renderSprite sprite, Uint8 targetLayer) {
    if (targetLayer >= 0 && targetLayer < toRenderSize) {
        toRenderLayers[targetLayer] = (renderSprite *)realloc(toRenderLayers[targetLayer],sizeof(renderSprite) * (++toRenderLayerSize[targetLayer])); // Realocando o Layer para caber o novo sprite.
        toRenderLayers[targetLayer][toRenderLayerSize[targetLayer] - 1] = sprite;   // Colocando o novo sprite no espaço realocado.
    } else {
        storage = realloc(storage, sizeof(renderSprite) * (++storageSize));
        storage[storageSize - 1] = sprite;
    }
}

void addRenderLayer() {
    toRenderLayers = (renderLayer *) realloc(toRenderLayers, sizeof(renderLayer) * ++toRenderSize);
    toRenderLayerSize = (Uint32 *) realloc(toRenderLayerSize, sizeof(Uint32) * toRenderSize);
    toRenderLayers[toRenderSize - 1] = (renderSprite *) malloc(0);
    toRenderLayerSize[toRenderSize - 1] = 0;
}

renderMetasprite createMetasprite(renderSprite * sprites, Sint32 * pos, Sint32 spriteCount, Sint32 metaSizeX, Sint32 metaSizeY, object * parent, Sint32 x, Sint32 y) {
    renderMetasprite newMeta;
    renderSprite newSprite;
    newMeta.object = createObject(x, y);
    parentObject(parent, newMeta.object, false);                     // Determinando o parente do metasprite.

    if (!(parent == NULL)) {                            // Se o metasprite for afiliado à algo, usa a posição do parente pra determinar a posição global.
        newMeta.object->globalX = parent->globalX + x;
        newMeta.object->globalY = parent->globalY + y;
    } else {                                            // Caso não tenha parente, utiliza a posição normal.
        newMeta.object->globalX = x;
        newMeta.object->globalY = y;
    }

    for (int ny = 0; ny < metaSizeY; ny++) for (int nx = 0; nx < metaSizeX; nx++) {
        if (pos[ny * metaSizeX + nx] != -1) {
            for (int i = 0; i < 16; i++) newSprite.pixels[i] = sprites[pos[ny * metaSizeX + nx]].pixels[i]; // Passando a informação dos pixels para o sprite que será impresso.
            newSprite.palette = sprites[pos[ny * metaSizeX + nx]].palette;                                  // Passando a paleta para o novo sprite.
            newSprite.state = sprites[pos[ny * metaSizeX + nx]].state;                                      // Passando o estado para o novo sprite.                                       
            
            newSprite.object = createObject(nx * 8, ny * 8);                                                // Criando o objeto do novo sprite.
            
            parentObject(newMeta.object, newSprite.object, false);                                          // Filiando o novo sprite ao metasprite.

            addSpriteToLayer(newSprite, 0);                                                                 // Adicionando o sprite ao novo layer.
        }
    }
    return newMeta;
}

renderMetasprite createMetaspriteFromSheet(char * sheetPath, Uint16 pathX0, Uint16 pathY0, Uint16 metaWidth, Uint16 metaHeight, renderPalette * palette, object * parent, Sint32 x, Sint32 y) {
    renderMetasprite newMeta;                       // Meta que será retornado.
    newMeta.object = createObject(x, y);
    if (parent != NULL) parentObject(parent, newMeta.object, false);
    SDL_Surface * sheet = loadSurface(sheetPath);   // Carregando o spritesheet.

    renderSprite newSprite;                         // Criando um novo objeto Sprite para ser retornado.

    Uint8 newPixels[16];                            // Novo conjunto de pixels q será anexado ao sprite.
    int index;                                      // Índice do píxel.
    int cb;                                         // Contagem do bit dentro do sprite.
    int ci;                                         // Contagem do indice dentro do sprite.
    int v;                                          // Valor do pixel atual (0, 1, 2 e 3).

    if (SDL_MUSTLOCK(sheet)) SDL_LockSurface(sheet);// Caso seja necessário fechar a superfície para mexer nos píxels, feche.
   
    Uint32 * pixels = (Uint32*) sheet->pixels;      // Pegando os dados necessários da superfície
    int sp = sheet->pitch;
    int sh = sheet->h;
    int sw = sheet->w;

    for (int sy = pathY0; sy < metaHeight; sy++) for (int sx = pathX0; sx < metaWidth; sx++) {             // Para todo sprite dentro da spritesheet   
        for (int y = 0; y < 8; y++) for (int x = 0; x < 8; x++) {                   // Repetir isso para cada pixel do sprite
            cb = x%4;                                                               // Pegando o bit do pixel do sprite.
            ci = x/4;                                                               // Pegando o índice do pixel.
            
            index = (((sy * 8) + y) * sw) + ((sx * 8) + x);                         // Índice a ser usado no pixel atual.
            v = getColorValue(pixels[index]);                                       // Pegando o valor do pixel atual.

            index = (y * 2) + ci;                                                   // Índice a ser usado no novo pixel.
            newPixels[index] = setByteValue(cb * 2, 2, v, newPixels[index]);  // Colocando o valor do novo pixel em seu local.
        }
        newSprite = createSprite(newPixels, palette, NULL, sx * 8, sy * 8, SPRITE_STATE_SHOWN);
        addSpriteToLayer(newSprite, 0);
        parentObject(newMeta.object, newSprite.object, false);
    }
    if (SDL_MUSTLOCK(sheet)) SDL_UnlockSurface(sheet);    // Caso a superfície precise fechar, ela foi fechada anteriormente, e agora será aberta.
    SDL_FreeSurface(sheet);
    return newMeta;
}

renderMetasprite createMetaspriteFromSheetPalette(char * sheetPath, Uint16 pathX0, Uint16 pathY0, Uint16 metaWidth, Uint16 metaHeight, object * parent, Sint32 x, Sint32 y) {
    renderMetasprite newMeta;                       // Meta que será retornado.
    newMeta.object = createObject(x, y);
    if (parent != NULL) parentObject(parent, newMeta.object, false);
    SDL_Surface * sheet = loadSurface(sheetPath);   // Carregando o spritesheet.

    renderSprite newSprite;                         // Criando um novo objeto Sprite para ser retornado.
    renderPalette * palette;
    Uint32 paletteColor[4];

    Uint8 newPixels[16];                            // Novo conjunto de pixels q será anexado ao sprite.
    int index;                                      // Índice do píxel.
    int cb;                                         // Contagem do bit dentro do sprite.
    int ci;                                         // Contagem do indice dentro do sprite.
    int v;                                          // Valor do pixel atual (0, 1, 2 e 3).

    if (SDL_MUSTLOCK(sheet)) SDL_LockSurface(sheet);// Caso seja necessário fechar a superfície para mexer nos píxels, feche.
   
    Uint32 * pixels = (Uint32*) sheet->pixels;      // Pegando os dados necessários da superfície
    int sp = sheet->pitch;
    int sh = sheet->h;
    int sw = sheet->w;

    for (int sy = pathY0; sy < metaHeight; sy++) for (int sx = pathX0; sx < metaWidth; sx++) {             // Para todo sprite dentro da spritesheet   
        for (int i = 0; i < 4; i++) {
            index = ((sh * 8) * sw) + (sy * sw / 2) + ((sx * 4 * (sy%2 + 1)) + i);
            paletteColor[i] = pixels[index];
        }
        palette = createPalette(paletteColor[0], paletteColor[1], paletteColor[2], paletteColor[3]);
        for (int y = 0; y < 8; y++) for (int x = 0; x < 8; x++) {                   // Repetir isso para cada pixel do sprite
            cb = x%4;                                                               // Pegando o bit do pixel do sprite.
            ci = x/4;                                                               // Pegando o índice do pixel.
            
            index = (((sy * 8) + y) * sp) + ((sx * 8) + x);                         // Índice a ser usado no pixel atual.
            v = getColorValue(pixels[index]);                                       // Pegando o valor do pixel atual.

            index = (y * 2) + ci;                                                   // Índice a ser usado no novo pixel.
            newPixels[index] = setByteValue(cb * 2, 2, v, newPixels[index]);  // Colocando o valor do novo pixel em seu local.
        }
        newSprite = createSprite(newPixels, palette, NULL, sx * 8, sy * 8, SPRITE_STATE_SHOWN);
        addSpriteToLayer(newSprite, 0);
        parentObject(newMeta.object, newSprite.object, false);
    }
    if (SDL_MUSTLOCK(sheet)) SDL_UnlockSurface(sheet);    // Caso a superfície precise fechar, ela foi fechada anteriormente, e agora será aberta.
    SDL_FreeSurface(sheet);
    return newMeta;
}

renderPalette * createPalette(Uint32 color1, Uint32 color2, Uint32 color3, Uint32 color4) {
    renderPalette * newP = (renderPalette *) malloc(sizeof(renderPalette));
    newP->color1 = color1;
    newP->color2 = color2;
    newP->color3 = color3;
    newP->color4 = color4;
    allPalettes = (renderPalette **) realloc(allPalettes, sizeof(renderPalette *) * ++paletteCount);
    allPalettes[paletteCount - 1] = (renderPalette *) malloc(sizeof(renderPalette));
    allPalettes[paletteCount - 1] = newP;
    return newP;
}

renderPalette * createEmptyPalette() {
    renderPalette * newP = (renderPalette *) malloc(sizeof(renderPalette));
    allPalettes = (renderPalette **) realloc(allPalettes, sizeof(renderPalette *) * ++paletteCount);
    allPalettes[paletteCount - 1] = (renderPalette *) malloc(sizeof(renderPalette));
    allPalettes[paletteCount - 1] = newP;
    return newP;
}

void changeCamera(renderCamera * newCamera) {
    currentCamera = newCamera;
    parentObject(currentCamera->object, screenObject, false);
    
}

renderCamera * getDefaultCamera() {
    return &defaultCamera;
}

void clearLayer(Uint8 targetLayer) {
    renderLayer toClearLayer;
    Uint32 * toClearLayerSize;

    if (targetLayer >= 0 && targetLayer < 30) {
        toClearLayer = toRenderLayers[targetLayer];
        toClearLayerSize = &toRenderLayerSize[targetLayer];
    } else {
        toClearLayer = storage;
        toClearLayerSize = &storageSize;
    }
    
    for (int i = 0; i < *toClearLayerSize; i++) destroyObject((object *)toClearLayer[i].object); // Destruindo todos os objetos associados aos sprites.

    free(toClearLayer);             // Libera o Layer.
    *toClearLayerSize = 0;          // Coloca o tamanho do layer pra zero.
    toClearLayer = (renderLayer) malloc(0);       // Aloca 0 pra preparar o layer pra novas adições.
}

void renderAllLayers(SDL_Surface * blitSurface, renderLayer * layersToRender, Uint32 * layersToRenderSize) {
    for (int i = 0; i < toRenderSize; i++) {      // Renderizar todos os toRenderLayers.
        renderCurrentLayer(blitSurface, layersToRender[toRenderSize - 1 - i], layersToRenderSize[toRenderSize - 1 - i]);
    }
}

void renderCurrentLayer(SDL_Surface * blitSurface, renderLayer layer, Uint32 currentLayerSize) {
    if (currentLayerSize == 0) return;  // Caso não haja nenhum sprite no renderizador, pare a função.

    for (int i = 0; i < currentLayerSize; i++) {    // Para cada sprite presente no Layer.
        if (isOnArea(layer[i].object->globalX, layer[i].object->globalY, -7 + currentCamera->object->globalX, -7 + currentCamera->object->globalY, virtualWidth + currentCamera->object->globalX, virtualHeight + currentCamera->object->globalY))  // Se o sprite estiver presente na área de renderização.
        renderCurrentSprite(blitSurface, layer[i]); // Renderiza sprite.
    }
}

void renderCurrentSprite(SDL_Surface * blitSurface,renderSprite sprite) {
    if (! isOnArea(sprite.object->globalX, sprite.object->globalY, -7 + currentCamera->object->globalX, -7 + currentCamera->object->globalY, virtualWidth + currentCamera->object->globalX, virtualHeight + currentCamera->object->globalY)) return;    // Se o sprite não estiver na área de renderização, parar a função.

    if (!((sprite.state >> 3) & 1)) return;   // Se o sprite estiver desabilitado não mostrar.
    Sint32 printX, printY;      // Coordenadas que serão usadas pra printar o pixel.
    Uint8 v;                    // Valor da cor do pixel
    renderPalette * palette = (renderPalette*)sprite.palette;   // Pegando a paleta do sprite.
    Uint32 color[4];            // Paleta do sprite atual.

    color[0] = palette->color1;     // Pegando as cores da paleta.
    color[1] = palette->color2;
    color[2] = palette->color3;
    color[3] = palette->color4;

    for (int y = 0; y < 8; y++) for (int x = 0; x < 8; x++) {   // Para cada pixel no sprite.
        // Colocando imprimindo a simetria correta do sprite.
        if ((sprite.state & 7) == SPRITE_STATE_3h00) {               // 3h00
            printX = sprite.object->globalX + x - currentCamera->object->globalX;
            printY = sprite.object->globalY + y - currentCamera->object->globalY;
        } else if ((sprite.state & 7) == SPRITE_STATE_6h15) {        // 6h15
            printX = sprite.object->globalX + 7 - y - currentCamera->object->globalX;
            printY = sprite.object->globalY + x - currentCamera->object->globalY;
        } else if ((sprite.state & 7) == SPRITE_STATE_9h30) {        // 9h30
            printX = sprite.object->globalX + 7 - x - currentCamera->object->globalX;
            printY = sprite.object->globalY + 7 - y - currentCamera->object->globalY;
        } else if ((sprite.state & 7) == SPRITE_STATE_12h45) {        // 12h45
            printX = sprite.object->globalX + y - currentCamera->object->globalX;
            printY = sprite.object->globalY + 7 - x - currentCamera->object->globalY;
        } else if ((sprite.state & 7) == SPRITE_STATE_9h00) {        // 9h00
            printX = sprite.object->globalX + 7 - x - currentCamera->object->globalX;
            printY = sprite.object->globalY + y - currentCamera->object->globalY;
        } else if ((sprite.state & 7) == SPRITE_STATE_12h15) {        // 12h15
            printX = sprite.object->globalX + y - currentCamera->object->globalX;
            printY = sprite.object->globalY + x - currentCamera->object->globalY;
        } else if ((sprite.state & 7) == SPRITE_STATE_3h30) {        // 3h30
            printX = sprite.object->globalX + x - currentCamera->object->globalX;
            printY = sprite.object->globalY + 7 - y - currentCamera->object->globalY;
        } else if ((sprite.state & 7) == SPRITE_STATE_6h45) {        // 6h45
            printX = sprite.object->globalX + 7 - y - currentCamera->object->globalX;
            printY = sprite.object->globalY + 7 - x - currentCamera->object->globalY;
        }
        
        if (!printedMask[printY * virtualWidth + printX]) {
            if (isOnArea(printX, printY, 0, 0, virtualWidth, virtualHeight)) {  // Se o pixel estiver na área de renderização, renderize ele.
                v = getByteValue((x%4) * 2, 2, sprite.pixels[(y * 2) + x/4]);   // Pegar a cor do pixel
                if (color[v] & 0xFF000000) {
                    renderCurrentPixel(blitSurface, printX, printY, color[v]);   // Caso o pixel não seja transparente, printar ele com a cor adequada;
                    printedMask[printY * virtualWidth + printX] = 1;
                }
            }
        }
    }
}

void IOSpritePrint(renderSprite sprite) {
    Uint8 pixels[16];
    int v;

    for (int i = 0; i < 16; i++) pixels[i] = sprite.pixels[i];
    
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
    SDL_Surface * renderSurface = SDL_CreateRGBSurfaceWithFormat(0, virtualWidth, virtualHeight, 32, windowSurface->format->format);    // Criando uma superfície para poder colocar na janela.

    SDL_FillRect(renderSurface, &renderRect, bgColor);  // Pintando a cor do fundo.

    for (int i = 0; i < virtualWidth * virtualHeight; i++) printedMask[i] = 0;  // Setando a máscara de renderização pra 0.

    if (SDL_MUSTLOCK(renderSurface)) SDL_LockSurface(renderSurface);    // Algumas superfícies precisam ser trancadas antes de poder ter seus pixeis acessados, isso garante que não teremos problemas com isso.

    renderCurrentSprite(renderSurface, defaultRenderSprite);    // Renderizar o cursor.
    renderAllLayers(renderSurface, toRenderLayers, toRenderLayerSize);  // Renderizar os layeres de renderização.

    if (SDL_MUSTLOCK(renderSurface)) SDL_UnlockSurface(renderSurface);  // Algumas superfícies precisam ser trancadas antes de poder ter seus pixeis acessados, isso garante que não teremos problemas com isso.

    SDL_BlitScaled(renderSurface, NULL, windowSurface, &renderRect);    // Blitando a superfície renderizada na superfície da janela.
    SDL_UpdateWindowSurface(window);    // Atualizando a janela.

    SDL_FreeSurface(renderSurface); // Liberando a superfície usada pra renderizar.
}

renderSprite * getDefaultSprite() {
    return &defaultRenderSprite;
}

renderSprite * getCoreSprites() {
    return core;
}

renderWindowState getWindowState() {
    renderWindowState ret; 
    ret.windowHeight = windowHeight;
    ret.windowWidth = windowWidth;
    ret.virtualHeight = virtualHeight;
    ret.virtualWidth = virtualWidth;
    return ret;
}

bool renderInit() {
    windowWidth = 1024; // Setando os valores da janela, e da janela virtual, respectivamente.
    windowHeight = 1024;
    virtualWidth = 256;
    virtualHeight = 256;

    screenObject = createObject(0, 0);
    defaultCamera.object = createObject(0,0);
    changeCamera(&defaultCamera);
    
    allPalettes = (renderPalette **) malloc(0);
    paletteCount = 0;

    toRenderLayers = (renderLayer *) malloc(0);
    toRenderLayerSize = (Uint32 *) malloc(0);
    toRenderSize = 0;

    addRenderLayer();

    core = (renderSprite *) malloc(sizeof(renderSprite) * 256);  // Alocando o espaço necessário para o layer dos sprites principais.

    printedMask = (Uint8 *) malloc(sizeof(Uint8) * (virtualHeight * virtualWidth));   // Alocando o espaço necessário para a máscara de renderização.
    for (int i = 0; i < virtualWidth * virtualHeight; i++) printedMask[i] = 0;  // Setando como zero todos os valores da máscara de renderização.
    

    window = SDL_CreateWindow( // Criar janela.
        "Troloze's Amazing Adventure(TM)",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        windowWidth,
        windowHeight,
        SDL_WINDOW_SHOWN
    );

    if(window == NULL) {    // Checar se ela foi criada com sucesso.
        printf("Janela não pôde ser criada! Erro: %s\n", SDL_GetError());
        return false;
    }

    windowSurface = SDL_GetWindowSurface(window);   // Pegar a superfície da janela.

    defaultPalette.color1 = 0x00000000; // Colocando os valores da paleta principal
    defaultPalette.color2 = 0xFFFF0000;
    defaultPalette.color3 = 0xFF00FF00;
    defaultPalette.color4 = 0xFF0000FF;

    bgColor = 0xFF000000;   // Colocando o valor do bg.

    getSpritesFromSheetPalette(CORE_SPRITE_LIMIT, 16, 16, core, "Sprites/MainSheet.bmp"); // Colocando todos os sprites principais no layer core.
    
    defaultRenderSprite = core[CORE_SPRITE_A];   // Setando o sprite genérico.
    defaultRenderSprite.state = SPRITE_STATE_SHOWN;
    defaultRenderSprite.palette = createPalette(0x00000000, 0xFFAAAAAA, 0x00000000, 0xFFAAAAAA);
    defaultRenderSprite.object = createObject(0, 0);
    parentObject(screenObject, defaultRenderSprite.object, false);

    renderRect.x = 0;   // Colocando os valores do rect usado pra renderizar na janela.
    renderRect.y = 0;
    renderRect.h = windowHeight;
    renderRect.w = windowWidth;

    return true;
}

void renderShut() {
    for (int i = 0; i < toRenderSize; i++) {
        free(toRenderLayers[i]);   // Limpando os layeres de renderização.
    }
    free(toRenderLayers);
    free(toRenderLayerSize);

    free(core); // Limpando os layers.
    free(storage);
    for (int i = 0; i < paletteCount; i++) {
        free(allPalettes[i]);
    }
    free(allPalettes); // Limpando as paletas.

    free(printedMask);  // Limpando a máscara de renderização.

    SDL_FreeSurface(windowSurface); // Liberando a Superfície da janela.

    SDL_DestroyWindow(window);  // Destruindo a janela.
    window = NULL;
}
