#ifndef __GameImport_Tro
#define __GameImport_Tro

// Aqui se colocam as definições do sistema:

// #define ERROR_REPORT
#define __LOGO
// #define PRINT_FPS

#include "Libs/MainInclude.h"

/**
 * \brief Função inicial, ela será chamada no começo do programa.
 * 
 * \param running Ponteiro para a variável responsável por fechar o programa, basta muda-la para false e o programa irá encerrar.
 * 
 * \return É esperado que se retorne true, caso contrário o programa irá identificar como um erro e não irá prosseguir.
 */
bool monkStart(bool * running);

/**
 * \brief Função que será chamada após a logo terminar de se mostrar. Caso a logo esteja desativada, será chamada logo após a monkStart().
 * 
 * \return É esperado que se retorne true, caso contrário o programa irá identificar como um erro e não irá prosseguir. 
 */
bool monkAfterLogo();

/**
 * \brief Função que irá ser chamada todo frame enquanto a logo estiver na tela. Não será chamada caso a logo esteja desativada.
 * 
 * \return É esperado que se retorne true, caso contrário o programa irá identificar como um erro e não irá prosseguir.
 */
bool monkOnLogoUpdate();


/**
 * \brief Função que irá ser chamada todo frame após a logo terminar de se mostrar. Caso a logo esteja desativada, será chamada todos os frames desde o início do programa.
 * 
 * \param c Valor que aumenta em um a cada frame que passa.
 * 
 * \return É esperado que se retorne true, caso contrário o programa irá identificar como um erro e não irá prosseguir.
 */
bool monkUpdate(Uint32 c);


/**
 * \brief Função que irá ser chamada logo antes do programa fechar.
 * 
 * \return É esperado que se retorne true, mas não faz muita diferença já que o programa vai fechar de qualquer forma.
 */
bool monkExit();

#endif