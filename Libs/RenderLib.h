#ifndef __RenderLib_Tro
#define __RenderLib_Tro

#define OBJ_MASK_RENDER 2
#define RENDER_TYPE_SPRITE 0
#define RENDER_TYPE_META 1
#define RENDER_TYPE_CAMERA 2

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

#define COLOR1 0xFF000000   // Bandeira da cor 1.
#define COLOR2 0xFFFF0000   // Bandeira da cor 2.
#define COLOR3 0xFF00FF00   // Bandeira da cor 3.
#define COLOR4 0xFF0000FF   // Bandeira da cor 4

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
    Uint8 * pixels;             // Parte visivel do sprite.
    Uint8 layer;                // 255 sendo mais perto da tela, 0 mais distante.
    void * palette;             // Ponteiro para a paleta a ser usada na hora de renderizar o sprite.
    object * object;            // Objeto renderizador.
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
 * \brief Objeto estado de janela
 * 
 * \param windowHeight Altura da janela, em pixels.
 * \param windowWidth Largura da janela, em pixels.
 * \param virtualHeight Altura da simulação, em pixels.
 * \param virtualWidth Largura da simulação, em pixels.
 */
typedef struct renderWindow {
    Uint16 windowHeight;    // Altura da janela.
    Uint16 windowWidth;     // Largura da janela.
    Uint16 virtualHeight;   // Altura da simulação.
    Uint16 virtualWidth;    // Largura da simulação.
} renderWindow;

/**
 * \brief Objeto câmera.
 * 
 * \param object objeto da câmera.
 */
typedef struct renderCamera {
    object * object;
} renderCamera;

typedef struct renderLayer {
    Uint32 size;
    renderSprite ** layer;
} renderLayer;

/**
 * \brief Recebe um pixel e retorna o valor que o bit representa.
 * 
 * \param pixel Pixel a ter seu valor determinado.
 * 
 * \return Valor do pixel. 
 */
#define getColorValue(pixel) (((!(pixel ^ COLOR1)) || (!(pixel ^ COLOR2)) ) ? ((!(pixel ^ COLOR1)) ? 0 : 1) : ((!(pixel ^ COLOR3)) ? 2 : 3))

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
 * \brief Macro que retorna a razão entre as larguras da simulação e da janela.
 */
#define getWindowRatioX() ((double) getWindowState()->virtualWidth / getWindowState()->windowWidth)

/**
 * \brief Macro que retorna a razão entre as alturas da simulação e da janela.
 */
#define getWindowRatioY() ((double) getWindowState()->virtualHeight / getWindowState()->windowHeight)

/**
 * \brief Esta função carrega uma superfície para o programa.
 * 
 * \param path Caminho para a imagem a ser carregada, em BMP.
 * 
 * \return Uma superfície com a imagem em path carregada.
 */
SDL_Surface * loadSurface(char * path);

/**
 * \brief Esta função obtem sprites 8x8 dentro de uma imagem, já com suas paletas, e os guarda em um vetor.
 * 
 * \param spriteCount Quantos sprites você quer que sejam obtidos.
 * \param stx Número de colunas de sprites.
 * \param sty Número de linhas de sprites.
 * \param spriteArray Vetor em que os sprites serão guardados.
 * \param sheetPath Endereço do spritesheet no sistema.
 * 
 * \return True caso a função tenha sido bem sucedida, False caso contrário.
 */
renderLayer * getSpritesFromSheetPalette(int spriteCount, int stx, int sty, char * sheetPath);

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
renderSprite * createSprite(Uint8 * pixels, renderPalette * palette, object * parent, Sint32 x, Sint32 y, Uint8 state, Uint8 layer);

/**
 * \brief Adiciona as informações de pixels no armazém.
 * 
 * \param pixels Sprite que será adicionado ao armazém.
 */
Uint8 * loadSpritePixels(Uint8 * pixels);

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
 *
renderMetasprite createMetasprite(renderSprite * sprites, Sint32 * pos, Sint32 spriteCount, Sint32 metaSizeX, Sint32 metaSizeY, object * parent, Sint32 x, Sint32 y);

/**
 * \brief Cria um novo metasprite usando uma sheet.
 * 
 * \param sheetPath caminho para o arquivo da sheet.
 * \param pathX0 Primeiro sprite do metasprite na sheet, em X.
 * \param pathY0 Primeiro sprite do metasprite na sheet, em Y.
 * \param metaWidth Número de sprites em X do metasprite.
 * \param metaHeight Número de sprites em Y do metasprite.
 * \param palette Ponteiro para paleta global do metasprite.
 * \param parent Objeto que terá o objeto do metasprite como parente.
 * \param x Posição local X do metasprite.
 * \param y Posição local Y do metasprite.
 * 
 * \return um metasprite com seus componentes afiliados.
 *
renderMetasprite createMetaspriteFromSheet(char * sheetPath ,Uint16 pathX0, Uint16 pathY0, Uint16 metaWidth, Uint16 metaHeight, renderPalette * palette, object * parent, Sint32 x, Sint32 y);

/**
 * \brief Cria um novo metasprite já colorido usando uma sheet.
 * 
 * \param sheetPath caminho para o arquivo da sheet.
 * \param pathX0 Primeiro sprite do metasprite na sheet, em X.
 * \param pathY0 Primeiro sprite do metasprite na sheet, em Y.
 * \param metaWidth Número de sprites em X do metasprite.
 * \param metaHeight Número de sprites em Y do metasprite.
 * \param parent Objeto que terá o objeto do metasprite como parente.
 * \param x Posição local X do metasprite.
 * \param y Posição local Y do metasprite.
 * 
 * \return um metasprite com seus componentes afiliados.
 *
renderMetasprite createMetaspriteFromSheetPalette(char * sheetPath, Uint16 pathX0, Uint16 pathY0, Uint16 metaWidth, Uint16 metaHeight, object * parent, Sint32 x, Sint32 y);
//*/

/**
 * \brief Cria uma paleta nova com as cores dadas.
 * 
 * \param color1 Cor 1 da paleta.
 * \param color2 Cor 2 da paleta.
 * \param color3 Cor 3 da paleta.
 * \param color4 Cor 4 da paleta.
 * 
 * \return O ponteiro para a paleta criada.
 */

renderPalette * createPalette(Uint32 color1, Uint32 color2, Uint32 color3, Uint32 color4);

/**
 * \brief Cria uma paleta vazia. 
 * 
 * \return O ponteiro para a paleta criada.
 */
renderPalette * createEmptyPalette();

/**
 * \brief Muda a orientação do renderizador para a nova câmera, enquanto mantêm os objetos associados à tela em suas posições normais
 * 
 * \param newCamera Ponteiro para a nova câmera de orientação.
 */
void changeCamera(renderCamera * newCamera);

/**
 * \brief Obtem o ponteiro para a câmera básica.
 * 
 * \return O ponteiro para a câmera básica.
 */
renderCamera * getDefaultCamera();

/**
 * \brief Adiciona um sprite ao layer de renderização.
 * 
 * \param sprite Novo sprite a ser adicionado.
 */
bool addSpriteToRender(renderSprite * sprite);

/**
 * \brief Limpa o layer de renderização, apagando todos os seus sprites.
 */
bool clearLayer();

/**
 * \brief Libera todos os sprites do layer dado.
 * 
 * \param layer Layer que terá seus sprites liberados.
 */
bool clearSpriteLayer(renderLayer * layer);

/**
 * \brief Libera o sprite.
 * 
 * \param sprite Sprite a ser Liberado
 */
bool clearSprite(renderSprite * sprite);

/**
 * \brief Libera todos os sprites carregados no armazém.
 */
bool cleanStorage();

/**
 * \brief Uma função de degug, pra printar um sprite no console.
 * 
 * \param sprite Sprite a ser printado no console.
 */
void IOSpritePrint(renderSprite * sprite);

/**
 * \brief Renderiza os sprites. LEMBRE DE TRANCAR A SUPERFÍCIE ANTES DE USAR!
 * 
 * \param blitSurface Superfície em que os sprites serão desenhados.
 */
void renderFrame(SDL_Surface * blitSurface);

/**
 * \brief Renderiza o sprite. LEMBRE DE TRANCAR A SUPERFÍCIE ANTES DE USAR!
 * 
 * \param sprite Sprite a ser renderizado.
 */
void renderCurrentSprite(SDL_Surface * blitSurface,renderSprite * sprite);

/**
 * \brief Atualiza todos os sistemas da RenderLib, deve ser chamada uma vez por frame.
 * 
 */
void renderUpdate();

/**
 * \brief Função getter do layer Core. 
 * 
 * \return Layer Core. 
 */
renderLayer * getCoreSprites();

/**
 * \brief Função que retorna o estado da janela.
 * 
 * \return o estado da janela.
 */
renderWindow * getWindowState();

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