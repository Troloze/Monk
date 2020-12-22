#include "InputLib.h"

static int axisCount;                   // Conta quantos elementos existem em axisArray.
static inputAxis * axisArray;           // Contem todos os eixos.
static int triggerCount;                // Conta quantos elementos existem em triggerArray.
static inputTrigger * triggerArray;     // Contem todos os gatilhos.
static inputMouse mouse;                // Contem a informação do mouse.
static Uint8 keyStates[512];            // Contem a informação do estado Cru das teclas do teclado.
static bool inputInitialized = false;   // Indica se a biblioteca foi ou não inicializada.

void createNewKeyStates(){
    Uint8 newKeyStates[SDL_NUM_SCANCODES];  // Vetor que será retornado com todos os estados.
    const Uint8 * ks = SDL_GetKeyboardState(NULL);  // Representa os estados de teclado dados pelo SDL.
    for(int i = 0; i < SDL_NUM_SCANCODES; i++) newKeyStates[i] = updateState(keyStates[i], ks[i]);    // Atualiza o estado do botão de scancode i
    for (int i = 0; i < SDL_NUM_SCANCODES; i++) keyStates[i] = newKeyStates[i]; // Atualizando oks;
} 

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

inputAxis getAxis(char * name) {
    inputAxis getter;
    bool g = false;
    for (int i = 0; i < axisCount; i++) {
        if(axisArray[i].name == name){
            getter = axisArray[i];
            g = true;
            break;
        }
    }
    if(!g){
        printf("Não pode obter eixo. Eixo com nome \"%s\" não encontrado.\n", name);
    }
    return getter;
}

inputTrigger getTrigger(char * name) {
    inputTrigger getter;
    bool g = false;
    for (int i = 0; i < triggerCount; i++) {
        if(triggerArray[i].name == name) {
            getter = triggerArray[i];
            g = true;
            break;
        }
    }
    if(!g){
        printf("Não pode obter gatilho. Gatilho com nome \"%s\" não encontrado.\n", name);
    }

    return getter;
}

inputMouse getMouse() {
    return mouse;
}

void createAxis(char * name, Uint8 weight, short int posKey, short int negKey) {
    for (int i = 0; i < axisCount; i++) {
        if (axisArray[i].name == name) {
            printf("Couldn't create new axis. There already exists an axis with the name \"%s\".",name);
            return;
        }
    }

    inputAxis newAxis;
    newAxis.name = (char *) malloc(sizeof(name));
    newAxis.name = name;
    newAxis.weight = weight;
    newAxis.posKey = posKey;
    newAxis.negKey = negKey;
    newAxis.value = 0;

    axisArray = realloc(axisArray, sizeof(inputAxis) * (++axisCount));
    axisArray[axisCount - 1] = newAxis;
}

void createTrigger(char * name, short int triggerKey) {
    for (int i = 0; i < triggerCount; i++) {
        if (triggerArray[i].name == name) {
            printf("Couldn't create new trigger. There already exists a trigger with the name \"%s\".",name);
            return;
        }
    }

    inputTrigger newTrigger;
    newTrigger.name = (char *) malloc (sizeof(name));
    newTrigger.name = name;
    newTrigger.value = 0;
    newTrigger.triggerKey = triggerKey;
    
    triggerArray = realloc(triggerArray, sizeof(inputTrigger) * (++triggerCount));
    triggerArray[triggerCount - 1] = newTrigger;
}

void destroyAxis(char * name) {
    bool found = 0;
    for (int i = 0; i < axisCount; i++) {
        if (axisArray[i].name != name) {
            if (found == 1) axisArray[i - 1] = axisArray[i];
        } 
        else found = 1;
        
        if (i == (axisCount - 1) && found == 0) {
            printf("Eixo de nome \"%s\" não encontrado e , portanto, não destruído.\n", name);
            return;
        }
    }

    axisArray = realloc(axisArray, sizeof(inputAxis) * (--axisCount));
}

void destroyTrigger(char * name) {
    bool found = 0;
    for (int i = 0; i < triggerCount; i++) {
        if (triggerArray[i].name != name) {
           if (found == 1) triggerArray[i - 1] = triggerArray[i];
        } 
        else found = 1;

        if (i == (triggerCount - 1) && found == 0) printf("Gatilho de nome \"%s\" não encontrado e , portanto, não destruído.\n", name);
    }

    triggerArray = realloc(triggerArray, sizeof(inputTrigger) * (--triggerCount));
}

void updateAxis() { 
    Uint8 posKeyState, negKeyState, cWeight;
    double cValue;
    bool isPos, isNeg;

    for(int i = 0; i < axisCount; i++) {
        posKeyState = keyStates[axisArray[i].posKey];   // Estado da tecla positiva do eixo sendo atualizado.
        negKeyState = keyStates[axisArray[i].negKey];   // Estado da tecla negativa do eixo sendo atualizado.
        cWeight = axisArray[i].weight;  // Peso do eixo sendo atualizado.
        cValue = axisArray[i].value;    // Valor do eixo sendo atualizado.
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
        axisArray[i].value = cValue;    // Atualizando o valor do Eixo.
    }
}

void updateTrigger() {
    // Atualizando os valores dos gatilhos.
    for(int i = 0; i < triggerCount; i++) triggerArray[i].value = keyStates[triggerArray[i].triggerKey];   
}

void updateMouse() {
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

    newMouse.leftButtonState = updateState(mouse.leftButtonState, lButton);    // Atualizando o estado dos três botões.
    newMouse.middleButtonState = updateState(mouse.middleButtonState, mButton);
    newMouse.rightButtonState = updateState(mouse.rightButtonState, rButton);

    mouse = newMouse;  // Atualizando o mouse para o estado atual.
}

void inputUpdate() {
    if (inputInitialized) {     // Se o sistema foi iniciado.
        createNewKeyStates();   // Atualiza o estado do teclado.
        updateMouse();          // Atualiza o estado do mouse.
        updateTrigger();        // Atualiza os gatilhos.
        updateAxis();           // Atualiza os eixos.
    } else {
        printf("Sistemas de Entrada não inicializados.");
    }
}

bool inputInit() {
    axisCount = 0;
    axisArray = (inputAxis *) malloc(0); 
    if (axisArray == NULL) {
        errorSet("inputInit: Não foi possível alocar o vetor de eixos.");
        return false;
    }

    triggerCount = 0;
    triggerArray = (inputTrigger *) malloc(0);
    if (triggerArray == NULL) {
        errorSet("inputInit: Não foi possível alocar o vetor de gatilhos");
        free(axisArray);
        return false;
    }

    inputInitialized = true;
    return true;
}

void inputShut() {
    free(axisArray);
    free(triggerArray);

    inputInitialized = false;
}

void resetAxis() {
    free(axisArray);
    axisCount = 0;
    axisArray = malloc(0);      
}

void resetTrigger() {    
    free(triggerArray);
    triggerCount = 0;
    triggerArray = malloc(0);
}