#ifndef __CORE_TRO
#define __CORE_TRO

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

#define OBJ_MSG_DEFAULT 0
#define OBJ_MSG_DESTROY 1

#define OBJ_MASK_OBJ 0          // Máscara do tipo.
#define OBJ_TYPE_UNDEFINED 0    // Para objetos que se desassociaram ao item.
#define OBJ_TYPE_OBJECT 1       // Para objetos sem nenhuma associação.


typedef struct object {
    Uint8 type, mask;           // Tipo de item que está associado ao objeto.
    void * item;                // Item que está associado ao objeto.

    Sint32 localX;              // Posição X em relação ao parente.
    Sint32 localY;              // Posição Y em relação ao parente.

    Sint32 globalX;             // Posição X global.
    Sint32 globalY;             // Posição X global.

    Uint32 parentPos;           // Posição do objeto entre os filiais do parente.
    void * parent;              // Ponteiro para o objeto parente.
    Uint32 childrenCount;       // Número de objetos filiados.
    void ** children;           // Ponteiros para todos os objetos filiados.

    char * instance;            // Instância do objeto.
    bool isUpdated;             // Se o objeto foi atualizado no frame.
    bool isInst;

    Uint64 tick;                // Quantidade de frames que se passaram desde que o objeto foi criado

    bool (*start)(void * self); // Função que será chamada quando o objeto foi instanciada.
    bool (*update)(void * self);// Função que será chamada a todo frame.
    bool (*end)(void * self);   // Função que será chamada quando o objeto for destruído.
} object;

#include "InputLib.h"
#include "RenderLib.h"
#include "AudioLib.h"
#include "ColorLib.h"
#include "FileLib.h"
#include "ErrorLib.h"
#include "GameLib.h"

/**
 * \brief Cria um novo objeto.
 * 
 * \param x posição X do objeto.
 * \param y posição y do objeto.
 * \param item Ponteiro para o item cujo objeto será criado.
 * \param inst String com o ID de instanciamento.
 * 
 * \return Ponteiro para o novo objeto.
 */
object * createObject(Sint32 x, Sint32 y, Uint8 mask, Uint8 type, void * item, char * inst);

/**
 * \brief Função que associa funções à um objeto.
 * 
 * \param obj Objeto que terá as funções associadas.
 * \param start Função que será chamada quando o objeto for instanciado.
 * \param update Função que será chamada a todo frame.
 * \param end Função que será chamada quando o objeto for destruído.
 */
void addFuncToObj(object * obj, bool (*start)(void * self), bool (*update)(void * self), bool (*end)(void * self));

/**
 * \brief Função que associa um ID a um objeto e todos os seus filhos.
 * 
 * \param source Objeto que terá a ID associada.
 * \param instanceID String com o ID de instanciamento.
 */
void setObjectInstanceID(object * source, char * instanceID);

/**
 * \brief Retorna o ponteiro para root.
 * 
 * \return O ponteiro para root.
 */
object * getRoot();

/**
 * \brief Função que instancia um objeto.
 * 
 * \param toInstance Objeto que será instanciado.
 * \param x Posição X do objeto a ser instanciado.
 * \param y posição Y do objeto a ser instanciado.
 * 
 * \return Novo objeto.
 */
object * instanceObject(object * toInstance, Sint32 x, Sint32 y);

/**
 * \brief Função que destrói um objeto.
 * 
 * \param targetObject objeto que será destruido. 
 */
void destroyObject(object * targetObject);

/**
 * \brief Realiza a parentagem entre os objectos
 * 
 * \param parent objeto que será o parente.
 * \param child objeto que será o filiado.
 * \param keepGlobalPosition caso verdadeiro, altera a posição local do objeto para que ele mantenha sua posição global.
 */
void parentObject(object * parent, object * child, bool keepGlobalPosition);

/**
 * \brief Realiza a dessociação do objeto com seu parente. NÃO USE ISSO A MENOS QUE SAIBA O QUE ESTÁ FAZENDO, Para desassociar um parente normalmente use parentObject(NULL, child, ...);
 * 
 * \param child objeto que será deparentado.
 * \param keepGlobalPosition caso verdadeiro, altera a posição local do objeto para que ele mantenha sua posição global.
 */
void unparentObject(object * child, bool keepGlobalPosition);

/**
 * \brief Atualiza todos os objetos.
 */
void coreUpdate();

/**
 * \brief Função que faz com que o objeto rode a função update associada a si.
 * 
 * \param target Objeto a ser atualizado.
 * 
 * \return true, ou false em caso de erros. 
 */
bool runObjectUpdate(object * target);

/**
 * \brief Atualiza a posição de um objeto em específico.
 * 
 * \param targetObject objeto que terá a posição atualizada.
 */
void updateObjectPosition(object * targetObject);

/**
 * \brief Inicializa o sistema de núcleo.
 */
bool coreInit();

/**
 * \brief Finaliza os sistemas de núcleo.
 */
void coreShut();

#endif