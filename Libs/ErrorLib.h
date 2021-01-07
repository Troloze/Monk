#ifndef __ErrorLib_Tro
#define __ErrorLib_Tro

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

#define reportError() printf("%s\n", errorGet())

#ifdef ERROR_REPORT
    #define __ERROR_REPORT true
#else
    #define __ERROR_REPORT false
#endif

/**
 * \brief Função usada pra declarar erros.
 * 
 * \param error String com o texto do erro a ser declarado.
 * 
 * \return True caso a função tenha sido bem sucedida, false caso contrário.
 */
bool errorSet(char * error);

/**
 * \brief Declara um erro crítico.
 */
void errorSetCritical(char * error);

/**
 * \brief Função usada para receber o último erro declarado.
 * 
 * \return Último erro declarado pelo programa.
 */
char * errorGet();

/**
 * \brief Função que inicializa o fim do programa
 */
void shutInit();

/**
 * \brief Função que retorna se o programa está rodando ou não.
 * 
 * \return True caso o programa ainda esteja rodando, false caso contrário
 */
bool runStatus();

/**
 * \brief Atualiza o sistema de erro.
 * 
 * \return True caso esteja tudo bem, false caso um erro crítico tenha sido declarado.
 */
bool errorUpdate();

/**
 * \brief Função de inicialização do sistema de erro.
 * 
 * \return True caso a função tenha sido bem sucedida, false caso contrário.
 */
bool errorInit();

/**
 * \brief Função de finalização do sistema de erro.
 * 
 * \return Sempre true.
 */
bool errorShut();


#endif