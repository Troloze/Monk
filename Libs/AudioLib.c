#include "AudioLib.h"

static audioChannel ** channelArray = NULL;
static audioChunk ** chunkArray = NULL;
static int chunkCount = 0;
static int channelCount = 0;
static object * listener = NULL;

bool audioCreateChannels(int n) {
    if (channelCount != 0) {    // Se já existem canais alocados, retornar erro.
        errorSet("audioCreateChannels: Já existem canais alocados, por favor libere-os com audioFreeChannels antes de criar mais.");
        return false;
    }

    if (Mix_AllocateChannels(n) != n) { // Alocar os canais no SDL_mixer, e checar por erros.
        printf("audioCreateChannels: Mix_AllocateChannels: %s\n", Mix_GetError());
        return false;
    }    
    
    channelArray = malloc(sizeof(audioChannel) * n);    // Alocar espaço pros canais do sistema de áudio.
    if (channelArray == NULL) { // Caso não tenha sido possível alocar a memória, retornar erro.
        errorSet("audioCreateChannels: Não foi possível alocar os canais");
        return false;
    }

    for (int i = 0; i < n; i++) {   // Criar os canais do sistema de áudio.
        channelArray[i] = audioCreateNewChannel(i);

        if (channelArray[i] == NULL) {   // Caso não tenha sido possível criar um canal, liberar os outros e retornar erro.
            for (int j = 0; j < i; j++) {
                free(channelArray[j]);
            }
            printf("%s\n", errorGet());
            Mix_AllocateChannels(0);
            errorSet("audioCreateChannels: Não foi possível criar os canais");
            return false;
        } 
    }

    channelCount = n;   // Determinando o valor da contagem de canais.

    return true;    // Retornando verdadeiro.
}

bool audioDestroyChannels() {
    if (channelCount == 0) return true; // Se não há canais não fazer nada. 

    for (int i = 0; i < channelCount; i++) {    // Destruir os canais locais.
        if (channelArray[i] != NULL) { 
            free(channelArray[i]);
        }
    }
    free(channelArray);         // Liberar o vetor de canais.
    channelArray = NULL;
    Mix_AllocateChannels(0);    // Deasalocar os canais do Mixer.
    channelCount = 0;           // Zerar a contagem.
    return true;
}

audioChannel * audioCreateNewChannel(int i) {
    audioChannel * ret = malloc(sizeof(audioChannel));  // Alocando espaço para o novo canal.
    if (ret == NULL) {  // Caso não tenha sido possível, retornar erro.
        errorSetCritical("audioCreateNewChannel: Não foi possível alocar um novo canal, memória máxima excedida.\n");
        return NULL;
    }

    ret->num = i;
    ret->volume = 128;
    ret->panning = 0;
    ret->status = AUDIO_STATUS_IDLE;

    return ret;
}

audioChannel * audioGetChannel(int n) {
    if (n < 0 || n >= channelCount) {   // Caso o canal requisitado não exista, retornar erro.
        errorSet("audioGetChannel: Canal não existe");
        return NULL;
    }
    return channelArray[n]; // Retornar o canal.
}

object * getListener() {
    return listener;
}

audioChunk * audioLoadChunk(char * path, Uint8 volume) {
    audioChunk * newChunk = Mix_LoadWAV(path);  // Carregando o arquivo de áudio.
    if (newChunk == NULL) { // Caso não tenha sido possível, retornar um erro.
        errorSet("audioLoadChunk: Não foi possível carregar o arquivo, utilize Mix_GetError() para mais informações.");
        return NULL;
    }
    
    chunkArray = realloc(chunkArray, sizeof(audioChunk *) * ++chunkCount); // Alocar espaço pro chunk no dump.
    if (chunkArray == NULL) {   // Caso não tenha sido possível, retornar um erro.
        errorSet("audioLoadChunk: Não foi possível realocar o vetor de chunks.");
        Mix_FreeChunk(newChunk);
        return NULL;
    }
    chunkArray[chunkCount - 1] = newChunk;  // Passar o novo chunk para o dump.

    Mix_VolumeChunk(newChunk,min(volume, 128)); // Setar o volume do novo chunk;

    return newChunk;    // Retornar o chunk.
}

bool audioUpdate() {
    audioChannel * currentChannel; 
    Uint8 distance;
    Sint16 angle;

    for (int i = 0; i < channelCount; i++) {    // Para todo canal:
        if (channelArray[i] == NULL) {  // Se for nulo, alguma coisa está errada. Retornar erro.
            errorSet("Audio Update: Um erro não identificado aconteceu.");
            return false;
        }
        currentChannel = channelArray[i];   
        if (Mix_Playing(i)) {   // Descobrir o estado do canal atual.
            if (Mix_Paused(i)) currentChannel->status = AUDIO_STATUS_PAUSED;
            else currentChannel->status = AUDIO_STATUS_PLAYING;
        } 
        else currentChannel->status = AUDIO_STATUS_IDLE;

        if (currentChannel->volume > 128) currentChannel->volume = 128; // Limitar o volume do canal.
        Mix_Volume(i, currentChannel->volume);  // Setar o volume do canal.
        Mix_SetPanning(i, 127 - currentChannel->panning, 127 + currentChannel->panning);    // Setar o panning do canal.
        
    }
    return true;    // Retornar verdadeiro.
}

bool audioInit() {
    listener = createObject(0, 0, OBJ_MASK_OBJ, OBJ_TYPE_OBJECT, listener, NULL);   // Criar objeto "ouvinte".
    if (listener == NULL) { // Caso não tenha sido possível, retornar um erro.
        printf("%s\n");
        errorSet("audioInit: Não foi possível criar objeto foco.");
        return false;
    }
    
    Mix_Init(0);

    if( Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) < 0) {    // Abrir o áudio, retornar um erro caso impossível.
        printf("audioInit: SDL_Mixer: Não pode inicializar o mixer. Erro: %s\n", Mix_GetError());
        return false;
    }   

    chunkArray = malloc(0); // Preparando o dump para poder ser aumentado.
    if (chunkArray == NULL) {   // Caso não tenha sido possível, retornar um erro.
        errorSet("audioInit: Não foi possível alocar o vetor de chunks.");
        return false;
    }

    return true;    // Retornar verdadeiro.
}

void audioShut() {
    audioDestroyChannels(); // Destruir os canais de áudio.
    for (int i = 0; i < chunkCount; i++) {  // Limpar o dump de chunks.
        Mix_FreeChunk(chunkArray[i]);
        chunkArray[i] = NULL;
    }
    free(chunkArray);   // Limpar o vetor do dump.
    Mix_Quit();
}