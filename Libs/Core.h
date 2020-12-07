#ifndef __CORE_TRO
#define __CORE_TRO

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>


/**
 * \brief Objeto render.
 * 
 * \param localX posição x em relação ao parente.
 * \param localY posição y em relação ao parente.
 * \param globalX posição x em relação à tela.
 * \param globalY posição y em relação à tela.
 * \param parentType guarda o tipo de objeto do parente.
 * \param parent ponteiro para o parente.
 * \param childrenCount número de objetos filiados.
 * \param children ponteiros para todos os objetos filiados.
 * \param message mensagem que pode ser lida pelo objeto associado.
 * \param arguments argumentos da mensagem.
 */
typedef struct object {
    Sint32 localX;              // Posição X em relação ao parente.
    Sint32 localY;              // Posição Y em relação ao parente.
    double localRot;            // Rotação em relação ao parente.
    Sint32 globalX;             // Posição X global.
    Sint32 globalY;             // Posição X global.
    double globalRot;           // Rotação global
    Uint32 parentPos;           // Posição do objeto entre os filiais do parente.
    void * parent;              // Ponteiro para o objeto parente.
    Uint32 childrenCount;       // Número de objetos filiados.
    void ** children;           // Ponteiros para todos os objetos filiados.
    Uint16 message;             // Mensagem para o item associado ao objeto.
    void * arguments;           // Argumentos da mensagem.
    bool isUpdated;             // Se o objeto foi atualizado no frame.
} object;

/**
 * \brief Cria um novo objeto.
 * 
 * \param x posição X do objeto.
 * \param y posição y do objeto.
 * 
 * \return Ponteiro para o novo objeto.
 */
object * createObject(Sint32 x, Sint32 y);

/**
 * \brief Retorna o ponteiro para root.
 * 
 * \return O ponteiro para root.
 */
object * getRoot();

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
 * \brief Realiza a dessociação do objeto com seu parente.
 * 
 * \param child objeto que será deparentado.
 * \param keepGlobalPosition caso verdadeiro, altera a posição local do objeto para que ele mantenha sua posição global.
 */
void unparentObject(object * child, bool keepGlobalPosition);

/**
 * \brief Atualiza todos os objetos dados no argumento.
 * 
 * \param targetObjects vetor de objetos que serão atualizados.
 * \param objectCount número de objetos no vetor.
 */
void coreUpdate();

/**
 * \brief Atualiza a posição de um objeto em específico.
 * 
 * \param targetObject objeto que terá a posição atualizada.
 */
void updateObjectPosition(object * targetObject);

/**
 * \brief Inicializa o sistema de núcleo.
 */
void coreInit();

/**
 * \brief Finaliza os sistemas de núcleo.
 */
void coreShut();

#endif