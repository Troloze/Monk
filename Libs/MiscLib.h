#ifndef __MiscLib_Tro
#define __MiscLib_Tro

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#define _USE_MATH_DEFINES
#include <math.h>

#ifndef max
#define max(a,b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#define rad2deg(a) (a * 180.0 / M_PI)

#define deg2rad(a) (a * M_PI / 180.0)

#define dabs(v) ((v > 0) ? v : -v)

#define square(v) (v * v)

/**
 * \brief Macro que faz o valor absoluto de um valor v.
 * 
 * \param v Valor double cujo valor absoluto será extraído.
 * 
 * \return Valor absoluto do parâmetro v. 
 */
#define dAbs(v) ((v > 0)? v : -v)

/**
 * \brief Macro que adiciona um valor em um lugar específico de um byte.
 * 
 * \param bytePos Posição que o valor novo será colocado.
 * \param size Tamanho do valor a ser adicionado.
 * \param value Valor a ser posicionado dentro do byte.
 * \param byte Byte que terá o valor posicionado.
 * 
 * \return o valor do byte com o valor na posição desejada.
 */
#define setByteValue(bytePos, size, value, byte) (((byte & ~((255 >> (8 - size)) << bytePos)) | (value << bytePos)))

/**
 * \brief Macro que obtem um valor de um lugar específico de um byte.
 * 
 * \param bytePos posição do valor a ser obtido.
 * \param valueLenght tamanho do valor a ser obtido.
 * \param byte Byte em que o valor será obtido.
 */
#define getByteValue(bytePos, valueLenght, byte) ((byte >> bytePos) & (255 >> (8 - valueLenght)))

/**
 * \brief Macro que verifica se um valor pertence a um intervalo.
 * 
 * \param x Posição do valor.
 * \param x0 Inicio do intervalo.
 * \param xf Fim do intervalo.
 * 
 * \return True caso o valor esteja dentro do intervalo, False caso contrário.
 */
#define isOnInterval( x, x0, xf) ((x < xf && x >= x0) ? true : false)

/**
 * \brief Macro que verifica se uma coordenada pertence a uma área.
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

#define distanceBetweenTwoPoints(x0, x1, y0, y1) (sqrt(((x1 - x0) * (x1 - x0)) + ((y1 - y0) * (y1 - y0))))

#define dBTP(x0, x1, y0, y1) distanceBetweenTwoPoints(x0, x1, y0, y1)

#define angleBetweenTwoPointsRadians(x0, x1, y0, y1) atan2(y1 - y0,x1 - x0)

#define aBTPR(x0, x1, y0, y1) angleBetweenTwoPointsRadians(x0, x1, y0, y1)

#define angleBetweenTwoPointsDegrees(x0, x1, y0, y1) (aBTPR(x0, x1, y0, y1) * 180.0 / M_PI)

#define aBTPD(x0, x1, y0, y1) angleBetweenTwoPointsDegrees(x0, x1, y0, y1)

#define areaOverlap(x0, y0, w0, h0, x1, y1, w1, h1) ((max((x0 + w0) - x1, (x1 + w1) - x0) < w0 + w1) && (max((y0 + h0) - y1, (y1 + h1) - y0) < h0 + h1))

#define openMem(p, pos, max, size)  memmove(p + pos + 1, p + pos, (max - pos) * size)

#define closeMem(p, pos, max, size) memmove(p + pos, p + pos + 1, (max - pos) * size)

#endif