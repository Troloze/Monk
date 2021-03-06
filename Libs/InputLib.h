#ifndef __InputLib_Tro
#define __InputLib_Tro

#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "MiscLib.h"
#include "ErrorLib.h"

#define Key_NotPressed 0    // Para ser usado com os novos estados de teclado, representa uma tecla não pressionada.
#define Key_JustPressed 1   // Para ser usado com os novos estados de teclado, representa uma tecla que acabou de ser pressionada.
#define Key_Pressed 2       // Para ser usado com os novos estados de teclado, representa uma tecla pressionada.
#define Key_JustReleased 3  // Para ser usado com os novos estados de teclado, representa uma tecla que acabou de ser solta.

#define Axis_Ammount 10     // Número máximo de eixos a serem usados.
#define Trigger_Ammount 20  // Número máximo de gatilhos a serem usados.


typedef struct inputAxis{
    char * name;        // Nome do eixo.
    double value;       // Varia entre -1 e 1;
    Uint8 weight;       // Taxa de variação do valor. 
    short int posKey;   // Guarda o valor da tecla que irá aumentar o valor quando precionada.
    short int negKey;   // Guarda o valor da tecla que irá reduzir o valor quando precionada.
} inputAxis;

typedef struct inputTrigger{
    char * name;            // Nome do gatilho.
    Uint8 value;            // Valor do gatilho. 0 significa solto, 1 acabou de ser engatilhado, 2 precionado, 3 acabou de ser solto.
    short int triggerKey;   // Guarda o valor da tecla associada ao gatilho.
} inputTrigger;

typedef struct inputMouse{
    int x;
    int y;
    Uint8 leftButtonState;
    Uint8 rightButtonState;
    Uint8 middleButtonState;
} inputMouse;



/**
 * \brief Função retorna o estado do teclado, mostrando os estados de todo o teclado.
 */
void createNewKeyStates();

/**
 * \brief Atualiza o estado da entrada.
 * 
 * \param oldState Antigo estado da entrada.
 * \param newState Se a entrada está sendo pressionada ou não.
 * 
 * \return O estado atualizado com base no estado anterior e atual.
 */
int updateState(int oldState, bool newState);

/**
 * \brief Essa função obtem um eixo num vetor, a partir de seu nome.
 * 
 * \param name Nome do eixo a ser obtido.
 * 
 * \return Eixo cujo nome foi dado como parâmetro.
 */
inputAxis getAxis(char * name);

/**
 * \brief Essa função obtem um gatilho num vetor, a partir de seu nome.
 * 
 * \param name Nome do gatilho a ser obtido.
 * 
 * \return Gatilho cujo nome foi dado como parâmetro.
 */
inputTrigger getTrigger(char * name);

/**
 * \brief Função que retorna o estado do mouse.
 */
inputMouse getMouse();

/**
 * \brief Cria um novo eixo a partir de seus parâmetros e o retorna.
 * 
 * \param name Nome do novo eixo.
 * \param weight Peso do novo eixo.
 * \param posKey Valor da tecla positiva do eixo.
 * \param negKey Valor da tecla negativa do eixo.
 * 
 * \return Um novo eixo criado a partir dos parâmetros.
 */
void createAxis(char * name, Uint8 weight, short int posKey, short int negKey);

/**
 * \brief Cria um novo gatilho a partir de seus parâmetros e o retorna.
 * 
 * \param name Nome do novo gatilho.
 * \param triggerKey Valor da tecla do gatilho.
 * 
 * \return Um novo gatilho criado a partir dos parâmetros.
 */
void createTrigger(char * name, short int triggerKey);

/**
 * \brief Destrói o eixo com o nome dado.
 * 
 * \param name Nome do eixo a ser destruído.
 */
void destroyAxis(char * name);

/**
 * \brief Destrói o gatilho com o nome dado.
 * 
 * \param name Nome do gatilho a ser destruído.
 */
void destroyTrigger(char * name);

/**
 * \brief Atualiza todos os eixos num vetor.
 */
void updateAxis();

/**
 * \brief Atualiza todos os gatilhos num vetor.
 */
void updateTrigger();

/**
 * \brief Atualiza o struct que guarda os dados do mouse.
 */
void updateMouse();

/**
 * \brief Atualiza tudo relacionado ao sistema de Input.
 */
void inputUpdate();

/**
 * \brief Inicializa o sistema de Entrada.
 */
bool inputInit();

/**
 * \brief Finaliza o sistema de Entrada.
 */
void inputShut();

/**
 * \brief Anula todos os eixos.
 */
void resetAxis();

/**
 * \brief Anula todos os gatilhos.
 */
void resetTrigger();

#endif