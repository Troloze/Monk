#ifndef __AudioLib_Tro
#define __AudioLib_Tro

// Definições de tipo de objeto.
#define OBJ_MASK_AUDIO 1        
#define AUDIO_TYPE_CHANNEL 0

#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include "Core.h"
#include "ErrorLib.h"
#include "MiscLib.h"

#define AUDIO_STATUS_PLAYING 0
#define AUDIO_STATUS_PAUSED 1
#define AUDIO_STATUS_IDLE 2

typedef struct audioChannel {
    int num; 
    bool world;
    Uint8 volume;
    Sint8 panning;
    Uint8 status;
    object * obj;
} audioChannel;

typedef Mix_Chunk audioChunk;

/**
 * \brief Faz um dos canais de audio tocar um som.
 * 
 * \param n Número do canal (ou -1 para o primeiro disponível).
 * \param c Chunk com o som a ser tocado.
 * \param l Quantidade de loops (-1 para ser infinito).
 * 
 * \return Canal de som que está tocando.
 */
#define audioPlay(n, c, l) Mix_PlayChannel(n, c, l)

/**
 * \brief Faz um dos canais de audio tocar um som por um tempo determinado.
 * 
 * \param n Número do canal (ou -1 para o primeiro disponível).
 * \param c Chunk com o som a ser tocado.
 * \param l Quantidade de loops (-1 para ser infinito).
 * \param t Tempo em que o canal irá parar de tocar.
 * 
 * \return Canal de som que está tocando.
 */
#define audioPlayTimed(n, c, l, t) Mix_PlayChannelTimed(n, c, l, t)

/**
 * \brief Faz um dos canais de audio tocar um som, com uma entrada gradiente.
 * 
 * \param n Número do canal (ou -1 para o primeiro disponível).
 * \param c Chunk com o som a ser tocado.
 * \param l Quantidade de loops (-1 para ser infinito).
 * \param ms Tempo, em milisegundos, que a entrada irá demorar para atingir o volume máximo.
 * 
 * \return Canal de som que está tocando.
 */
#define audioFadeIn(n, c, l, ms) Mix_FadeInChannel(n, c, l, ms)

/**
 * \brief Faz um dos canais de audio tocar um som, com uma entrada gradiente, por um tempo determinado.
 * 
 * \param n Número do canal (ou -1 para o primeiro disponível).
 * \param c Chunk com o som a ser tocado.
 * \param l Quantidade de loops (-1 para ser infinito).
 * \param ms Tempo, em milisegundos, que a entrada irá demorar para atingir o volume máximo.
 * \param t Tempo em que o canal irá parar de tocar.
 * 
 * \return Canal de som que está tocando.
 */
#define audioFadeInTimed(n, c, l, ms, t) Mix_FadeInChannelTimed(n, c, l, ms, t)

/**
 * \brief Para um canal de som, com saída gradiente.
 * 
 * \param n Número do canal (ou -1 para fazer isso em TODOS os canais que estão tocando no momento).
 * \param ms Tempo, em milisegundos, em que a saída irá demorar para silenciar.
 * 
 * \return Quantos canais foram afetados (1 ou 0 normalmente, mas podem ter mais caso n seja -1).
 */
#define audioFadeOut(n, ms) Mix_FadeOutChannel(n, ms)

/**
 * \brief Retorna o ponteiro para o chunk que está sendo tocado no canal n.
 * 
 * \param n Canal que terá o chunk que está sendo tocado retornado.
 * 
 * \return O chunk sendo tocado no canal n.
 */
#define audioPlayingChunk(n) Mix_GetChunk(n)

/**
 * \brief Pausa o canal n.
 * 
 * \param n Canal que será pausado.
 */
#define audioPause(n) Mix_Pause(n)

/**
 * \brief Volta a tocar o canal n.
 * 
 * \param n Canal que voltará a tocar.
 */
#define audioResume(n) Mix_Resume(n)

/**
 * \brief Finaliza o audio de um canal n.
 * 
 * \param n Canal que terá o áudio finalizado. Caso -1 todos os canais serão afetados
 * 
 * \return Sempre 0.
 */
#define audioHalt(n) Mix_Halt(n)

/**
 * \brief Cria n canais de áudio.
 * 
 * \param n Quantidade de canais de áudio que serão criadas.
 * 
 * \return True, ou false em caso de erro.
 */
bool audioCreateChannels(int n);

/**
 * \brief Destrói os canais de áudio para poder alocar mais.
 * 
 * \return Sempre true.
 */
bool audioDestroyChannels();

/**
 * \brief Carrega o arquivo de audio.
 * 
 * \param path Caminho do arquivo.
 * \param volume Volume inicial do audio (0 = silêncio; 128 = Mais alto possível).
 * 
 * \return Ponteiro para o chunk criado.
 */
audioChunk * audioLoadChunk(char * path, Uint8 volume);

/**
 * \brief Função interna do sistema de Audio, não deve ser utilizada.
 */
audioChannel * audioCreateNewChannel(int i);

/**
 * \brief Obtém o struct do canal com o índice dado.
 * 
 * \param n Índice do canal esperado.
 * 
 * \return Canal com o índice dado, ou NULL, em caso de erro.
 */
audioChannel * audioGetChannel(int n);

/** 
 * \brief Obtem o objeto do ouvinte.
 * 
 * \return O objeto do ouvinte.
 */
object * getListener();

/**
 * \brief Atualiza o sistema de áudio.
 * 
 * \return True, false em caso de erro.
 */
bool audioUpdate();

/**
 * \brief Inicializa o sistema de áudio.
 * 
 * \return True, false em caso de erro.
 */
bool audioInit();

/**
 * \brief Finaliza o sistema de áudio.
 */
void audioShut();

#endif