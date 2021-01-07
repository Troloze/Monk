#ifndef __GameLib_Tro
#define __GameLib_Tro

#include "SDL2/SDL.h"
#include "Core.h"
#include "../GameImport.h"

#define OBJ_MASK_GAME 3
#define GAME_TYPE_AREA 0

#define AREA_NO_MASK 0
#define AREA_L_MASK(x) (1 << (x))

#define AREA_NO_CONTACT 0
#define AREA_ON_ENTER 1
#define AREA_OVERLAP 2
#define AREA_ON_EXIT 3

typedef struct gameArea {
    object * obj;
    char * tag;
    
    Uint8 width, height;
    
    Uint8 layer;        // Max 64;
    Uint64 layerMask;   // To check collision with.
    
    Uint32 pos;
    
    bool active;
} gameArea;

typedef struct gameOverlapFunc {
    bool (*onEnter)(void * self, void * overlap);
    bool (*onOverlap)(void * self, void * overlap);
    bool (*onExit)(void * self, void * overlap);
} gameOverlapFunc;

/**
 * \brief Função que cria uma área.
 * 
 * \param w Largura da área.
 * \param h Tamanho da área.
 * \param layer Layer da área.
 * \param layerMask Máscara da área.
 * \param parent Objeto parente (pode ser NULL).
 * \param instance ID de instanciamento da área (pode ser NULL).
 */
gameArea * createArea(Uint16 w, Uint16 h, Uint8 layer, Uint64 layerMask, object * parent, char * instance);

/**
 * \brief Função que adiciona a área pro sistema de detecção de colisão.
 * 
 * \return NULL em caso de erro.
 */
gameArea * addAreaToDump(gameArea * toDump);

/**
 * \brief Função que libera uma área.
 * 
 * \param area Área que será liberada.
 * 
 * \return true, ou false em caso de erro.
 */
bool freeArea(gameArea * area);

/**
 * \brief Função que irá clonar a área, quando instanciada.
 * 
 * \param source objeto da área a ser clonada.
 * \param inst ID que estará associada à nova área.
 * 
 * \return Objeto da nova área. 
 */
object * cloneArea(object * source, char * inst);

/**
 * \brief Função que irá atualizar o estado de colisão das áreas e chamar as funções de colisão.
 * 
 * \return true, ou false em caso de erro.
 */
bool updateArea();

/**
 * \brief Função de debug que printa o overlap.
 */
void printOverlap();

/**
 * \brief Função que associa uma colisão entre duas IDs com as funções de colisão.
 * 
 * \param inst1 ID da área sensível.
 * \param inst2 ID da área alvo.
 * \param onEnter Função que será chamada quando inst1 detectar inst2.
 * \param onOverlap Função que será chamada quando inst1 manter contato com inst2.
 * \param onExit Função que será chamada quando inst1 perder contato com inst2.
 * 
 * \return true, ou false em caso de erros.
 */
bool addFuncToInstOverlap(char * inst1, char * inst2, bool (*onEnter)(void * self, void * overlap), bool (*onOverlap)(void * self, void * overlap), bool (*onExit)(void * self, void * overlap));

/**
 * \brief Função de atualização do sistema.
 * 
 * \return true, ou false em caso de erro.
 */
bool gameUpdate();

/**
 * \brief Função de inicialização do sistema.
 * 
 * \return true, ou false em caso de erro. 
 */
bool gameInit();

/**
 * \brief Função de finalização do sistema.
 * 
 * \return true.
 */
bool gameShut();

#endif