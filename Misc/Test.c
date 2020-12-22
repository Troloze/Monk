#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>

#undef main

typedef struct data {
    char name[16];
    int value;
} data;

int main() {
    unsigned char a[8] = {254, 254, 254, 254, 254, 254, 254, 254};
    int * b = ((int *) &a[0]);

    a[5] = 1;
        
    return 0;
}