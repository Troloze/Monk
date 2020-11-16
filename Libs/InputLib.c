// Incluindo as variáveis necessárias para a biblioteca.
#include "InputLib.h"

/**
 * \brief Função retorna o estado do teclado, mostrando os estados de todo o teclado.
 * 
 * \param oks (OldKeyStates) Esse parâmetro mostra o estado anterior do teclado, usado para comparação.
 */
void createNewKeyStates(Uint8 * oks){
    Uint8 newKeyStates[SDL_NUM_SCANCODES];  // Vetor que será retornado com todos os estados.
    const Uint8 * ks = SDL_GetKeyboardState(NULL);  // Representa os estados de teclado dados pelo SDL.
    for(int i = 0; i < SDL_NUM_SCANCODES; i++) newKeyStates[i] = updateState(oks[i], ks[i]);    // Atualiza o estado do botão de scancode i
    for (int i = 0; i < SDL_NUM_SCANCODES; i++) oks[i] = newKeyStates[i]; // Atualizando oks;
} 

/**
 * \brief Atualiza o estado da entrada.
 * 
 * \param oldState Antigo estado da entrada.
 * \param newState Se a entrada está sendo pressionada ou não.
 * 
 * \return O estado atualizado com base no estado anterior e atual.
 */
int updateState(int oldState, bool newState) {
    if(oldState == 0 && newState == 0) return Key_NotPressed;           // Se anteriormente solto, e ainda solto, retorna solto.
    else if(oldState == 0 && newState == 1) return Key_JustPressed;     // Se anteriormente solto, e agora pressionado, retorna acabou de ser precionado.
    else if(oldState == 1 && newState == 0) return Key_JustReleased;    // Se anteriormente acabou de ser pressionado, e agora solto, retorna acabou de ser solto.
    else if(oldState == 1 && newState == 1) return Key_Pressed;         // Se anteriormente acabou de ser pressionado, e ainda pressionado, retorna pressionado.
    else if(oldState == 2 && newState == 0) return Key_JustReleased;    // Se anteriormente pressionado, e agora solto, retorna acabou de ser solto
    else if(oldState == 2 && newState == 1) return Key_Pressed;         // Se anteriormente pressionado, e ainda pressionado, retorna pressionado.
    else if(oldState == 3 && newState  == 0) return Key_NotPressed;     // Se anteriormente acabou de ser solto, e ainda solto, retorna solto.
    else if(oldState == 3 && newState == 1) return Key_JustPressed;     // Se anteriormente acabou de ser solto, e agora acabou de ser pressionado, retorna acabou de ser pressionado.
}

/**
 * \brief Essa função obtem um eixo num vetor, a partir de seu nome.
 * 
 * \param array Vetor cujo eixo será extraído.
 * \param name Nome do eixo a ser obtido.
 * 
 * \return Eixo cujo nome foi dado como parâmetro.
 */
inputAxis getAxis(inputAxis * array, char * name) {
    inputAxis getter;
    inputAxis nAxis;
    nAxis.name = "NULL";
    nAxis.weight = 0;
    nAxis.value = 0;
    nAxis.posKey = 0;
    nAxis.negKey = 0;
    bool g = false;
    for (int i = 0; i < Axis_Ammount; i++) {
        if(array[i].name == name){
            getter = array[i];
            g = true;
            break;
        }
    }
    if(g){
        return getter;
    } else {
        printf("Não pode obter eixo. Eixo com nome \"%s\" não encontrado.\n", name);
        return nAxis;
    }
}

/**
 * \brief Essa função obtem um gatilho num vetor, a partir de seu nome.
 * 
 * \param array Vetor cujo gatilho será extraído.
 * \param name Nome do gatilho a ser obtido.
 * 
 * \return Gatilho cujo nome foi dado como parâmetro.
 */
inputTrigger getTrigger(inputTrigger * array, char * name) {
    inputTrigger getter;
    inputTrigger nTrigger;
    nTrigger.name = "NULL";
    nTrigger.triggerKey = 0;
    nTrigger.value = 0;
    bool g = false;
    for (int i = 0; i < Trigger_Ammount; i++) {
        if(array[i].name == name) {
            getter = array[i];
            g = true;
            break;
        }
    }
    if(g){
        return getter;
    } else {
        printf("Não pode obter gatilho. Gatilho com nome \"%s\" não encontrado.\n", name);
        return nTrigger;
    }
}

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
inputAxis createAxis(char * name, Uint8 weight, short int posKey, short int negKey) {
    inputAxis newAxis;
    newAxis.name = malloc(sizeof(name));
    newAxis.name = name;
    newAxis.weight = weight;
    newAxis.posKey = posKey;
    newAxis.negKey = negKey;
    newAxis.value = 0;
    return newAxis;
}

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
inputTrigger createTrigger(char * name, short int triggerKey) {
    inputTrigger newTrigger;
    newTrigger.name = malloc (sizeof(name));
    newTrigger.name = name;
    newTrigger.value = 0;
    newTrigger.triggerKey = triggerKey;
    return newTrigger;
}

/**
 * \brief Atualiza todos os eixos num vetor.
 * 
 * \param array Vetor cujos eixos serão atualizados.
 * \param keyStates Vetor com o estado atual do teclado.
 */
void updateAxis(inputAxis * array, const Uint8 * keyStates) {
    Uint8 posKeyState, negKeyState, cWeight;
    double cValue;
    bool isPos, isNeg;

    for(int i = 0; i < Axis_Ammount; i++) {
        posKeyState = keyStates[array[i].posKey];   // Estado da tecla positiva do eixo sendo atualizado.
        negKeyState = keyStates[array[i].negKey];   // Estado da tecla negativa do eixo sendo atualizado.
        cWeight = array[i].weight;  // Peso do eixo sendo atualizado.
        cValue = array[i].value;    // Valor do eixo sendo atualizado.
        isPos = (posKeyState == Key_Pressed || posKeyState == Key_JustPressed); // Se a tecla positiva está precionada isso será verdadeiro.
        isNeg = (negKeyState == Key_Pressed || negKeyState == Key_JustPressed); // Se a tecla negativa está precionada isso será verdadeiro.
        if (isPos && !isNeg) {          // Se somente a tecla positiva está ativa.
            if (cValue != 1) {          // E o valor é diferente de 1.
                if (cValue < 1) cValue += (cWeight + 1)/256.0;
                if (cValue > 1) cValue = 1;
            }
        } else if (isNeg && !isPos) {   // Se somente a tecla negativa está ativa.
            if (cValue != -1){          // E o valor é diferente de -1.
                if (cValue > -1) cValue -= (cWeight + 1)/256.0;
                if (cValue < -1) cValue = -1;
            }
        } else if (isPos == isNeg) {    // Se ambas teclas estão ou ativas ou inativas.
            if (cValue != 0){           // E o valor é diferente de 0.
                if((cWeight + 1)/256.0 >= dAbs(cValue))cValue = 0;
                else if (cValue >= -1 && cValue < 0) cValue += (cWeight + 1)/256.0;
                else if (cValue <= 1 && cValue > 0) cValue -= (cWeight + 1)/256.0;
            }
        }
        array[i].value = cValue;    // Atualizando o valor do Eixo.
    }
}

/**
 * \brief Atualiza todos os gatilhos num vetor.
 * 
 * \param array Vetor cujos gatilhos serão atualizados.
 * \param keyStates Vetor com o estado atual do teclado.
 */
void updateTrigger(inputTrigger * array, const Uint8 * keyState) {
    // Atualizando os valores dos gatilhos.
    for(int i = 0; i < Trigger_Ammount; i++) array[i].value = keyState[array[i].triggerKey];   
}

/**
 * \brief Atualiza o struct que guarda os dados do mouse.
 * 
 * \param mouse Ponteiro para o struct do mouse a ser atualizado.
 * 
 */
void updateMouse(inputMouse * mouse) {
    inputMouse newMouse;    // Objeto mouse que será utilizado para atualizar o mouse dado no parâmetro.
    Uint32 mouseState;      // Variável que irá guardar o estado do mouse, para identificarmos os estados dos botões.
    bool lButton, mButton, rButton;
    mouseState = SDL_GetMouseState(&newMouse.x, &newMouse.y);   // Capturando o mouse.

    if(mouseState & SDL_BUTTON(SDL_BUTTON_LEFT)) lButton = true;    // Se precionando o botão da esquerda, verdadeiro, caso contrário, falso.
    else lButton = false;
    if(mouseState & SDL_BUTTON(SDL_BUTTON_MIDDLE)) mButton = true;  // Se precionando o botão do meio, verdadeiro, caso contrário, falso.
    else mButton = false;
    if(mouseState & SDL_BUTTON(SDL_BUTTON_RIGHT)) rButton = true;   // Se precionando o botão da direita, verdadeiro, caso contrário, falso.
    else rButton = false;

    newMouse.leftButtonState = updateState(mouse->leftButtonState, lButton);    // Atualizando o estado dos três botões.
    newMouse.middleButtonState = updateState(mouse->middleButtonState, mButton);
    newMouse.rightButtonState = updateState(mouse->rightButtonState, rButton);

    *mouse = newMouse;  // Atualizando o mouse para o estado atual.
}

/**
 * \brief Atualiza tudo relacionado ao sistema de Input.
 * 
 * \param keyState Representa o estado do teclado no frame atual, isso será atualizado.
 * \param tArray Vetor que guarda os gatilhos, os valores de todos os gatilhos serão atualizados.
 * \param aArray Vetor que guarda os eixos, os valores de todos os eixos serão atualizados.
 * \param mouse Variável que guarda os dados do mouse, seus valores serão atualizados.
 */
void inputUpdate(Uint8 * keyStates, inputTrigger * tArray, inputAxis * aArray, inputMouse * mouse) {
    createNewKeyStates(keyStates);      // Atualiza o estado do teclado.
    updateMouse(mouse);                 // Atualiza o estado do mouse.
    updateTrigger(tArray, keyStates);   // Atualiza os gatilhos.
    updateAxis(aArray, keyStates);      // Atualiza os eixos.
}

bool InputInit() {

}