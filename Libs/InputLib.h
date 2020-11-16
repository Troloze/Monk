#ifndef __InputLib_Tro
#define __InputLib_Tro

#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "MiscLib.h"

#define Key_NotPressed 0    // Para ser usado com os novos estados de teclado, representa uma tecla não pressionada.
#define Key_JustPressed 1   // Para ser usado com os novos estados de teclado, representa uma tecla que acabou de ser pressionada.
#define Key_Pressed 2       // Para ser usado com os novos estados de teclado, representa uma tecla pressionada.
#define Key_JustReleased 3  // Para ser usado com os novos estados de teclado, representa uma tecla que acabou de ser solta.

#define Axis_Ammount 10     // Número máximo de eixos a serem usados.
#define Trigger_Ammount 20  // Número máximo de gatilhos a serem usados.


/**
 * \brief Esse tipo de dado representa um eixo de entrada.
 * 
 * \param name String que representa o nome do eixo.
 * \param value Valor atual do eixo, atualizado todos os frames.
 * \param weight Taxa de variação do valor, com valor máximo de 255.
 * \param posKey Valor da tecla que irá aumentar o valor, quando precionada.
 * \param negKey Valor da tecla que irá reduzir o valor, quando precionada.
 * 
 */
typedef struct inputAxis{
    char * name;        // Nome do eixo.
    double value;       // Varia entre -1 e 1;
    Uint8 weight;       // Taxa de variação do valor. 
    short int posKey;   // Guarda o valor da tecla que irá aumentar o valor quando precionada.
    short int negKey;   // Guarda o valor da tecla que irá reduzir o valor quando precionada.
} inputAxis;

/**
 * \brief Esse tipo de dado representa um gatilho.
 * 
 * \param name String que representa o nome do gatilho.
 * \param value Valor do gatilho.
 * \param triggerKey Valor da tecla que é associada ao gatilho.
 */
typedef struct inputTrigger{
    char * name;            // Nome do gatilho.
    Uint8 value;            // Valor do gatilho. 0 significa solto, 1 acabou de ser engatilhado, 2 precionado, 3 acabou de ser solto.
    short int triggerKey;   // Guarda o valor da tecla associada ao gatilho.
} inputTrigger;

/**
 * \brief Esse tipo de dado representa o mouse.
 * 
 * \param x Posição x do mouse.
 * \param y Posição y do mouse.
 * \param leftButtonState Estado do botão esquerdo do mouse.
 * \param middleButtomState Estado do botão do meio do mouse.
 * \param rightButtomState Estado do botão direito do mouse.
 */
typedef struct inputMouse{
    int x;
    int y;
    Uint8 leftButtonState;
    Uint8 rightButtonState;
    Uint8 middleButtonState;
} inputMouse;



/**
 * \brief Função retorna o estado do teclado, mostrando os estados de todo o teclado.
 * 
 * \param oks (OldKeyStates) Esse parâmetro mostra o estado anterior do teclado, usado para comparação.
 */
void createNewKeyStates(Uint8 * oks);

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
 * \param array Vetor cujo eixo será extraído.
 * \param name Nome do eixo a ser obtido.
 * 
 * \return Eixo cujo nome foi dado como parâmetro.
 */
inputAxis getAxis(inputAxis * array, char * name);

/**
 * \brief Essa função obtem um gatilho num vetor, a partir de seu nome.
 * 
 * \param array Vetor cujo gatilho será extraído.
 * \param name Nome do gatilho a ser obtido.
 * 
 * \return Gatilho cujo nome foi dado como parâmetro.
 */
inputTrigger getTrigger(inputTrigger * array, char * name);

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
inputAxis createAxis(char * name, Uint8 weight, short int posKey, short int negKey);

/**
 * \brief Cria um novo gatilho a partir de seus parâmetros e o retorna.
 * 
 * \param name Nome do novo gatilho.
 * \param weight Peso do novo gatilho.
 * \param posKey Valor da tecla positiva do gatilho.
 * \param negKey Valor da tecla negativa do gatilho.
 * 
 * \return Um novo gatilho criado a partir dos parâmetros.
 */
inputTrigger createTrigger(char * name, short int triggerKey);

/**
 * \brief Atualiza todos os eixos num vetor.
 * 
 * \param array Vetor cujos eixos serão atualizados.
 * \param keyStates Vetor com o estado atual do teclado.
 */
void updateAxis(inputAxis * array, const Uint8 * keyStates);

/**
 * \brief Atualiza todos os gatilhos num vetor.
 * 
 * \param array Vetor cujos gatilhos serão atualizados.
 * \param keyStates Vetor com o estado atual do teclado.
 */
void updateTrigger(inputTrigger * array, const Uint8 * keyState);

/**
 * \brief Atualiza o struct que guarda os dados do mouse.
 * 
 * \param mouse Ponteiro para o struct do mouse a ser atualizado.
 * 
 */
void updateMouse(inputMouse * mouse);

/**
 * \brief Atualiza tudo relacionado ao sistema de Input.
 * 
 * \param keyState Representa o estado do teclado no frame atual, isso será atualizado.
 * \param tArray Vetor que guarda os gatilhos, os valores de todos os gatilhos serão atualizados.
 * \param aArray Vetor que guarda os eixos, os valores de todos os eixos serão atualizados.
 * \param mouse Variável que guarda os dados do mouse, seus valores serão atualizados.
 */
void inputUpdate(Uint8 * keyStates, inputTrigger * tArray, inputAxis * aArray, inputMouse * mouse);

#endif