#include "RenderLib.h"


static Uint16 windowWidth;              // Largura da janela.
static Uint16 windowHeight;             // Altura da janela.
static Uint16 virtualWidth;             // Largura virtual da janela.
static Uint16 virtualHeight;            // Altura virtual da janela.

static SDL_Window * window;             // Ponteiro para janela.
static SDL_Surface * windowSurface;     // Ponteiro para a superfície da janela.

static Uint32 bgColor;                  // Cor do fundo.

static renderLayer core;                // Layer dos sprites principais.

static renderLayer storage;             // Layer dos sprites genéricos.
static Uint16 storageSize;              // Contagem do número de sprites genéricos.

static renderLayer toRenderLayers[30];  // Layeres de renderização.
static Uint16 toRenderLayerSize[30];    // Tamanho dos layeres de renderização.

static renderPalette defaultPalette;    // Paleta genérica.
static renderPalette allPalettes[100];  // Lista com todas as paletas.

static Uint8 * printedMask;             // Máscara de renderização.
static renderSprite defaultRenderSprite;// Sprite genérico. (por enquanto é o cursor).
static SDL_Rect renderRect;             // Rect com as dimensões da janela.



SDL_Surface * loadSurface(char * path) {
    SDL_Surface * loadSurface = SDL_LoadBMP(path);   // Superfície que irá carregar a imagem.
    
    if (loadSurface == NULL) {  // Checando se o carregamento foi bem sucedido.
        printf("Erro carregando superfície: %s\n", SDL_GetError());
        return NULL;
    }
    
    return loadSurface;  // Retornando a superfície
}

bool getSpritesFromSheet(int spriteCount, int stx, int sty, renderSprite * spriteArray, char * sheetPath) {
    SDL_Surface * sheet = loadSurface(sheetPath);   // Carregando o spritesheet.
    if (sheet == NULL) return false;                // Conferindo pra evitar problemas.

    renderSprite newSprite;                         // Criando um novo objeto Sprite para ser retornado.
    newSprite = defaultRenderSprite;

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
        newSprite = createSprite(NULL, newPixels, NULL, NULL, 0, 0, SPRITE_STATE_SHOWN);
        spriteArray[scount] = newSprite;                                            // Adicionando o novo sprite ao array dado.
        scount++;
        if (spriteCount <= scount) break;
    }
    if (SDL_MUSTLOCK(sheet)) SDL_UnlockSurface(sheet);    // Caso a superfície precise fechar, ela foi fechada anteriormente, e agora será aberta.
    return true;
}

renderSprite createSprite(char * name, Uint8 pixels[16], renderPalette * palette, object * parent, Sint32 x, Sint32 y, Uint8 state) {
    renderSprite newSprite; // Sprite que será retornado.
    newSprite.object = malloc(sizeof(object));
    newSprite.object->parent = parent;  // Determinando o parente do sprite.
    if (name == NULL) newSprite.name = malloc(1);
    else {
        newSprite.name = malloc(sizeof(name));
        newSprite.name = name;
    }

    if (!(palette == NULL)) newSprite.palette = (void*) palette;    // Caso a paleta tenha sido determinada, use ela, caso contrário, use a genérica.
    else newSprite.palette = (void*) &defaultPalette;

    if (!(pixels == NULL)) for (int i = 0; i < 16; i++) newSprite.pixels[i] = pixels[i];    // Passando a informação dos pixels pro sprite.
    else for (int i = 0; i < 16; i++) newSprite.pixels[i] = 0;  // Caso pixels seja nulo, vai criar um sprite de cor 0;

    newSprite.object->localX = x;   // Marca as coordenadas locais do sprite
    newSprite.object->localY = y;

    if (!(parent == NULL)) {    // Se o sprite for afiliado à algo, usa a posição do parente pra determinar a posição global.
        newSprite.object->globalX = parent->globalX + x;
        newSprite.object->globalY = parent->globalY + y;
    } else {    // Caso não tenha parente, utiliza a posição normal.
        newSprite.object->globalX = x;
        newSprite.object->globalY = y;
    }

    newSprite.state = state;    // Passando o estado do sprite.

    return newSprite;   // Retorna o novo sprite.
}

void addSpriteToLayer(renderSprite sprite, Uint8 targetLayer) {
    if (targetLayer >= 0 && targetLayer < 30) {
        toRenderLayers[targetLayer] = (renderSprite *)realloc(toRenderLayers[targetLayer],sizeof(renderSprite) * (++toRenderLayerSize[targetLayer])); // Realocando o Layer para caber o novo sprite.
        toRenderLayers[targetLayer][toRenderLayerSize[targetLayer] - 1] = sprite;   // Colocando o novo sprite no espaço realocado.
    } else {
        storage = realloc(storage, sizeof(renderSprite) * (++storageSize));
        storage[storageSize - 1] = sprite;
    }
}

renderMetasprite createMetasprite(char * name, renderSprite * sprites, Sint16 * pos, Sint32 spriteCount, Sint32 metaSizeX, Sint32 metaSizeY, object * parent, Sint32 x, Sint32 y) {
    renderMetasprite newMeta;
    renderSprite newSprite;
    newMeta.object = createObject(x, y);
    parentObject(parent, newMeta.object, false);                     // Determinando o parente do metasprite.

    if (name == NULL) newMeta.name = malloc(1);
    else {
        newMeta.name = malloc(sizeof(name));
        newMeta.name = name;
    }

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

            newSprite.name = malloc(sizeof(sprites[pos[ny * metaSizeX + nx]].name));                        // Passando o nome para o novo sprite.
            newSprite.name = sprites[pos[ny * metaSizeX + nx]].name;                                        
            
            newSprite.object = createObject(nx * 8, ny * 8);                                                // Criando o objeto do novo sprite.
            
            parentObject(newMeta.object, newSprite.object, false);                                          // Filiando o novo sprite ao metasprite.

            addSpriteToLayer(newSprite, 0);                                                                 // Adicionando o sprite ao novo layer.
        }
    }
    return newMeta;
}

void clearLayer(Uint8 targetLayer) {
    renderLayer toClearLayer;
    Uint16 * toClearLayerSize;

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
    toClearLayer = malloc(0);       // Aloca 0 pra preparar o layer pra novas adições.
}

int getColorValue(Uint32 pixel) {
    if (!(pixel ^ COLOR1)) return 0;         // Se o pixel indicar cor 1.
    else if (!(pixel ^ COLOR2)) return 1;    // Se o pixel indicar cor 2.
    else if (!(pixel ^ COLOR3)) return 2;    // Se o pixel indicar cor 3.
    else return 3;                           // Se o pixel indicar cor 4 ou não definido.
}

void renderAllLayers(SDL_Surface * blitSurface, renderLayer * layersToRender, Uint16 * layersToRenderSize) {
    for (int i = 0; i < 30; i++) {      // Renderizar todos os toRenderLayers.
        renderCurrentLayer(blitSurface, layersToRender[29 - i], layersToRenderSize[29 - i]);
    }
}

void renderCurrentLayer(SDL_Surface * blitSurface, renderLayer layer, Uint16 currentLayerSize) {
    if (currentLayerSize == 0) return;  // Caso não haja nenhum sprite no renderizador, pare a função.

    for (int i = 0; i < currentLayerSize; i++) {    // Para cada sprite presente no Layer.
        if (isOnArea(layer[i].object->globalX, layer[i].object->globalY, -7, -7, virtualWidth, virtualHeight))  // Se o sprite estiver presente na área de renderização.
        renderCurrentSprite(blitSurface, layer[i]); // Renderiza sprite.
    }
}

void renderCurrentSprite(SDL_Surface * blitSurface,renderSprite sprite) {
    if (! isOnArea(sprite.object->globalX, sprite.object->globalY, -7, -7, virtualWidth, virtualHeight)) return;    // Se o sprite não estiver na área de renderização, parar a função.

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
            printX = sprite.object->globalX + x;
            printY = sprite.object->globalY + y;
        } else if ((sprite.state & 7) == SPRITE_STATE_6h15) {        // 6h15
            printX = sprite.object->globalX + 7 - y;
            printY = sprite.object->globalY + x;
        } else if ((sprite.state & 7) == SPRITE_STATE_9h30) {        // 9h30
            printX = sprite.object->globalX + 7 - x;
            printY = sprite.object->globalY + 7 - y;
        } else if ((sprite.state & 7) == SPRITE_STATE_12h45) {        // 12h45
            printX = sprite.object->globalX + y;
            printY = sprite.object->globalY + 7 - x;
        } else if ((sprite.state & 7) == SPRITE_STATE_9h00) {        // 9h00
            printX = sprite.object->globalX + 7 - x;
            printY = sprite.object->globalY + y;
        } else if ((sprite.state & 7) == SPRITE_STATE_12h15) {        // 12h15
            printX = sprite.object->globalX + y;
            printY = sprite.object->globalY + x;
        } else if ((sprite.state & 7) == SPRITE_STATE_3h30) {        // 3h30
            printX = sprite.object->globalX + x;
            printY = sprite.object->globalY + 7 - y;
        } else if ((sprite.state & 7) == SPRITE_STATE_6h45) {        // 6h45
            printX = sprite.object->globalX + 7 - y;
            printY = sprite.object->globalY + 7 - x;
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

renderSprite getDefaultSprite() {
    return defaultRenderSprite;
}

renderSprite * getCoreSprites() {
    return core;
}

void setDefaultSprite(renderSprite sprite) {
    defaultRenderSprite = sprite;
}

bool renderInit() {
    windowWidth = 1024; // Setando os valores da janela, e da janela virtual, respectivamente.
    windowHeight = 1024;
    virtualWidth = 256;
    virtualHeight = 256;

    core = malloc(sizeof(renderSprite) * 256);  // Alocando o espaço necessário para o layer dos sprites principais.

    printedMask = malloc(sizeof(Uint8) * (virtualHeight * virtualWidth));   // Alocando o espaço necessário para a máscara de renderização.
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

    getSpritesFromSheet(CORE_SPRITE_LIMIT, 16, 16, core, "Sprites/MainSheet.bmp"); // Colocando todos os sprites principais no layer core.

    allPalettes[0].color1 = 0x00000000;     // Criando uma paletta nova.
    allPalettes[0].color2 = 0xFFAAAAAA;
    allPalettes[0].color3 = 0x00000000;
    allPalettes[0].color4 = 0xFFAAAAAA;

    for (int i = 0; i < 30; i++) {          // Preparando os layeres de renderização.
        toRenderLayers[i] = malloc(0);
        toRenderLayerSize[i] = 0;
    }
   
    defaultRenderSprite = core[CORE_SPRITE_CURSOR_1];   // Setando o sprite genérico.
    defaultRenderSprite.state = SPRITE_STATE_SHOWN;
    defaultRenderSprite.palette = &allPalettes[0];
    defaultRenderSprite.object = malloc(sizeof(object));
    defaultRenderSprite.object->globalX = 60;
    defaultRenderSprite.object->globalY = 60;


    renderRect.x = 0;   // Colocando os valores do rect usado pra renderizar na janela.
    renderRect.y = 0;
    renderRect.h = windowHeight;
    renderRect.w = windowWidth;

    return true;
}

void renderShut() {
    for (int i = 0; i < 30; i++) free(toRenderLayers[i]);   // Limpando os layeres de renderização.

    free(core); // Limpando os layers.

    free(printedMask);  // Limpando a máscara de renderização.

    SDL_FreeSurface(windowSurface); // Liberando a Superfície da janela.

    SDL_DestroyWindow(window);  // Destruindo a janela.
    window = NULL;
}
