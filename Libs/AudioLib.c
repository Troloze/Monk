#include "AudioLib.h"

bool audioInit() {
    if( Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) < 0) {
        printf("Não pode inicializar o mixer. Erro: %s\n", Mix_GetError());
        return false;
    }

    return true;
}

void audioShut() {
    Mix_Quit();
}