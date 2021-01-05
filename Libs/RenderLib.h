#ifndef __RenderLib_Tro
#define __RenderLib_Tro

#define OBJ_MASK_RENDER 2
#define RENDER_TYPE_SPRITE 0
#define RENDER_TYPE_LAYER 1
#define RENDER_TYPE_META 2
#define RENDER_TYPE_CAMERA 3


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
    Uint8 state;                // Estado do sprite: 0bXXXXSFRR (X - Para uso futuro, S - Visivel, F - Espelhado, R - Rotacionado).
    Uint8 * pixels;             // Parte visivel do sprite.
    Uint8 layer;                // 255 sendo mais perto da tela, 0 mais distante.
    void * palette;             // Ponteiro para a paleta a ser usada na hora de renderizar o sprite.
    object * obj;               // Objeto renderizador.
    void * meta;                // Metasprite em que está incluído.
} renderSprite;

/**
 * \brief Layer de sprites.
 * 
 * \param size tamanho do layer.
 * \param layer o vetor de ponteiros para os sprites.
 */
typedef struct renderGroup {
    Uint32 size;
    renderSprite ** group;
} renderGroup;

/**
 * \brief Objeto metasprite.
 * 
 * \param object objeto do metasprite.
 * \param meta Layeres que compõem o metasprite.
 * \param metaSize Quantidade de layeres no metasprite.
 * \param state Estado do metasprite.
 */
typedef struct renderMetasprite {
    object * obj;           // Objeto renderizador.
    Uint8 state;            // Estado dos sprites.
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
    object * obj;
} renderCamera;

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
 * \param sheetPath Endereço do spritesheet no sistema.
 * \param spriteCount Quantos sprites você quer que sejam obtidos. -1 é lido como todos os sprites dentro da área.
 * \param stx Número de colunas de sprites.
 * \param sty Número de linhas de sprites.
 * \param x0 Primeiro sprite em X a ser importado. -1 é lido como 0.
 * \param x1 Último sprite em X a ser importado. -1 é lido como stx.
 * \param y0 Primeiro sprite em Y a ser importado. -1 é lido como 0.
 * \param y1 Último sprite em Y a ser importado. -1 é lido como sty.
 * \param followSheetPos Caso seja verdadeiro, os sprites serão posicionados como são na spriteSheet, caso contrário todos os sprites estarão em (0,0).
 * \param spriteLayer Layer em que os sprites serão adicionados.
 * 
 * \return O layer com os sprites importados, ou NULL em caso de erro.
 */
renderGroup * getSpritesFromSheet(char * sheetPath, int spriteCount, int stx, int sty, int x0, int x1, int y0, int y1, bool followSheetPos, Uint8 spriteLayer);

/**
 * \brief Cria um sprite com as propriedades colocadas.
 *
 * \param pixels Imagem do sprite.
 * \param palette Paletta em que o sprite será desenhado.
 * \param parent Parente do sprite. Pode ser NULL.
 * \param x posição x do novo sprite em relação ao parente. Caso o parente seja NULL, será a posição global.
 * \param y posição y do novo sprite em relação ao parente. Caso o parente seja NULL, será a posição global.
 * \param state Forma como o sprite será desenhado. Utilize as flags SPRITE_ para isso, para múltiplas use |.
 * \param laye Layer do sprite.
 * 
 * \return Um novo sprite. Ou false em caso de erro.
 */
renderSprite * createSprite(Uint8 * pixels, renderPalette * palette, object * parent, char * inst, Sint32 x, Sint32 y, Uint8 state, Uint8 layer);

/**
 * \brief Adiciona as informações de pixels no armazém.
 * 
 * \param pixels Sprite que será adicionado ao armazém.
 * 
 * \return O ponteiro para os pixels. Ou NULL em caso de erro.
 */
Uint8 * loadSpritePixels(Uint8 * pixels);

/**
 * \brief Cria um layer com o tamanho dado.
 * 
 * \param size Tamanho do layer.
 * \param x Posição X do objeto do layer.
 * \param y Posição Y do objeto do layer.
 * \param parent Objeto parente, NULL caso não queira parentes.
 * 
 * \return Um render Layer novo. Ou NULL em caso de erro.
 */
renderGroup * createGroup(Uint32 size, Sint32 x, Sint32 y, object * parent, char * inst);

/**
 * \brief Cria um metasprite.
 * 
 * \param x Posição x do objeto metasprite.
 * \param y Posição y do objeto metasprite.
 * \param parent Parente do objeto metasprite.
 * 
 * \return Ponteiro para o metasprite criado, NULL em caso de erros. 
 */
renderMetasprite * createMeta(Sint32 x, Sint32 y, object * parent, char * inst);

/**
 * \brief Adiciona um layer ao metasprite.
 * 
 * \param meta Metasprite que terá o layer adicionado.
 * \param layer Layer que será adicionado ao metasprite.
 * 
 * \return o Metasprite dado, ou NULL em caso de erros.
 */
renderMetasprite * addToMeta(renderMetasprite * meta, renderGroup * group);

/**
 * \brief Função que adiciona um sprite a um layer. NÃO USE ISSO EM LAYERES INCOMPLETOS, SOMENTE EM LAYERES CHEIOS.
 * 
 * \param layer Layer que terá o sprite adicionado.
 * \param sprite Sprite que será adicionado ao layer.
 * \param parent Se verdadeiro, irá realizar a parentagem do objeto sprite e do objeto layer.
 * 
 * \return Layer de entrada, ou NULL em caso de erro.
 */
renderGroup * addToGroup(renderGroup * group, renderSprite * sprite);


/**
 * \brief Cria um metasprite a partir de um spritesheet. Cria somente um layer e novos layeres podem ser anexados posteriormente.
 * 
 * \param path Caminho do metasprite.
 * \param spriteCount Quantidade de sprites a serem importados, lidos da esquerda para direita, de cima pra baixo. Use -1 para importar todos os sprites da área dada.
 * \param stx Quantidade de sprites na sheet em X.
 * \param sty Quantidade de sprites na sheet em Y.
 * \param x0 Primeiro sprite em X da área de importção. -1 é lido como 0.
 * \param x1 Último sprite em X da área de importação. -1 é lido como stx.
 * \param y0 Primeiro sprite em Y da área de importação. -1 é lido comp 0.
 * \param y1 Último sprite em Y da área de importação. -1 é lido como sty.
 * \param x Posição x do objeto metasprite.
 * \param y Posição y do objeto metasprite.
 * \param parent Objeto que será parente do objeto metasprite.
 * \param layer Layer dos sprites dentro do metasprite.
 * 
 * \return Metasprite com os sprites importados no layer 0. Ou NULL em caso de erro.
 */
renderMetasprite * createMetaFromSheet(char * path, int spriteCount, int stx, int sty, int x0, int x1, int y0, int y1, Sint32 x, Sint32 y, object * parent, Uint8 layer);

/**
 * \brief Cria uma paleta nova com as cores dadas.
 * 
 * \param color1 Cor 1 da paleta.
 * \param color2 Cor 2 da paleta.
 * \param color3 Cor 3 da paleta.
 * \param color4 Cor 4 da paleta.
 * 
 * \return O ponteiro para a paleta criada. Ou NULL em caso de erros.
 */
renderPalette * createPalette(Uint32 color1, Uint32 color2, Uint32 color3, Uint32 color4);

/**
 * \brief Cria uma paleta vazia. 
 * 
 * \return O ponteiro para a paleta criada. Ou NULL em caso de erros.
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


object * cloneSprite(object * source, char * inst);

object * cloneMeta(object * source, char * inst);


/**
 * \brief Adiciona um sprite ao layer de renderização.
 * 
 * \param sprite Novo sprite a ser adicionado.
 * 
 * \return True, ou false em caso de erros.
 */
bool addSpriteToRender(renderSprite * sprite);

/**
 * \brief Adiciona todos os sprites do metasprite ao layer de renderização.
 * 
 * \param meta Metasprite que terá todos os seus sprites renderizados.
 * 
 * \return True, ou false em caso de erro.
 */
bool addMetaToRender(renderMetasprite * meta);

/**
 * \brief Limpa o layer de renderização, apagando todos os seus sprites.
 * 
 * \return True, ou false em caso de erro.
 */
bool clearRenderLayer();

/**
 * \brief Libera todos os sprites do layer dado.
 * 
 * \param layer Layer que terá seus sprites liberados.
 * 
 * \return True, ou false em caso de erros.
 */
bool clearLayer(renderGroup * group);

/**
 * \brief Libera o sprite.
 * 
 * \param sprite Sprite a ser Liberado
 */
bool freeSprite(renderSprite * sprite);

/**
 * \brief Libera a memória de um layer.
 * 
 * \param layer Layer que será liberado.
 * \param removeSprites Caso verdadeiro, todos os sprites do layer serão liberados.
 * 
 * \return True, ou false em caso de erro.
 */
bool freeGroup(renderGroup * group, bool removeSprites);

/**
 * \brief Libera a memória de um metasprite.
 * 
 * \param removeLayer Caso verdadeiro, irá liberar todos os layeres do metasprite.
 * \param removeSprites Caso verdadeiro, irá liberar todos os sprites dos layeres. Não irá fazer nada caso removeLayer seja falso.
 * 
 * \return True, ou false em caso de erro.
 */
bool freeMeta(renderMetasprite * meta);

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
 * \param blitSurface Superfície em que os sprites serão renderizados.
 * \param sprite Sprite a ser renderizado.
 */
void renderCurrentSprite(SDL_Surface * blitSurface,renderSprite * sprite);

/**
 * \brief Atualiza todos os sistemas da RenderLib, deve ser chamada uma vez por frame.
 */
void renderUpdate();

/**
 * \brief Função getter do layer Core. 
 * 
 * \return Layer Core. 
 */
renderGroup * getCore();

/**
 * \brief Função que retorna o estado da janela.
 * 
 * \return o estado da janela.
 */
renderWindow * getWindowState();

Uint32 getBgColor();

void setBgColor(Uint32 newColor);

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