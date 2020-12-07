#ifndef __ColorLib_Tro
#define __ColorLib_Tro

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include "MiscLib.h"

#define color256ToDec(c) ( c / 255.0)
#define colorDecTo256(c) ((Uint8) c * 255)

/**
 * \brief Função que soma duas cores.
 * 
 * \param color1 Uma das cores a ser somada.
 * \param color2 Outra cor que será somada.
 * 
 * \return A cor que é resultado da soma das outras duas.
 */
Uint32 colorAdd(Uint32 color1, Uint32 color2);

/**
 * \brief Função que multiplica duas cores.
 * 
 * \param color1 Uma das cores a ser multiplicada.
 * \param color2 Outra cor que será multiplicada.
 * 
 * \return A cor que é resultado da multiplicação das outras duas.
 */
Uint32 colorMultiply(Uint32 color1, Uint32 color2);

/**
 * \brief Função que subtrai duas cores.
 * 
 * \param color1 Uma das cores a ser subtraida.
 * \param color2 Outra cor que será subtraida.
 * 
 * \return A cor que é resultado da subtração das outras duas.
 */
Uint32 colorSubtract(Uint32 color1, Uint32 color2);

/**
 * \brief Função que divide duas cores.
 * 
 * \param color1 Uma das cores a ser dividida.
 * \param color2 Outra cor que será dividida.
 * 
 * \return A cor que é resultado da divisão das outras duas.
 */
Uint32 colorDivide(Uint32 color1, Uint32 color2);

/**
 * \brief Função que adiciona um valor à uma cor.
 * 
 * \param color Cor que terá o valor adicionado.
 * \param value Valor que será adicionado à cor.
 * 
 * \return A cor que é resultado da soma.
 */
Uint32 colorAddByValue(Uint32 color, Uint8 value);

/**
 * \brief Função que multiplica um valor à uma cor.
 * 
 * \param color Cor que terá o valor multiplicado.
 * \param value Valor que será multiplicado à cor.
 * 
 * \return A cor que é resultado do produto.
 */
Uint32 colorMultiplyByValue(Uint32 color, Uint8 value);

/**
 * \brief Função que subtrai um valor de uma cor.
 * 
 * \param color Cor que terá o valor subtraido.
 * \param value Valor que será subtraido à cor.
 * 
 * \return A cor que é resultado da diferença.
 */
Uint32 colorSubtractByValue(Uint32 color, Uint8 value);

/**
 * \brief Função que divide uma cor por um valor.
 * 
 * \param color Cor que terá o valor dividido.
 * \param value Valor que dividirá a cor.
 * 
 * \return A cor que é resultado da divisão.
 */
Uint32 colorDivideByValue(Uint32 color, Uint8 value);

/**
 * \brief Função que converte uma cor em 4 bytes para os 4 canais de 1 byte.
 * 
 * \param color Cor que será convertida.
 * \param a canal Alfa (transparência).
 * \param r canal Red (vermelho).
 * \param g canal Green (verde).
 * \param b canal Blue (azul).
 */
void color32To8(Uint32 color, Uint8 * a, Uint8 * r, Uint8 * g, Uint8 * b);

/**
 * \brief Função que converte 4 canais de 1 byte em uma cor de 4 bytes.
 * 
 * \param a canal Alfa (transparência).
 * \param r canal Red (vermelho).
 * \param g canal Green (verde).
 * \param b canal Blue (azul).
 * \param color Cor representada pela conversão dos canais.
 */
void color8To32(Uint8 a, Uint8 r, Uint8 g, Uint8 b, Uint32 * color);

#endif