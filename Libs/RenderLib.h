#ifndef __RenderLib_Tro
#define __RenderLib_Tro
#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "MiscLib.h"

#define width 1024
#define height 1024
#define vWidth 256
#define vHeight 192
#define wRatio width/vWidth
#define hRatio height/vHeight

#define COLOR1 0x00000000   // Bandeira da cor 1.
#define COLOR2 0x00FF0000   // Bandeira da cor 2.
#define COLOR3 0x0000FF00   // Bandeira da cor 3.
#define COLOR4 0x000000FF   // Bandeira da cor 4
/**
 * \brief Objeto sprite.
 * 
 * \param pixels Informação dos pixels do sprite. (8x8)
 * \param palette Código da paleta.
 */
typedef struct renderSprite {
    Uint8 pixels[16];
    Uint32 palette; 
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