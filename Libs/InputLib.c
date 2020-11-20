#include "InputLib.h"

static inputAxis nullAxis;
static inputTrigger nullTrigger;
static inputAxis axisArray[Axis_Ammount];
static inputTrigger triggerArray[Trigger_Ammount];
static inputMouse mouse;
static Uint8 keyStates[512];
static bool inputInitialized = false;

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
    for (int i = 0; i < Axis_Ammount; i++) {
        if(axisArray[i].name == name){
            getter = axisArray[i];
            g = true;
            break;
        }
    }
    if(g){
        return getter;
    } else {
        printf("Não pode obter eixo. Eixo com nome \"%s\" não encontrado.\n", name);
        return nullAxis;
    }
}

inputTrigger getTrigger(char * name) {
    inputTrigger getter;
    bool g = false;
    for (int i = 0; i < Trigger_Ammount; i++) {
        if(triggerArray[i].name == name) {
            getter = triggerArray[i];
            g = true;
            break;
        }
    }
    if(g){
        return getter;
    } else {
        printf("Não pode obter gatilho. Gatilho com nome \"%s\" não encontrado.\n", name);
        return nullTrigger;
    }
}

inputMouse getMouse() {
    return mouse;
}

void createAxis(char * name, Uint8 weight, short int posKey, short int negKey) {
    if (name == "NULL") {// O nome do eixo não pode ser "NULL".
        printf("Nome do eixo não pode ser \"NULL\", novo eixo não pôde ser criado.\n");
        return;
    }
   
    int c = 0;          // Valor do espaço em que o novo eixo será alocado.
    bool found = false; // Caso um espaço seja encontrado, fica verdadeiro.
    for (int i = 0; i < Axis_Ammount; i++) {            // Checando por espaços livres no vetor de eixos.
        if (axisArray[i].name == "NULL") found = true;  // Se houver um espaço livre, a função continua.
        
        if (!found) c++;

        if (axisArray[i].name == name) {                // Checando por eixos com o mesmo nome.
            printf("Eixo com nome \"%s\" já existe, novo eixo não pôde ser criado.", name);
            return;
        }

        if (!found && i == Axis_Ammount - 1) {          // Senão, uma mensagem é enviada, e a função para.
            printf("Número máximo de eixos excedido, novo eixo não pôde ser criado.");
            return;
        }
    }

    inputAxis newAxis;
    newAxis.name = malloc(sizeof(name));
    newAxis.name = name;
    newAxis.weight = weight;
    newAxis.posKey = posKey;
    newAxis.negKey = negKey;
    newAxis.value = 0;

    axisArray[c] = newAxis;
}

void createTrigger(char * name, short int triggerKey) {
    if (name == "NULL") {// O nome do gatilho não pode ser "NULL".
        printf("Nome do gatilho não pode ser \"NULL\", novo gatilho não pôde ser criado.\n");
        return;
    }

    int c = 0;          // Valor do espaço em que o novo gatilho será alocado.
    bool found = false; // Caso um espaço seja encontrado, fica verdadeiro.
    for (int i = 0; i < Trigger_Ammount; i++) {             // Checando por espaços livres no vetor de gatilhos.
        if (triggerArray[i].name == "NULL") found = true;   // Se houver um espaço livre, a função continua.
        
        if (!found) c++;

        if (triggerArray[i].name == name) {                 // Checando por gatilhos com o mesmo nome.
            printf("Gatilho com nome \"%s\" já existe, novo gatilho não pôde ser criado.", name);
            return;
        }
        
        if (!found && i == Trigger_Ammount - 1) {                     // Senão, uma mensagem é enviada, e a função para.
            printf("Número máximo de gatilhos excedido, novo gatilho não pôde ser criado.");
            return;
        }
    }

    inputTrigger newTrigger;
    newTrigger.name = malloc (sizeof(name));
    newTrigger.name = name;
    newTrigger.value = 0;
    newTrigger.triggerKey = triggerKey;
    
    triggerArray[c] = newTrigger;
}

void destroyAxis(char * name) {
    for (int i = 0; i < Axis_Ammount; i++) {
        if (axisArray[i].name == name) {
            axisArray[i] = nullAxis;
            break;
        }
        if (i == Axis_Ammount - 1) printf("Eixo de nome \"%s\" não encontrado e , portanto, não destruído.\n", name);
    }
}

void destroyTrigger(char * name) {
    for (int i = 0; i < Trigger_Ammount; i++) {
        if (triggerArray[i].name == name) {
            triggerArray[i] = nullTrigger;
            break;
        }
        if (i == Trigger_Ammount - 1) printf("Gatilho de nome \"%s\" não encontrado e , portanto, não destruído.\n", name);
    }
}

void updateAxis() {
    Uint8 posKeyState, negKeyState, cWeight;
    double cValue;
    bool isPos, isNeg;

    for(int i = 0; i < Axis_Ammount; i++) if (axisArray[i].name != "NULL"){
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
    for(int i = 0; i < Trigger_Ammount; i++) if (triggerArray[i].name != "NULL") triggerArray[i].value = keyStates[triggerArray[i].triggerKey];   
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

void inputInit() {
    nullAxis.name = malloc(sizeof("NULL"));
    nullAxis.name = "NULL";
    nullAxis.negKey = 0;
    nullAxis.posKey = 0;
    nullAxis.value = 0;
    nullAxis.weight = 0;

    nullTrigger.name = malloc(sizeof("NULL"));
    nullTrigger.name = "NULL";
    nullTrigger.triggerKey = 0;
    nullTrigger.value = 0;

    resetAxis();
    resetTrigger();
    inputInitialized = true;
}

void inputShut() {
    resetAxis();
    resetTrigger();
    inputInitialized = false;
}

void resetAxis() {
    for (int i = 0; i < Axis_Ammount; i++) axisArray[i] = nullAxis;             // Anular todos os eixos.
}

void resetTrigger() {    
    for (int i = 0; i < Trigger_Ammount; i++) triggerArray[i] = nullTrigger;    // Anular todos os gatilhos.
}