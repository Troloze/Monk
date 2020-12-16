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
    FILE * newFile = NULL;
    int test, n, c;
    char name[16];
    data a[5];
    strcpy(a[0].name, "Hello");
    a[0].value = 1;
    strcpy(a[1].name, "This");
    a[1].value = 2;
    strcpy(a[2].name, "Is");
    a[2].value = 3;
    strcpy(a[3].name, "A");
    a[3].value = 4;
    strcpy(a[4].name, "Test");
    a[4].value = 5;

    newFile = fopen("Testing.banana", "w");
    fprintf(newFile, "%d", 5);
    for (int i = 0; i < 5; i++) {
        fprintf(newFile,"%d:%u:%s\n", a[i].value, 10, a[i].name);
    }
    fclose(newFile);

    newFile = fopen("Testing.banana", "r");
    fscanf(newFile, "%d\n", &n);
    for (int i = 0; i < n; i++) {
        fscanf(newFile, "%d:%d:%[^\n]%*c\n", &test, &c, name);
        printf("%s:%d:%d\n", name, test, c);
    }
    
    return 0;
}