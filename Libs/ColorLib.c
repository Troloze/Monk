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

Uint32 mergeAlphat(Uint32 a, Uint32 b) {
    Uint32 ret;
    double ar;
    Uint8 rr, gr, br,
    aa = ((a & 0xFF000000) >> (3*8)), 
    ab = ((b & 0xFF000000) >> (3*8)), 
    ra = ((a & 0xFF0000) >> (2*8)), 
    rb = ((b & 0xFF0000) >> (2*8)), 
    ga = ((a & 0xFF00) >> (8)), 
    gb = ((b & 0xFF00) >> (8)), 
    ba = ((a & 0xFF)),
    bb = ((b & 0xFF));

    ar = ((aa/255.0) + (ab/255.0) * (1 - (aa/255.0)));
    rr = 255 * ((((ra/255.0) * ((aa/255.0))) + ((rb/255.0) * (ab/255.0) * (1 - (aa/255.0)))) / ar);
    gr = 255 * ((((ga/255.0) * ((aa/255.0))) + ((gb/255.0) * (ab/255.0) * (1 - (aa/255.0)))) / ar);
    br = 255 * ((((ba/255.0) * ((aa/255.0))) + ((bb/255.0) * (ab/255.0) * (1 - (aa/255.0)))) / ar);
    
    ret = (((Uint8)(255 * ar)) << 3 * 8) | (rr << 2 * 8) | (gr << 8) | br;
    
    return ret;
}