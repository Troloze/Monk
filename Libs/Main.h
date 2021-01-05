#ifndef __Main_Tro
#define __Main_Tro

#define _USE_MATH_DEFINES
#define ERROR_REPORT

#include "../GameImport.h"

#ifdef __LOGO
    #define SHOW_LOGO true
#else
    #define SHOW_LOGO false
#endif

#ifdef PRINT_FPS
    #define SHOW_FPS true
#else
    #define SHOW_FPS false
#endif

bool init();
void shut();

#endif