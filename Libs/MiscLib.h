#ifndef __MiscLib_Tro
#define __MiscLib_Tro


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

#endif