#ifndef __CORE_TRO
#define __CORE_TRO

#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>


#define OBJECT_TYPE_OBJECT 0
#define OBJECT_TYPE_RENDER_SPRITE 1
#define OBJECT_TYPE_RENDER_METASPRITE 2
#define OBJECT_TYPE_RENDER_CAMERA 3

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
    Sint32 globalX;             // Posição X em relação ao Zero Absoluto.
    Sint32 globalY;             // Posição X em relação ao Zero Absoluto.
    Uint16 parentPos;           // Posição do objeto entre os filiais do parente.
    void * parent;              // Ponteiro para o objeto parente.
    Uint16 childrenCount;       // Número de objetos filiados.
    void ** children;           // Ponteiros para todos os objetos filiados.
    Uint16 message;             // Mensagem para o item associado ao objeto.
    void * arguments;           // Argumentos da mensagem.
} object;

/**
 * \brief Realiza a parentagem entre os objectos
 * 
 * \param parent objeto que será o parente.
 * \param child objeto que será o filiado.
 */
void parentObjects(object * parent, object * child, bool keepGlobalPosition);


object * createObject(Sint32 x, Sint32 y);


void destroyObject(object * targetObject);


void unparentObject(object * child, bool keepGlobalPosition);


void coreUpdate(object ** targetObjects, Uint32 objectCount);


void updateObjectPosition(object * targetObject);

#endif