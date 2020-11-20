#ifndef __RenderLib_Tro
#define __RenderLib_Tro
#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "MiscLib.h"

#define SPRITE_STATE_SHOWN 0b00001000
#define SPRITE_STATE_MIRRORED 0b00000100
#define SPRITE_STATE_ROTATED_90A 0b00000001
#define SPRITE_STATE_ROTATED_180A 0b00000010
#define SPRITE_STATE_ROTATED_270A 0b00000011
#define SPRITE_STATE_ROTATED_90C SPRITE_STATE_ROTATED_270A
#define SPRITE_STATE_ROTATED_180C SPRITE_STATE_ROTATED_180A
#define SPRITE_STATE_ROTATED_270C SPRITE_STATE_ROTATED_90A

#define SPRITE_STATE_3h00 0b00000000
#define SPRITE_STATE_6h15 SPRITE_STATE_ROTATED_90C
#define SPRITE_STATE_9h30 SPRITE_STATE_ROTATED_180C
#define SPRITE_STATE_12h45 SPRITE_STATE_ROTATED_270C
#define SPRITE_STATE_9h00 SPRITE_STATE_MIRRORED | SPRITE_STATE_3h00
#define SPRITE_STATE_12h15 SPRITE_STATE_MIRRORED | SPRITE_STATE_6h15
#define SPRITE_STATE_3h30 SPRITE_STATE_MIRRORED | SPRITE_STATE_9h30
#define SPRITE_STATE_6h45 SPRITE_STATE_MIRRORED | SPRITE_STATE_12h45

#define COLOR1 0x00000000   // Bandeira da cor 1.
#define COLOR2 0x00FF0000   // Bandeira da cor 2.
#define COLOR3 0x0000FF00   // Bandeira da cor 3.
#define COLOR4 0x000000FF   // Bandeira da cor 4

/**
 * \brief Objeto sprite.
 * 
 * \param localX posição x em relação ao parente.
 * \param localY posição y em relação ao parente.
 * \param globalX posição x em relação à tela.
 * \param globalY posição y em relação à tela.
 * \param state Como a imagem será apresentada (A mostra, espelhado e girado)
 * \param pixels Informação dos pixels do sprite. (8x8)
 * \param palette ponteiro para a paleta.
 * \param parent ponteiro para o parente.
 */
typedef struct renderSprite {
    Uint16 localX;
    Uint16 localY;
    Uint16 globalX;
    Uint16 globalY;
    Uint8 state;                    //0bXXXXSFRR XXXX - for future use. S - Shown, f - flipped, rotated
    Uint8 pixels[16];
    void * palette; 
    void * parent;
} renderSprite;

/**
 * \brief Objeto paleta
 * 
 * \param color1 Cor representada por 0 0.
 * \param color2 Cor representada por 0 1.
 * \param color3 Cor representada por 1 0.
 * \param color4 Cor representada por 1 1.
 */
typedef struct renderPalette {
    Uint32 color1;
    Uint32 color2;
    Uint32 color3;
    Uint32 color4;
} renderPalette;

typedef renderSprite * renderLayer;

/**
 * \brief Esta função carrega uma superfície para o programa.
 * 
 * \param path Caminho para a imagem a ser carregada, em BMP.
 * 
 * \return Uma superfície com a imagem em path carregada.
 */
SDL_Surface * loadSurface(char * path);

/**
 * \brief Esta função obtem sprites 8x8 dentro de uma imagem e os guarda em um vetor.
 * 
 * \param spriteCount Quantos sprites você quer que sejam obtidos.
 * \param x Número de colunas de sprites.
 * \param y Número de linhas de sprites.
 * \param spriteArray Vetor em que os sprites serão guardados.
 * \param sheetPath Endereço do spritesheet no sistema.
 * 
 * \return True caso a função tenha sido bem sucedida, False caso contrário.
 * 
 */
bool getSpritesFromSheet(int spriteCount, int x, int y, renderSprite * spriteArray, char * sheetPath);

/**
 * \brief Recebe um pixel e retorna o valor que o bit representa.
 * 
 * \param pixel Pixel a ter seu valor determinado.
 * 
 * \return Valor do pixel. 
 */
int getColorValue(Uint32 pixel);

/**
 * \brief Cria um sprite com as propriedades colocadas.
 * 
 * \param pixels Imagem do sprite.
 * \param palette Paletta em que o sprite será desenhado.
 * \param parent Parente do sprite. Pode ser NULL.
 * \param x posição x do novo sprite em relação ao parente. Caso o parente seja NULL, será a posição global.
 * \param y posição y do novo sprite em relação ao parente. Caso o parente seja NULL, será a posição global.
 * \param state Forma como o sprite será desenhado. Utilize as flags SPRITE_ para isso, para múltiplas use |.
 * 
 * \return Um novo sprite.
 */
renderSprite createSprite(Uint8 pixels[16], renderPalette * palette, renderSprite * parent, Uint16 x, Uint16 y, Uint8 state);

/**
 * \brief Uma função de degug, pra printar um sprite no console.
 * 
 * \param sprite Sprite a ser printado no console.
 */
void IOSpritePrint(renderSprite sprite);

/**
 * \brief Renderiza os sprites dentro do layer. LEMBRE DE TRANCAR A SUPERFÍCIE ANTES DE USAR!
 * 
 * \param layer Layer cujos sprites serão renderizados.
 */
void renderCurrentLayer(SDL_Surface * blitSurface, renderLayer layer);

/**
 * \brief Renderiza o sprite. LEMBRE DE TRANCAR A SUPERFÍCIE ANTES DE USAR!
 * 
 * \param sprite Sprite a ser renderizado.
 */
void renderCurrentSprite(SDL_Surface * blitSurface,renderSprite sprite);

/**
 * \brief Coloca um pixel de determinada cor na posição (x, y) da superfície dada.
 * 
 * \param blitSurface Superfície que terá o pixel desenhado.
 * \param x Posição x do pixel.
 * \param y Posição y do pixel.
 * \param color Cor do pixel.
 */
void renderCurrentPixel(SDL_Surface * blitSurface, Uint16 x, Uint16 y, Uint32 color);

/**
 * \brief Atualiza todos os sistemas da RenderLib, deve ser chamada uma vez por frame.
 * 
 */
void renderUpdate();

renderSprite getDefaultSprite();

void setDefaultSprite(renderSprite sprite);

/**
 * \brief Função que inicializa os sistemas de imagem e janela do SDL.
 * 
 * \param window Ponteiro para a janela principal.
 * \param renderer Ponteiro para o renderizador.
 * 
 * \return true, caso tudo se inicie corretamente; false, caso contrário.  
 */
bool renderInit();

/**
 * \brief Finaliza todos os sistemas de imagem e janela do SDL.
 * 
 * \param window Ponteiro para a janela principal.
 * \param renderer Ponteiro para o renderizador principal.
 */
void renderShut();

#endif