#ifndef __RenderLib_Tro
#define __RenderLib_Tro

#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "MiscLib.h"
#include "RLCSDef.h"
#include "Core.h"

#define SPRITE_STATE_SHOWN 0b00001000                                       // Bandeira que quando habilitada, faz com que o sprite seja renderizado.
#define SPRITE_STATE_MIRRORED 0b00000100                                    // Bandeira que quando habilitada, renderiza o sprite espelhado.
#define SPRITE_STATE_ROTATED_90A 0b00000001                                 // Bandeira que quando habilitada, renderiza o sprite rotacionado em 90 graus no sentido anti-horário.
#define SPRITE_STATE_ROTATED_180A 0b00000010                                // Bandeira que quando habilitada, renderiza o sprite rotacionado em 180 graus no sentido anti-horário.                               
#define SPRITE_STATE_ROTATED_270A 0b00000011                                // Bandeira que quando habilitada, renderiza o sprite rotacionado em 270 graus no sentido anti-horário.
#define SPRITE_STATE_ROTATED_90C SPRITE_STATE_ROTATED_270A                  // Bandeira que quando habilitada, renderiza o sprite rotacionado em 90 graus no sentido horário.
#define SPRITE_STATE_ROTATED_180C SPRITE_STATE_ROTATED_180A                 // Bandeira que quando habilitada, renderiza o sprite rotacionado em 180 graus no sentido horário.
#define SPRITE_STATE_ROTATED_270C SPRITE_STATE_ROTATED_90A                  // Bandeira que quando habilitada, renderiza o sprite rotacionado em 270 graus no sentido horário.

#define SPRITE_STATE_3h00 0b00000000
#define SPRITE_STATE_6h15 SPRITE_STATE_ROTATED_90C
#define SPRITE_STATE_9h30 SPRITE_STATE_ROTATED_180C
#define SPRITE_STATE_12h45 SPRITE_STATE_ROTATED_270C
#define SPRITE_STATE_9h00 (SPRITE_STATE_MIRRORED | SPRITE_STATE_3h00)
#define SPRITE_STATE_12h15 (SPRITE_STATE_MIRRORED | SPRITE_STATE_6h15)
#define SPRITE_STATE_3h30 (SPRITE_STATE_MIRRORED | SPRITE_STATE_9h30)
#define SPRITE_STATE_6h45 (SPRITE_STATE_MIRRORED | SPRITE_STATE_12h45)



#define COLOR1 0x00000000   // Bandeira da cor 1.
#define COLOR2 0x00FF0000   // Bandeira da cor 2.
#define COLOR3 0x0000FF00   // Bandeira da cor 3.
#define COLOR4 0x000000FF   // Bandeira da cor 4



/**
 * \brief Objeto sprite.
 * 
 * \param name nome do sprite.
 * \param state Como a imagem será apresentada (A mostra, espelhado e girado)
 * \param pixels Informação dos pixels do sprite. (8x8)
 * \param palette ponteiro para a paleta.
 * \param object objeto do sprite.
 */
typedef struct renderSprite {
    Uint8 state;                // Estado do sprite: 0bXXXXSFRR X - Para uso futuro, S - Visivel, F - Espelhado, R - Rotacionado.
    Uint8 pixels[16];           // Parte visivel do sprite.
    void * palette;             // Ponteiro para a paleta a ser usada na hora de renderizar o sprite.
    object * object;        // Objeto renderizador.
} renderSprite;

/**
 * \brief Objeto metasprite.
 * 
 * \param name nome do metasprite.
 * \param object objeto do metasprite.
 */
typedef struct renderMetasprite {
    object * object;        // Objeto renderizador.
} renderMetasprite;

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
 * \brief Objeto câmera.
 * 
 * \param object objeto da câmera.
 */
typedef struct renderCamera {
    object * object;
} renderCamera;

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
#define getColorValue(pixel) (((!(pixel ^ COLOR1)) || (!(pixel ^ COLOR2)) ) ? ((!(pixel ^ COLOR1)) ? 0 : 1) : ((!(pixel ^ COLOR3)) ? 2 : 3))

/**
 * \brief Cria um sprite com as propriedades colocadas.
 * 
 * \param name Nome do sprite.
 * \param pixels Imagem do sprite.
 * \param palette Paletta em que o sprite será desenhado.
 * \param parent Parente do sprite. Pode ser NULL.
 * \param x posição x do novo sprite em relação ao parente. Caso o parente seja NULL, será a posição global.
 * \param y posição y do novo sprite em relação ao parente. Caso o parente seja NULL, será a posição global.
 * \param state Forma como o sprite será desenhado. Utilize as flags SPRITE_ para isso, para múltiplas use |.
 * 
 * \return Um novo sprite.
 */
renderSprite createSprite(Uint8 pixels[16], renderPalette * palette, object * parent, Sint32 x, Sint32 y, Uint8 state);

/**
 * \brief Função que cria um novo metasprite.
 * 
 * \param name Nome do novo metasprite.
 * \param sprites vetor com os sprites que serão usados no metasprite.
 * \param pos vetor da construção do metasprite.
 * \param spriteCount número de sprites no vetor "sprites".
 * \param metaSizeX tamanho X do metasprite, em sprites. 
 * \param metaSizeY tamanho Y do metasprite, em sprites.
 * \param parent Objeto que será associado ao novo metasprite como parente.
 * \param x posição x local do metasprite.
 * \param y posição y local do metasprite.
 */
renderMetasprite createMetasprite(renderSprite * sprites, Sint32 * pos, Sint32 spriteCount, Sint32 metaSizeX, Sint32 metaSizeY, object * parent, Sint32 x, Sint32 y);

void changeCamera(renderCamera * newCamera);

renderCamera * getDefaultCamera();

/**
 * \brief Adiciona um sprite ao layer de número dado.
 * 
 * \param sprite Novo sprite a ser adicionado.
 * \param targetLayer Número do layer em que o sprite novo será adicionado.
 */
void addSpriteToLayer(renderSprite sprite, Uint8 targetLayer);

/**
 * \brief Limpa um layer, apagando todos os seus sprites.
 * 
 * \param targetLayer Número do layer que será limpo.
 */
void clearLayer(Uint8 targetLayer);

/**
 * \brief Uma função de degug, pra printar um sprite no console.
 * 
 * \param sprite Sprite a ser printado no console.
 */
void IOSpritePrint(renderSprite sprite);

/**
 * 
 */
void renderAllLayers(SDL_Surface * blitSurface, renderLayer * layersToRender, Uint32 * layersToRenderSize);

/**
 * \brief Renderiza os sprites dentro do layer. LEMBRE DE TRANCAR A SUPERFÍCIE ANTES DE USAR!
 * 
 * \param layer Layer cujos sprites serão renderizados.
 * \param currentLayerSize Número de sprites no layer.
 */
void renderCurrentLayer(SDL_Surface * blitSurface, renderLayer layer, Uint32 currentLayerSize);

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
#define renderCurrentPixel(blitSurface,  x,  y, color) *(Uint32*)((Uint8*) blitSurface->pixels + y * blitSurface->pitch + x * 4) = color

/**
 * \brief Atualiza todos os sistemas da RenderLib, deve ser chamada uma vez por frame.
 * 
 */
void renderUpdate();

/**
 * \brief Função getter do defaultSprite.
 * 
 * \return uma cópia do defaultSprite.
 */
renderSprite * getDefaultSprite();

/**
 * \brief Função getter do layer Core. 
 * 
 * \return Layer Core. 
 */
renderSprite * getCoreSprites();

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