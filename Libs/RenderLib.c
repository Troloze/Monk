#include "RenderLib.h"

static SDL_Window * window;
static SDL_Surface * windowSurface;
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
    newSprite.palette = 0;

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
        for (int i1 = 0; i1 < 16; i1++) newSprite.pixels[i1] = newPixels[i1];       // Transferindo os valores da imagem carregada para o novo sprite.
        spriteArray[scount] = newSprite;                                            // Adicionando o novo sprite ao array dado.
        scount++;
        if (spriteCount == scount) break;
    }
    if (SDL_MUSTLOCK(sheet)) SDL_UnlockSurface(sheet);    // Caso a superfície precise fechar, ela foi fechada anteriormente, e agora será aberta.
    return true;
}

int getColorValue(Uint32 pixel) {
    if (!(pixel & ~COLOR1)) return 0;         // Se o pixel indicar cor 1.
    else if (!(pixel & ~COLOR2)) return 1;    // Se o pixel indicar cor 2.
    else if (!(pixel & ~COLOR3)) return 2;    // Se o pixel indicar cor 3.
    else return 3;                            // Se o pixel indicar cor 4 ou não definido.
}

bool renderInit() {
    window = SDL_CreateWindow(
        "Troloze's Amazing Adventure(TM)",
        SDL_WINDOWPOS_UNDEFINED,
        SDL_WINDOWPOS_UNDEFINED,
        width,
        height,
        SDL_WINDOW_SHOWN
    );  // Criar janela.
    if(window == NULL) {    // Checar se ela foi criada com sucesso.
        printf("Janela não pôde ser criada! Erro: %s\n", SDL_GetError());
        return false;
    }

    windowSurface = SDL_GetWindowSurface(window);

    defaultPalette.color1 = 0x00000000;
    defaultPalette.color2 = 0x00FF0000;
    defaultPalette.color3 = 0x0000FF00;
    defaultPalette.color4 = 0x000000FF;

    getSpritesFromSheet(87, 16, 16, core, "Sprites/MainSheet.bmp");

    return true;
}

void renderShut() {
    SDL_DestroyWindow(window);
    window = NULL;

}
