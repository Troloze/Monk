#ifndef __GameImport_Tro
#define __GameImport_Tro

// Aqui se colocam as definições do sistema:

// #define ERROR_REPORT
#define __LOGO
// #define PRINT_FPS

#include "Libs/MainInclude.h"

bool monkStart(bool * running);

bool monkAfterLogo();

bool monkOnLogoUpdate();

bool monkUpdate(Uint32 c);

bool monkExit();

#endif