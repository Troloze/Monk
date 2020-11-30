#include "ColorLib.h"

Uint32 colorAdd(Uint32 color1, Uint32 color2) {
    Uint8 a1, a2, r1, r2, g1, g2, b1, b2;
    Uint8 na, nr, ng, nb;
    color32To8(color1, &a1, &r1, &b1, &g1);
    color32To8(color2, &a2, &r2, &g2, &b2);
    na = min(0xFF, a1 + a2);
    nr = min(0xFF, r1 + r2);
    ng = min(0xFF, g1 + g2);
    nb = min(0xFF, b1 + b2);
    color8To32(na, nr, ng, nb, &color1);
    return color1;
}

Uint32 colorMultiply(Uint32 color1, Uint32 color2) {
    Uint8 a1, a2, r1, r2, g1, g2, b1, b2;
    Uint8 na, nr, ng, nb;
    color32To8(color1, &a1, &r1, &g1, &b1);
    color32To8(color2, &a2, &r2, &g2, &b2);
    na = (a1 * a2) / 255;
    nr = (r1 * r2) / 255;
    ng = (g1 * g2) / 255;
    nb = (b1 * b2) / 255;
    color8To32(na, nr, ng, nb, &color1);
    return color1;
}

Uint32 colorSubtract(Uint32 color1, Uint32 color2) {
    Uint8 a1, a2, r1, r2, g1, g2, b1, b2;
    Uint8 na, nr, ng, nb;
    color32To8(color1, &a1, &r1, &b1, &g1);
    color32To8(color2, &a2, &r2, &g2, &b2);
    na = min(0x00, a1 - a2);
    nr = min(0x00, r1 - r2);
    ng = min(0x00, g1 - g2);
    nb = min(0x00, b1 - b2);
    color8To32(na, nr, ng, nb, &color1);
    return color1;
}

Uint32 colorDivide(Uint32 color1, Uint32 color2) {
    Uint8 a1, a2, r1, r2, g1, g2, b1, b2;
    Uint8 na, nr, ng, nb;
    color32To8(color1, &a1, &r1, &b1, &g1);
    color32To8(color2, &a2, &r2, &g2, &b2);
    na = min(0xFF, a1 / ((a2 + 1) / 256));
    nr = min(0xFF, r1 / ((r2 + 1) / 256));
    ng = min(0xFF, g1 / ((g2 + 1) / 256));
    nb = min(0xFF, b1 / ((b2 + 1) / 256));
    color8To32(na, nr, ng, nb, &color1);
    return color1;
}

Uint32 colorAddByValue(Uint32 color, Uint8 value) {
    Uint32 v;
    color8To32(0xFF, value, value, value, &v);
    return colorAdd(color, v);
}

Uint32 colorMultiplyByValue(Uint32 color, Uint8 value) {
    Uint32 v;
    color8To32(0xFF, value, value, value, &v);
    return colorMultiply(color, v);
}

Uint32 colorSubtractByValue(Uint32 color, Uint8 value) {
    Uint32 v;
    color8To32(0xFF, value, value, value, &v);
    return colorSubtract(color, v);
}

Uint32 colorDivideByValue(Uint32 color, Uint8 value) {
    Uint32 v;
    color8To32(0xFF, value, value, value, &v);
    return colorDivide(color, v);
}

void color32To8(Uint32 color, Uint8 * a, Uint8 * r, Uint8 * g, Uint8 * b) {
    *b = color % 256;
    color /= 256;
    *g = color % 256;
    color/= 256;
    *r = color % 256;
    color /= 256;
    *a = color;
}

void color8To32(Uint8 a, Uint8 r, Uint8 g, Uint8 b, Uint32 * color) {
    *color = a;
    *color *= 256;
    *color += r;
    *color *= 256;
    *color += g;
    *color *= 256;
    *color += b;
}