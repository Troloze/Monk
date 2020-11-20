#include "MiscLib.h"

double dAbs(double v) {
    return (v > 0)? v : -v;
}

unsigned char convertValueToByte(unsigned char bytePos, unsigned char size, unsigned char value, unsigned char byte) {
    unsigned char mask = 0b11111111, aux = value;
    for (int i = 0; i < 8 - size; i++) mask >>= 1;

    for (int i = 0; i < bytePos; i++) { // Colocando o valor e a máscara na posição desejada.
        value <<= 1;
        mask <<= 1;
    }

    mask ^= 0b11111111; // Invertendo a máscara (e.g. antes -> 0b00110000; depois -> 0b11001111)

    byte &= mask; // Limpando os bits na posição do novo valor.

    byte |= value; // Colocando o novo valor na posição que acabou de ser limpa.

    return byte;
}

unsigned char getByteValue(unsigned char bytePos, unsigned char valueLenght, unsigned char byte) {
    unsigned char mask = 0b11111111;

    for (int i = 0; i < 8 - valueLenght; i++) mask >>= 1;   // Colocando a máscara para pegar somente o que está no espaço do valor e nada mais.

    for (int i = 0; i < bytePos; i++) byte >>= 1;   // Colocando o valor na posição 0;

    return byte & mask; // Removendo a parte desnecessária do valor e retornando.
}

bool isOnArea(int x, int y, int x0, int y0, int xf, int yf) {
    if (isOnInterval(x, x0, xf) && isOnInterval(y, y0, yf)) return true;
    return false;
}

bool isOnInterval(int x, int x0, int xf) {
    if (x < xf && x >= x0) return true;
    return false;
}