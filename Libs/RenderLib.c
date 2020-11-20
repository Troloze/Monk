#include "RenderLib.h"

static SDL_Window * window;
static SDL_Surface * windowSurface;
static Uint16 windowWidth;
static Uint16 windowHeight;
static Uint16 virtualWidth;
static Uint16 virtualHeight;
static Uint32 bgColor;
static renderLayer layers[30];
static Uint16 layerSize[30];
static renderSprite defaultRenderSprite;
static SDL_Rect renderRect;
static renderPalette defaultPalette;
static renderSprite core[256];

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
            newPixels[index] = convertValueToByte(cb * 2, 2, v, newPixels[index]);  // Colocando o valor do novo pixel em seu local.
        }
        newSprite = createSprite(newPixels, NULL, NULL, 0, 0, SPRITE_STATE_SHOWN);
        spriteArray[scount] = newSprite;                                            // Adicionando o novo sprite ao array dado.
        scount++;
        if (spriteCount <= scount) break;
    }
    if (SDL_MUSTLOCK(sheet)) SDL_UnlockSurface(sheet);    // Caso a superfície precise fechar, ela foi fechada anteriormente, e agora será aberta.
    return true;
}

renderSprite createSprite(Uint8 pixels[16], renderPalette * palette, renderSprite * parent, Sint32 x, Sint32 y, Uint8 state) {
    renderSprite newSprite;
    newSprite.parent = (void*) parent;

    if (!(palette == NULL)) newSprite.palette = (void*) palette;
    else newSprite.palette = (void*) &defaultPalette;

    if (!(pixels == NULL)) for (int i = 0; i < 16; i++) newSprite.pixels[i] = pixels[i];
    else for (int i = 0; i < 16; i++) newSprite.pixels[i] = 0;

    newSprite.localX = x;
    newSprite.localY = y;

    if (!(parent == NULL)) {
        newSprite.globalX = parent->globalX + x;
        newSprite.globalY = parent->globalY + y;
    }

    newSprite.state = state;

    return newSprite;
}

int getColorValue(Uint32 pixel) {
    if (!(pixel ^ COLOR1)) return 0;         // Se o pixel indicar cor 1.
    else if (!(pixel ^ COLOR2)) return 1;    // Se o pixel indicar cor 2.
    else if (!(pixel ^ COLOR3)) return 2;    // Se o pixel indicar cor 3.
    else return 3;                           // Se o pixel indicar cor 4 ou não definido.
}

void renderAllLayers(SDL_Surface * blitSurface, renderLayer * layersToRender, Uint16 * layersToRenderSize) {
    int n;
    for (int i = 0; i < 30; i++) {
        renderCurrentLayer(blitSurface, layersToRender[i], layersToRenderSize[i]);
    }
}

void renderCurrentLayer(SDL_Surface * blitSurface, renderLayer layer, Uint16 currentLayerSize) {
    int n = currentLayerSize;
    if (n == 0) return;

    for (int i = 0; i < n; i++) {
        if (isOnArea(layer[i].globalX, layer[i].globalY, -7, -7, virtualWidth, virtualHeight))
        renderCurrentSprite(blitSurface, layer[i]);
    }
}

void renderCurrentSprite(SDL_Surface * blitSurface,renderSprite sprite) {
    if (isOnArea(sprite.globalX, sprite.globalY, -7, -7, virtualWidth, virtualHeight)) {
        Uint8 pixels[16];
        Uint8 pc, v;
        Sint32 printX, printY;
        renderPalette * palette = (renderPalette*)sprite.palette;
        Uint32 color[4];

        color[0] = palette->color1;
        color[1] = palette->color2;
        color[2] = palette->color3;
        color[3] = palette->color4;

        if (!(sprite.state & SPRITE_STATE_SHOWN)) return;
        
        for (int i = 0; i < 16; i++) pixels[i] = sprite.pixels[i];
    
        pc = 0;
        for (int y = 0; y < 8; y++) for (int x = 0; x < 8; x++) {
            printX = sprite.globalX + x;
            printY = sprite.globalY + y;
            if (isOnArea(printX, printY, 0, 0, virtualWidth, virtualHeight)) {
                v = getByteValue((x%4) * 2, 2, pixels[(y * 2) + x/4]);
                if (color[v] & 0xFF000000) renderCurrentPixel(blitSurface, printX, printY, color[v]);   // Se o pixel não for transparente printar.
            }
        }
    }
}

void renderCurrentPixel(SDL_Surface * blitSurface, Uint16 x, Uint16 y, Uint32 color) {
    Uint8 * target_pixel = (Uint8 *) blitSurface->pixels + y * blitSurface->pitch + x * 4;  // Setando um ponteiro para o pixel desejado
    * ((Uint32*) target_pixel) = color; // Trocando o pixel desejado.
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
    SDL_Surface * renderSurface = SDL_CreateRGBSurfaceWithFormat(0, virtualWidth, virtualHeight, 32, windowSurface->format->format);
    SDL_Rect virtualRect;
    virtualRect.x = 0;
    virtualRect.y = 0;
    virtualRect.h = virtualHeight;
    virtualRect.w = virtualWidth;

    SDL_FillRect(renderSurface, &virtualRect, bgColor);

    if (SDL_MUSTLOCK(renderSurface)) SDL_LockSurface(renderSurface);

    renderCurrentLayer(renderSurface, layers[0], layerSize[0]);
    renderCurrentSprite(renderSurface, defaultRenderSprite);

    if (SDL_MUSTLOCK(renderSurface)) SDL_UnlockSurface(renderSurface);

    SDL_BlitScaled(renderSurface, NULL, windowSurface, &renderRect);
    SDL_UpdateWindowSurface(window);

    SDL_FreeSurface(renderSurface);
}

renderSprite getDefaultSprite() {
    return defaultRenderSprite;
}

void setDefaultSprite(renderSprite sprite) {
    defaultRenderSprite = sprite;
}

bool renderInit() {
    windowWidth = 1024;
    windowHeight = 1024;
    virtualWidth = 128;
    virtualHeight = 128;

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
    defaultPalette.color2 = 0xFF94E344;
    defaultPalette.color3 = 0xFF46878F;
    defaultPalette.color4 = 0xFFE2F3E4;
    bgColor = 0xFF332C50;

    getSpritesFromSheet(87, 16, 16, core, "Sprites/MainSheet.bmp");

    defaultRenderSprite = core[0];
    defaultRenderSprite.globalX = 60;
    defaultRenderSprite.globalY = 60;
    defaultRenderSprite.state = SPRITE_STATE_SHOWN;

    layers[0] = malloc(sizeof(renderSprite) * 8000);
    layerSize[0] = 8000;

    for (int i = 0; i < 8000; i++) {
        layers[0][i] = defaultRenderSprite;
    }

    defaultRenderSprite = core[77];
    defaultRenderSprite.globalX = 60;
    defaultRenderSprite.globalY = 60;


    renderRect.x = 0;   // Colocando os valores do rect usado pra renderizar na janela.
    renderRect.y = 0;
    renderRect.h = windowHeight;
    renderRect.w = windowWidth;

    return true;
}

void renderShut() {
    SDL_DestroyWindow(window);
    window = NULL;

}
