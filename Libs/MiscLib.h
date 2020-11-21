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
#define dAbs(v) ((v > 0)? v : -v)

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
#define convertValueToByte(bytePos, size, value, byte) (((byte & ~((255 >> (8 - size)) << bytePos)) | (value << bytePos)))

/**
 * \brief Função que obtem um valor de um lugar específico de um byte.
 * 
 * \param bytePos posição do valor a ser obtido.
 * \param valueLenght tamanho do valor a ser obtido.
 * \param byte Byte em que o valor será obtido.
 */
#define getByteValue(bytePos, valueLenght, byte) ((byte >> bytePos) & (255 >> (8 - valueLenght)))

/**
 * \brief Função que verifica se um valor pertence a um intervalo.
 * 
 * \param x Posição do valor.
 * \param x0 Inicio do intervalo.
 * \param xf Fim do intervalo.
 * 
 * \return True caso o valor esteja dentro do intervalo, False caso contrário.
 */
#define isOnInterval( x, x0, xf) ((x < xf && x >= x0) ? true : false)

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
#define isOnArea(x, y, x0, y0, xf, yf) ((isOnInterval(x, x0, xf) && isOnInterval(y, y0, yf)) ? true : false)


#endif