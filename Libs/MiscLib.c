#include "MiscLib.h"

/**
 * \brief Função que retorna o valor absoluto de um double.
 * 
 * \param v Valor double cujo valor absoluto será extraído.
 * 
 * \return Valor absoluto do parâmetro v. 
 */
double dAbs(double v) {
    return (v > 0)? v : -v;
}