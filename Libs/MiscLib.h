#ifndef __MiscLib_Tro
#define __MiscLib_Tro

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>


/**
 * \brief Função que retorna o valor absoluto de um double.
 * 
 * \param v Valor double cujo valor absoluto será extraído.
 * 
 * \return Valor absoluto do parâmetro v. 
 */
double dAbs(double v);

/**
 * \brief Função que adiciona um valor em um lugar específico de um byte.
 * 
 * \param bytePos Posição que o valor novo será colocado.
 * \param size Tamanho do valor a ser adicionado.
 * \param value Valor a ser posicionado dentro do byte.
 * \param byte Byte que terá o valor posicionado.
 * 
 * \return o valor do byte com o valor na posição desejada.
 */
unsigned char convertValueToByte(unsigned char bytePos, unsigned char size, unsigned char value, unsigned char byte);

/**
 * \brief Função que obtem um valor de um lugar específico de um byte.
 * 
 * \param bytePos posição do valor a ser obtido.
 * \param valueLenght tamanho do valor a ser obtido.
 * \param byte Byte em que o valor será obtido.
 */
unsigned char getByteValue(unsigned char bytePos, unsigned char valueLenght, unsigned char byte);

/**
 * \brief Função que verifica se uma coordenada pertence a uma área.
 * 
 * \param x Posição x da coordenada.
 * \param y Posição y da coordenada.
 * \param x0 Inicio x da área.
 * \param y0 Inicio y da área.
 * \param xf Fim x da área.
 * \param yf Fim y da área. 
 * 
 * \return True caso a coordenada esteja dentro da área, False caso contrário.
 */
bool isOnArea(int x, int y, int x0, int y0, int xf, int yf);

/**
 * \brief Função que verifica se um valor pertence a um intervalo.
 * 
 * \param x Posição do valor.
 * \param x0 Inicio do intervalo.
 * \param xf Fim do intervalo.
 * 
 * \return True caso o valor esteja dentro do intervalo, False caso contrário.
 */
bool isOnInterval(int x, int x0, int xf);

#endif